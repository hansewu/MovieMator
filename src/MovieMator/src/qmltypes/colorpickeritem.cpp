/*
 * Copyright (c) 2014 Meltytech, LLC
 * Author: Brian Matherly <pez4brian@yahoo.com>
 * Author: Dan Dennedy <dan@dennedy.org>
 * Inspiration: KDENLIVE colorpickerwidget.cpp by Till Theato (root@ttill.de)
 * Inspiration: QColorDialog.cpp
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

#include "colorpickeritem.h"
#include <QTimer>
#include <QIcon>
#include <QMouseEvent>
#include <QApplication>
#include <QGuiApplication>
#include <QDesktopWidget>
#include <QImage>
#include <QScreen>
#include "mainwindow.h"

class EventFilter : public QObject
{
public:
    explicit EventFilter(ScreenSelector *selector, QObject *parent = nullptr)
        : QObject(parent)
        , m_selector(selector)
    {}

    bool eventFilter(QObject *, QEvent *event);
private:
    ScreenSelector *m_selector;
};

bool EventFilter::eventFilter(QObject *, QEvent *event)
{
    Q_ASSERT(event);
    Q_ASSERT(m_selector);
//    qDebug()<<"eventFilter begins";
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        return m_selector->onMousePressEvent(static_cast<QMouseEvent *>(event));
    case QEvent::MouseMove:
        return m_selector->onMouseMoveEvent(static_cast<QMouseEvent *>(event));
    case QEvent::MouseButtonRelease:
        return m_selector->onMouseReleaseEvent(static_cast<QMouseEvent *>(event));
    case QEvent::KeyPress:
        return m_selector->onKeyPressEvent(static_cast<QKeyEvent *>(event));
    default:
        break;
    }
//    qDebug()<<"eventFilter ends";
    return false;
}

ScreenSelector::ScreenSelector(QWidget* parent)
    : QFrame(parent)
    , m_selectionInProgress(false)
    , m_selectionRect()
    , m_eventFilter(nullptr)
{
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setWindowOpacity(0.5);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    hide();
}

void ScreenSelector::startSelection()
{
    m_selectionInProgress = false;
    if (!m_eventFilter)
        m_eventFilter = new EventFilter(this);
    Q_ASSERT(m_eventFilter);
    QApplication::instance()->installEventFilter(m_eventFilter);
    grabMouse();
    grabKeyboard();
    MAIN.setCursor(Qt::CrossCursor);
}

bool ScreenSelector::onMousePressEvent(QMouseEvent *event)
{
    Q_ASSERT(event);
    if (event->button() == Qt::LeftButton && !m_selectionInProgress) {
        m_selectionInProgress = true;
        show();
        m_selectionRect = QRect(event->globalPos(), QSize(1,1));
        setGeometry(m_selectionRect);
    }
    return true;
}

bool ScreenSelector::onMouseMoveEvent(QMouseEvent *event)
{
    Q_ASSERT(event);
    if (m_selectionInProgress) {
        m_selectionRect.setWidth(event->globalX() - m_selectionRect.x());
        m_selectionRect.setHeight(event->globalY() - m_selectionRect.y());

        if (m_selectionRect.width() == 0) {
            m_selectionRect.setWidth(1);
        }
        if (m_selectionRect.height() == 0) {
            m_selectionRect.setHeight(1);
        }
        setGeometry(m_selectionRect.normalized());
    }
    return true;
}

bool ScreenSelector::onMouseReleaseEvent(QMouseEvent *event)
{
    Q_ASSERT(event);
  //  qDebug()<<"ScreenSelector::onMouseReleaseEvent begin";
    if(event->button() == Qt::LeftButton && m_selectionInProgress == true ) {
        release();
        // Give the frame buffer time to clear the selector window before
        // signaling the selection.
        QTimer::singleShot(100, this, SLOT(grabColor()));
    }
 //   qDebug()<<"ScreenSelector::onMouseReleaseEvent end";
    return true;
}

bool ScreenSelector::onKeyPressEvent(QKeyEvent* event)
{
    Q_ASSERT(event);
    if (event->key() == Qt::Key_Escape)
        release();
    event->accept();
    return true;
}

void ScreenSelector::release()
{
    QApplication::instance()->removeEventFilter(m_eventFilter);
    releaseMouse();
    releaseKeyboard();
    MAIN.setCursor(Qt::ArrowCursor);
    m_selectionInProgress = false;
    hide();
}

void ScreenSelector::grabColor()
{
    m_selectionRect = m_selectionRect.normalized();
    QDesktopWidget* desktop = QApplication::desktop();
    Q_ASSERT(desktop);
    int screenNum = desktop->screenNumber(m_selectionRect.topLeft());
    QScreen* screen = QGuiApplication::screens()[screenNum];
    Q_ASSERT(screen);
    QPixmap screenGrab = screen->grabWindow(desktop->winId(),
        m_selectionRect.x(), m_selectionRect.y(), m_selectionRect.width(), m_selectionRect.height());
    QImage image = screenGrab.toImage();
    int numPixel = image.width() * image.height();
    int sumR = 0;
    int sumG = 0;
    int sumB = 0;

    for (int x = 0; x < image.width(); ++x) {
        for (int y = 0; y < image.height(); ++y) {
            QColor color = image.pixel(x, y);
            sumR += color.red();
            sumG += color.green();
            sumB += color.blue();
        }
    }

    QColor avgColor(sumR / numPixel, sumG / numPixel, sumB / numPixel);
    emit colorPicked(avgColor);
}

ColorPickerItem::ColorPickerItem(QObject* parent)
    : QObject(parent)
{
    connect(this, SIGNAL(pickColor()), &m_selector, SLOT(startSelection()));
    connect(&m_selector, SIGNAL(colorPicked(QColor)), SIGNAL(colorPicked(QColor)));
}
