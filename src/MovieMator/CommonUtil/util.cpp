/*
 * Copyright (c) 2014-2016 Meltytech, LLC
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

#include "util.h"
#include <QFileInfo>
#include <QWidget>
#include <QCryptographicHash>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

QString Util::baseName(const QString &filePath)
{
    QString s = filePath;
    // Only if absolute path and not a URI.
    if (s.startsWith('/') || s.midRef(1, 2) == ":/" || s.midRef(1, 2) == ":\\")
        s = QFileInfo(s).fileName();
    return s;
}

void Util::setColorsToHighlight(QWidget* widget, QPalette::ColorRole role)
{
    Q_ASSERT(widget);
    QPalette palette = widget->palette();
    palette.setColor(role, palette.color(palette.Highlight));
    palette.setColor(role == QPalette::Button ? QPalette::ButtonText : QPalette::WindowText,
        palette.color(palette.HighlightedText));
    widget->setPalette(palette);
    widget->setAutoFillBackground(true);
}

QString Util::removeFileScheme(QUrl& url)
{
    QString path = url.url();
    if (url.scheme() == "file")
        path = url.url(QUrl::PreferLocalFile);
    return path;
}

QString Util::getFileHash(const QString& path)
{
    // This routine is intentionally copied from Kdenlive.
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData;
         // 1 MB = 1 second per 450 files (or faster)
         // 10 MB = 9 seconds per 450 files (or faster)
        if (file.size() > 1000000*2) {
            fileData = file.read(1000000);
            if (file.seek(file.size() - 1000000))
                fileData.append(file.readAll());
        } else {
            fileData = file.readAll();
        }
        file.close();
        return QCryptographicHash::hash(fileData, QCryptographicHash::Md5).toHex();
    }
    return QString();
}

QString Util::templatePath()
{
    QDir dir(qApp->applicationDirPath());
#if defined(Q_OS_MAC)
    dir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
#endif
    dir.cd("templates");
    return  dir.absolutePath();
}

QString Util::resourcesPath()
{
    QDir dir(qApp->applicationDirPath());
#if defined(Q_OS_MAC)
    dir.cdUp();
    dir.cd("Resources");
#endif
    return dir.absolutePath();
}

bool Util::isAudioFile(const QString& strFilePath)
{
    QList<QString> audioFormat;
    audioFormat<<"a52"<<"asf"<<"ac3"<<"aac"<<"au"<<"aiff"<<"aif"<<
                 "ape"<<"amr"<<"caf"<<"cda"<<"dts"<<"dvr-ms"<<"flac"<<
                 "m4r"<<"mp2"<<"mp3"<<"mmf"<<"m2a"<<"mp1"<<"mp1"<<"mpa"<<
                 "mod"<<"m4a"<<"mka"<<"ogg"<<"ra"<<"wma"<<"wav";

    QFileInfo fileInfo(strFilePath);
    QString strSuffix = fileInfo.suffix();
    for (int i = 0; i < audioFormat.count(); i++)
    {
        if (strSuffix.compare(audioFormat.at(i), Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }
    return  false;
}

QString Util::logFolderPath()
{
    QDir dir(qApp->applicationDirPath());
#if defined(Q_OS_MAC)
    dir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
#endif
    if (!dir.exists()) dir.mkpath(dir.path());
    dir.mkdir("log");
    dir.cd("log");
    return  dir.absolutePath();
}







