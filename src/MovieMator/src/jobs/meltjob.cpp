/*
 * Copyright (c) 2012-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
  *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: wyl <wyl@pylwyl.local>
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

#include "meltjob.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QIODevice>
#include <QApplication>
#include <QAction>
#include <QDialog>
#include <QDir>
#include <Logger.h>
#include "mainwindow.h"
#include "dialogs/textviewerdialog.h"
#include <settings.h>

MeltJob::MeltJob(const QString& name, const QString& xml)
    : AbstractJob(name)
    , m_xml(QDir::tempPath().append("/MovieMator-XXXXXX.mmp"))
    , m_isStreaming(false)
{
    QAction* action = new QAction(tr("View XML"), this);
    action->setToolTip(tr("View the MLT XML for this job"));
    connect(action, SIGNAL(triggered()), this, SLOT(onViewXmlTriggered()));
    m_standardActions << action;
    m_xml.open();
    m_xml.write(xml.toUtf8());
    m_xml.close();
    m_standardActions << action;
}

MeltJob::~MeltJob()
{
    LOG_DEBUG() << "begin";
}

void MeltJob::start()
{
    QDir appDir(qApp->applicationDirPath());
#if defined (Q_OS_MAC)
    appDir.cdUp();
    appDir.cd("Resources");
#endif
    appDir.cd("share");
    appDir.cd("mlt");
//    setenv("MLT_DATA", appDir.path().toUtf8().constData(), 1);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("MLT_DATA",  appDir.path().toUtf8().constData()); // Add an environment variable
    QProcess::setProcessEnvironment(env);

    QString shotcutPath = qApp->applicationDirPath();
#ifdef Q_OS_WIN
    QFileInfo meltPath(shotcutPath, "qmelt.exe");
#else
//    QFileInfo meltPath(shotcutPath, "moviemator");  //modify by wyl
    QFileInfo meltPath(shotcutPath, "qmelt");
#endif
    setReadChannel(QProcess::StandardError);
    QStringList args;
    args << "-progress2";
    args << "-abort";
    args << xmlPath();

    // 导出视频 5分钟时长限制
#if defined(MOVIEMATOR_FREE) && !defined(SHARE_VERSION)
    if (!Settings.isSubscribed())
    {
        int length = 5*60*MLT.profile().fps() - 1;
        QString out = QString("out=%1").arg(length);
        args << out;
    }
#endif

    LOG_DEBUG() << meltPath.absoluteFilePath() << args;

#ifdef Q_OS_WIN
    if (m_isStreaming) args << "-getc";
    QProcess::start(meltPath.absoluteFilePath(), args);
#else
    args.prepend(meltPath.absoluteFilePath());
    QProcess::start("/usr/bin/nice", args);
//    QProcess::start(meltPath.absoluteFilePath(), args);
#endif
    AbstractJob::start();
}

QString MeltJob::xml()
{
    m_xml.open();
    QString s(m_xml.readAll());
    m_xml.close();
    return s;
}

void MeltJob::setIsStreaming(bool streaming)
{
    m_isStreaming = streaming;
}

void MeltJob::onViewXmlTriggered()
{
    TextViewerDialog dialog(&MAIN);
    dialog.setWindowTitle(tr("MLT XML"));
    dialog.setText(xml());
    dialog.exec();
}

void MeltJob::onReadyRead()
{
    QString msg = readLine();
    if (msg.contains("percentage:")) {
        uint percent = msg.mid(msg.indexOf("percentage:") + 11).toUInt();
        emit progressUpdated(m_index, percent);
    }
    else {
        appendToLog(msg);
    }
}
