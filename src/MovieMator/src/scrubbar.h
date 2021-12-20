/*
 * Copyright (c) 2011 Meltytech, LLC
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

#ifndef SCRUBBAR_H
#define SCRUBBAR_H

#include <QWidget>

class ScrubBar : public QWidget
{
    Q_OBJECT

    enum controls {
        CONTROL_NONE,
        CONTROL_HEAD,
        CONTROL_IN,
        CONTROL_OUT
    };

public:
    explicit ScrubBar(QWidget *parent = nullptr);

    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event);
    void setScale(int maximum);
    void setFramerate(double fps);
    int position() const;
    void setInPoint(int in);
    void setOutPoint(int out);
    void setMarkers(const QList<int>&);
    QList<int> markers() const {
        return m_markers;
    }

signals:
    void seeked(int);
    void inChanged(int);
    void outChanged(int);
    void oneActionTrimIn(int);
    void oneActionTrimOut(int);

public slots:
    bool onSeek(int value);

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *);
    virtual bool event(QEvent *event);

private:
    int m_cursorPosition;
    int m_head;
    double m_scale;
    double m_fps;
    int m_interval;
    int m_max;
    int m_in;
    int m_preReleaseIn;
    int m_out;
    int m_preReleaseOut;

    enum controls m_activeControl;
    QPixmap m_pixmap;
    int m_timecodeWidth;
    int m_secondsPerTick;
    QList<int> m_markers;

    void updatePixmap();
};

#endif // SCRUBBAR_H
