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
#include "mmsplashscreen.h"
#include <QApplication>

MMSplashScreen::MMSplashScreen(const QPixmap& pixmap) : QSplashScreen (pixmap)
  ,progressBar(nullptr)
  ,progressTimer(nullptr)
  ,progressValue(0)
{
//    progressBar = new QProgressBar(this);
//    progressBar->setGeometry(0, pixmap.height()-50, pixmap.width(), 30);

//    progressBar->setMinimum(0);
//    progressBar->setMaximum(100);
//    progressBar->setTextVisible(false);


//    progressThread = new QThread();
//    progressThread->start();

//    progressTimer = new QTimer();
//    progressTimer->setInterval(1000);
//    progressTimer->moveToThread(progressThread);
//    connect(progressTimer, SIGNAL(timeout()), this, SLOT(updateProgress()), Qt::DirectConnection);
//    connect(progressThread, SIGNAL(started()), progressTimer, SLOT(start()));
//    connect(this, SIGNAL(progressValueChanged(int)), this, SLOT(onProgressValueChanged(int)));


//    progressTimer->start();
//    progressBar->setAlignment(Qt::AlignHCenter);
//    progressBar->setStyleSheet("QProgressBar {color:black;font:30px;text-align:center; }QProgressBar::chunk {background-color: rgb(202, 165, 14);}");
//    infoLabel = new QLabel(this);
//    infoLabel->setGeometry(0, pixmap.height()-80, pixmap.height(), 20);
}

MMSplashScreen::~MMSplashScreen()
{
//    delete progressBar;
//    progressTimer->stop();
//    delete progressTimer;
//    progressThread->exit();
//    delete progressThread;
//    delete infoLabel;
}

void MMSplashScreen::updateProgress()
{
    if (progressValue < 90)
        progressValue += 10;
    else if (progressValue < 100)
        progressValue += 1;
//    progressBar->setValue(progressValue);
//    qApp->processEvents();
    emit progressValueChanged(progressValue);
    printf("progressValue - %d\n", progressValue);
//    QMetaObject::invokeMethod(progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, progressValue));
}

void MMSplashScreen::onProgressValueChanged(int value)
{
    progressBar->setValue(value);
}
