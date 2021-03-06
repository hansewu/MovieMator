/*
 * Copyright (c) 2015 Meltytech, LLC
 * Author: Brian Matherly <code@brianmatherly.com>
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

#ifndef AUDIOWAVEFORMSCOPEWIDGET_H
#define AUDIOWAVEFORMSCOPEWIDGET_H

#include "scopewidget.h"
#include <QMutex>
#include <QImage>
#include <QTime>

class AudioWaveformScopeWidget Q_DECL_FINAL : public ScopeWidget
{
    Q_OBJECT
    
public:
    explicit AudioWaveformScopeWidget();
    ~AudioWaveformScopeWidget() Q_DECL_OVERRIDE;
    QString getTitle() Q_DECL_OVERRIDE;

private:
    // Functions run in scope thread.
    void refreshScope(const QSize& size, bool full) Q_DECL_OVERRIDE;
    void createGrid(const QSize& size);
    
    // Functions run in GUI thread.
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

    // Members accessed only in scope thread (no thread protection).
    QImage m_renderWave;
    QTime m_refreshTime;
    int m_graphTopPadding;
    int m_graphLeftPadding;
    int m_channels;

    // Members accessed in multiple threads (mutex protected).
    QMutex m_mutex;
    QImage m_displayWave;
    QImage m_displayGrid;
};

#endif // AUDIOWAVEFORMSCOPEWIDGET_H
