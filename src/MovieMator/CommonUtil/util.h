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

#ifndef UTIL_H
#define UTIL_H

#include "commonutil_global.h"

#include <QString>
#include <QPalette>
#include <QUrl>

class QWidget;

class COMMONUTILSHARED_EXPORT Util
{
private:
    Util() {}
public:
    static QString baseName(const QString &filePath);
    static void setColorsToHighlight(QWidget* widget, QPalette::ColorRole role = QPalette::Window);
    static QString removeFileScheme(QUrl& url);
    static QString getFileHash(const QString& path);
    static QString resourcesPath();
    static QString templatePath();
};

#endif // UTIL_H
