/*
 * Copyright (c) 2015-2016 Meltytech, LLC
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

#include "timelineitems.h"
#include "mltcontroller.h"

#include <QQuickPaintedItem>
#include <QPainter>
#include <QPalette>
#include <QPainterPath>
#include <QLinearGradient>
#include <QDebug>

class TimelineTransition : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor colorA MEMBER m_colorA NOTIFY propertyChanged)
    Q_PROPERTY(QColor colorB MEMBER m_colorB NOTIFY propertyChanged)

public:
    TimelineTransition()
    {
        setAntialiasing(QPainter::Antialiasing);
        connect(this, SIGNAL(propertyChanged()), this, SLOT(update()));
    }

    void paint(QPainter *painter)
    {
        Q_ASSERT(painter);
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0, m_colorA);
        gradient.setColorAt(1, m_colorB);

        QPainterPath path;
        path.moveTo(0,0);
//        path.lineTo(width(), height());
//        path.lineTo(width(), 0);
//        path.lineTo(0, height());
//        painter->fillPath(path, gradient);
//        painter->strokePath(path, painter->pen());
        path.lineTo(0, height());
        path.lineTo(width(), 0);
        painter->fillPath(path, gradient);

//        QPen pen;
//        pen.setWidth(2);
//        pen.setColor(QColor(255,255,255));

//        path.moveTo(0, 0);
//        path.lineTo(0, height());
//        painter->strokePath(path, pen);

//        path.moveTo(0, 0);
//        path.lineTo(width(), 0);
//        painter->strokePath(path, pen);

    }
signals:
    void propertyChanged();

private:
    QColor m_colorA;
    QColor m_colorB;
};

class TimelinePlayhead : public QQuickPaintedItem
{
    void paint(QPainter *painter)
    {
        Q_ASSERT(painter);

        QPainterPath path;
        path.moveTo(width(), 0);
        path.lineTo(width() / 2.0, height());
        path.lineTo(0, 0);
        QPalette p;
        painter->fillPath(path, p.color(QPalette::Active, QPalette::Highlight));
    }
};

class TimelineTriangle : public QQuickPaintedItem
{
public:
    TimelineTriangle()
    {
        setAntialiasing(QPainter::Antialiasing);
    }
    void paint(QPainter *painter);
};

void TimelineTriangle::paint(QPainter *painter)
{
    Q_ASSERT(painter);

    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(width(), 0);
    path.lineTo(0, height());
    painter->fillPath(path, Qt::black);
}

class TimelineWaveform : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QVariant levels MEMBER m_audioLevels NOTIFY propertyChanged)
    Q_PROPERTY(QColor fillColor MEMBER m_color NOTIFY propertyChanged)
    Q_PROPERTY(int inPoint MEMBER m_inPoint NOTIFY inPointChanged)
    Q_PROPERTY(int outPoint MEMBER m_outPoint NOTIFY outPointChanged)


public:
    TimelineWaveform()
    {
        setMipmap(true);
        setAntialiasing(QPainter::Antialiasing);
        connect(this, SIGNAL(propertyChanged()), this, SLOT(update()));
    }

    void paint(QPainter *painter)
    {
        Q_ASSERT(painter);
        QVariantList levelsData = m_audioLevels.toList();

        if (levelsData.isEmpty())
        {
            return;
        }

        // In and out points are # frames at current fps,
        // but audio levels are created at 25 fps.
        // Scale in and out point to 25 fps.
        const int nInPoint              = qRound(m_inPoint / MLT.profile().fps() * 25.0);
        const int nOutPoint             = qRound(m_outPoint / MLT.profile().fps() * 25.0);
        const qreal indicesPrPixel      = qreal(nOutPoint - nInPoint) / width();

        QPainterPath path;
        path.moveTo(-1, height());
        int i = 0;
        for (; i < width(); ++i)
        {
            int idx = nInPoint + int(i * indicesPrPixel);
            if (idx + 1 >= levelsData.length())
            {
                break;
            }

            qreal level = qMax(levelsData.at(idx).toReal(), levelsData.at(idx + 1).toReal()) / 256;
            path.lineTo(i, height() - level * height());
        }
        path.lineTo(i, height());

//        painter->fillPath(path, m_color.lighter());
        painter->fillPath(path, m_color);

        QPen pen(painter->pen());
        pen.setColor(m_color.darker());
        painter->strokePath(path, pen);
    }

signals:
    void propertyChanged();
    void inPointChanged();
    void outPointChanged();

private:
    QVariant m_audioLevels;
    int m_inPoint;
    int m_outPoint;
    QColor m_color;
};

void registerTimelineItems()
{
    qmlRegisterType<TimelineTransition>("MovieMator.Controls", 1, 0, "TimelineTransition");
    qmlRegisterType<TimelinePlayhead>("MovieMator.Controls", 1, 0, "TimelinePlayhead");
    qmlRegisterType<TimelineTriangle>("MovieMator.Controls", 1, 0, "TimelineTriangle");
    qmlRegisterType<TimelineWaveform>("MovieMator.Controls", 1, 0, "TimelineWaveform");
}

#include "timelineitems.moc"
