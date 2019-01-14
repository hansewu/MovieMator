/*
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
#ifndef MMSPLASHSCREEN_H
#define MMSPLASHSCREEN_H

#include <QSplashScreen>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QThread>


class MMSplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    explicit MMSplashScreen(const QPixmap &pixmap = QPixmap());
    ~MMSplashScreen();
signals:
    void progressValueChanged(int);

public slots:

private slots:
    void updateProgress();
    void onProgressValueChanged(int value);

private:
    QProgressBar *progressBar;
    QLabel *infoLabel;

    QTimer *progressTimer;
    int progressValue;

    QThread *progressThread;

};

#endif // MMSPLASHSCREEN_H
