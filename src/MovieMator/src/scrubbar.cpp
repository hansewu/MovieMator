/*
 * Copyright (c) 2011-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "scrubbar.h"
#include "mltcontroller.h"
#include <QtWidgets>

static const int margin = 0;        /// left and right margins
// 使用的地方被注释了，消除警告
//static const int selectionSize = 5; /// the height of the top bar
#ifndef CLAMP
#define CLAMP(x, min, max) (((x) < (min))? (min) : ((x) > (max))? (max) : (x))
#endif

ScrubBar::ScrubBar(QSlider *parent)
    : QSlider(parent)
    , m_cursorPosition(0)
    , m_head(-1)
    , m_scale(-1)
    , m_fps(25)
    , m_max(1)
    , m_in(-1)
    , m_out(-1)
    , m_activeControl(CONTROL_NONE)
    , m_timecodeWidth(0)
{
    setMouseTracking(true);
 //   setMinimumHeight(/*fontMetrics().height() +*/ selectionSize);
  //  setStyleSheet("background-color:rgb(255,0,0);");
    QString scrubStyle = "QSlider::sub-page:horizontal{background:rgb(15,114,103);border-radius: 4px}";
    scrubStyle.append("QSlider::groove:horizontal{background:'grey';height:8px;border-radius: 4px }");
    scrubStyle.append("QSlider::handle:horizontal{background:'white';width:5px;margin: -2px 1px -2px 1px;border-radius: 2px}");

    setStyleSheet(scrubStyle);
}


void ScrubBar::setScale(int maximum)
{
    setRange(0, maximum);
    if (!m_timecodeWidth) {
        const int fontSize = font().pointSize() - (font().pointSize() > 10? 2 : (font().pointSize() > 8? 1 : 0));
        setFont(QFont(font().family(), fontSize * devicePixelRatio()));
        m_timecodeWidth = fontMetrics().width("00:00:00:00") / devicePixelRatio();
    }
    m_max = maximum;
    /// m_scale is the pixels per frame ratio
    m_scale = double(width() - 2 * margin) / double(maximum);
    if(qFuzzyIsNull(m_scale)) m_scale = -1;   //if (m_scale == 0)
    m_secondsPerTick = qRound(double(m_timecodeWidth * 1.8) / m_scale / m_fps);
    if (m_secondsPerTick > 3600)
        // force to a multiple of one hour
        m_secondsPerTick += 3600 - m_secondsPerTick % 3600;
    else if (m_secondsPerTick > 300)
        // force to a multiple of 5 minutes
        m_secondsPerTick += 300 - m_secondsPerTick % 300;
    else if (m_secondsPerTick > 60)
        // force to a multiple of one minute
        m_secondsPerTick += 60 - m_secondsPerTick % 60;
    else if (m_secondsPerTick > 5)
        // force to a multiple of 10 seconds
        m_secondsPerTick += 10 - m_secondsPerTick % 10;
    else if (m_secondsPerTick > 2)
        // force to a multiple of 5 seconds
        m_secondsPerTick += 5 - m_secondsPerTick % 5;
    /// m_interval is the number of pixels per major tick to be labeled with time
    m_interval = qRound(double(m_secondsPerTick) * m_fps * m_scale);
    m_head = -1;
//    updatePixmap();
}

void ScrubBar::setFramerate(double fps)
{
    m_fps = fps;
}

int ScrubBar::position() const
{
    return m_head;
}

void ScrubBar::setInPoint(int in)
{
    m_in = qMax(in, -1);
//    updatePixmap();
    emit inChanged(in);
}

void ScrubBar::setOutPoint(int out)
{
    m_out = qMin(out, m_max);
//    updatePixmap();
    emit outChanged(out);
}

void ScrubBar::setMarkers(const QList<int> &list)
{
    m_markers = list;
//    updatePixmap();
}

void ScrubBar::mousePressEvent(QMouseEvent * event)
{

    Q_ASSERT(event);
     int x = event->x() - margin;
//     int in = m_in * m_scale;
//     int out = m_out * m_scale;
//     int head = m_head * m_scale;
     int pos = CLAMP(int(x / m_scale), 0, m_max);

     if (event->button() == Qt::LeftButton)
     {
            if (orientation() == Qt::Vertical)
                setValue(minimum() + ((maximum()-minimum()) * (height()-event->y())) / height() ) ;
            else
                setValue(minimum() + ((maximum()-minimum()) * event->x()) / width() ) ;

            event->accept();
      }

//    if (m_in > -1 && m_out > -1) {
//        if (x >= in - 12 && x <= in + 6) {
//            m_activeControl = CONTROL_IN;
//            setInPoint(pos);
//        }
//        else if (x >= out - 6 && x <= out + 12) {
//            m_activeControl = CONTROL_OUT;
//            setOutPoint(pos);
//        }
//    }
//    if (m_head > -1) {
//        if (m_activeControl == CONTROL_NONE) {
//            m_activeControl = CONTROL_HEAD;
//            m_head = pos;
//            const int offset = height() / 2;
//            const int x = head;
//            const int w = qAbs(x - head);
//         // update(margin + x - offset, 0, w + 2 * offset, height());
//        }
//    }
    emit seeked(pos);
          QSlider::mousePressEvent(event);
}


void ScrubBar::mouseReleaseEvent(QMouseEvent * event)
{
    qDebug()<<"ScrubBar::mouseReleaseEvent begins";
    Q_UNUSED(event)
    m_activeControl = CONTROL_NONE;
    qDebug()<<"ScrubBar::mouseReleaseEvent ends";
}


void ScrubBar::mouseMoveEvent(QMouseEvent * event)
{
    Q_ASSERT(event);
    int x = event->x() - margin;
    int pos = CLAMP(int(x / m_scale), 0, m_max);

    if (event->buttons() & Qt::LeftButton) {
        if (m_activeControl == CONTROL_IN)
            setInPoint(pos);
        else if (m_activeControl == CONTROL_OUT)
            setOutPoint(pos);
        else if (m_activeControl == CONTROL_HEAD) {
            const int head = int(m_head * m_scale);
            const int offset = height() / 2;
            const int x = head;
            const int w = qAbs(x - head);
            update(margin + x - offset, 0, w + 2 * offset, height());
            m_head = pos;
        }
        emit seeked(pos);
    }
}

bool ScrubBar::onSeek(int value)
{
    if (m_activeControl != CONTROL_HEAD)
        m_head = value;
    int oldPos = m_cursorPosition;
    m_cursorPosition = int(value * m_scale);
    const int offset = height() / 2;
    const int x = qMin(oldPos, m_cursorPosition);
    const int w = qAbs(oldPos - m_cursorPosition);
    update(margin + x - offset, 0, w + 2 * offset, height());

    setValue(value);

    return true;
}



void ScrubBar::resizeEvent(QResizeEvent *)
{
    setScale(m_max);
}

//bool ScrubBar::event(QEvent *event)
//{
//    QWidget::event(event);
////    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange)
////        updatePixmap();
//    return false;
//}

/*void ScrubBar::updatePixmap()
{
    qDebug()<<"updatePixmap is called";
    const int ratio = devicePixelRatio();
    const int l_width = width() * ratio;
    const int l_height = height() * ratio;
    const int l_margin = margin * ratio;
    const int l_selectionSize = selectionSize * ratio;
    const int l_interval = m_interval * ratio;
    const int l_timecodeWidth = m_timecodeWidth * ratio;
    m_pixmap = QPixmap(l_width, l_height);
    m_pixmap.fill(palette().window().color());
    QPainter p(&m_pixmap);
    p.setFont(font());
    const int markerHeight = fontMetrics().ascent() + 2 * ratio;
    QPen pen;

    if (!isEnabled()) {
        p.fillRect(0, 0, l_width, l_height, palette().background().color());
        p.end();
        update();
        return;
    }

    // background color

    p.fillRect(2, 0, l_width  , l_height, QColor("grey"));//palette().base().color());
//     p.fillRect(l_margin, 0, pos, l_height, QColor(223,66,10));//palette().highlight().color());


    // selected region
    if (m_in > -1 && m_out > m_in)
    {
        const int in = m_in * m_scale * ratio;
        const int out = m_out * m_scale * ratio;
    //     p.fillRect(l_margin + in, 0, out - in, l_selectionSize, Qt::red);
    //     p.fillRect(l_margin + in + (2 + ratio), ratio, // 2 for the in point line
    //                out - in - 2 * (2 + ratio) - qFloor(0.5 * ratio),
    //                l_selectionSize - ratio * 2,
    //                palette().highlight().color());
    // }

    // // draw time ticks
    // pen.setColor(palette().text().color());
    // pen.setWidth(ratio);
    // p.setPen(pen);
    // if (l_interval > 2) {
    //     for (int x = l_margin; x < l_width - l_margin; x += l_interval) {
    //         p.drawLine(x, l_selectionSize, x, l_height - 1);
    //         if (x + l_interval / 4 < l_width - l_margin)
    //             p.drawLine(x + l_interval / 4,     l_height - 3 * ratio, x + l_interval / 4,     l_height - 1);
    //         if (x + l_interval / 2 < l_width - l_margin)
    //             p.drawLine(x + l_interval / 2,     l_height - 7 * ratio, x + l_interval / 2,     l_height - 1);
    //         if (x + l_interval * 3 / 4 < l_width - l_margin)
    //             p.drawLine(x + l_interval * 3 / 4, l_height - 3 * ratio, x + l_interval * 3 / 4, l_height - 1);
    //     }
    // }

        p.fillRect(l_margin + in, 0, out - in, l_selectionSize, QColor(223,66,10));//palette().highlight().color());
    }


    // draw markers
    if (m_in < 0 && m_out < 0) {
        int i = 1;
        foreach (int pos, m_markers) {
            int x = l_margin + pos * m_scale * ratio;
            QString s = QString::number(i++);
            int markerWidth = fontMetrics().width(s) * 1.5;
            p.fillRect(x, 0, 1, l_height, palette().highlight().color());
            p.fillRect(x - markerWidth/2, 0, markerWidth, markerHeight, palette().highlight().color());
            p.drawText(x - markerWidth/3, markerHeight - 2 * ratio, s);
        }
    }

    p.end();
    update();
}
*/
