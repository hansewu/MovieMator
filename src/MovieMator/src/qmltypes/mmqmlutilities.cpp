/*
 * Copyright (c) 2013-2015 Meltytech, LLC
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

#include "qmltypes/colorpickeritem.h"
#include "qmltypes/colorwheelitem.h"
#include "qmltypes/mmqmlutilities.h"
#include <qmlview.h>
#include "qmltypes/qmlfile.h"
#include "qmltypes/qmlhtmleditor.h"
#include "qmlmetadata.h"
#include "qmltypes/timelineitems.h"
#include "qmltypes/filesavedialog.h"
#include "models/metadatamodel.h"
#include "models/attachedfiltersmodel.h"
#include <QCoreApplication>
#include <QSysInfo>
#include <QCursor>
#include <QtQml>
#include <QQmlEngine>
#include <QQmlContext>
#include "qmltypes/qmltextmetadata.h"

MMQmlUtilities::MMQmlUtilities(QmlUtilities *parent) :
    QmlUtilities(parent)
{
}

void MMQmlUtilities::registerCommonTypes()
{
    qmlRegisterType<QmlFile>("com.moviemator.qml", 1, 0, "File");
    qmlRegisterType<QmlHtmlEditor>("com.moviemator.qml", 1, 0, "HtmlEditor");
    qmlRegisterType<QmlMetadata>("com.moviemator.qml", 1, 0, "Metadata");
    qmlRegisterType<QmlTextMetaData>("com.moviemator.qml",1,0,"TextMetadata");
    qmlRegisterType<QmlKeyframesMetadata>();
    qmlRegisterType<QmlKeyframesParameter>("com.moviemator.qml", 1,0, "Parameter");

    qmlRegisterType<MMQmlUtilities>("com.moviemator.qml", 1, 0, "Utilities");
    qmlRegisterType<FileSaveDialog>("com.moviemator.qml",1, 0, "FileSaveDialog");
    // MetadataModel is registered to access its MetadataFilter enum.
    qmlRegisterUncreatableType<MetadataModel>("com.moviemator.qml", 1, 0, "MetadataModel",
                                              "You cannot create a MetadataModel from QML.");
    qmlRegisterUncreatableType<AttachedFiltersModel>("com.moviemator.qml", 1, 0, "AttachedFiltersModel",
                                              "You cannot create a AttachedFiltersModel from QML.");
    qmlRegisterType<ColorPickerItem>("MovieMator.Controls", 1, 0, "ColorPickerItem");
    qmlRegisterType<ColorWheelItem>("MovieMator.Controls", 1, 0, "ColorWheelItem");
    registerTimelineItems();
}



// void QmlUtilities::setCommonProperties(QQmlContext* context)
// {
//     context->setContextProperty("settings", &ShotcutSettings::singleton());
//     context->setContextProperty("application", &QmlApplication::singleton());
//     context->setContextProperty("profile", &QmlProfile::singleton());
// }

// QDir QmlUtilities::qmlDir()
// {
// //    QDir dir(qApp->applicationDirPath());
// //    dir.cdUp();
// //    dir.cd("Resources");

//     QDir dir(qApp->applicationDirPath());

// #if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
//     dir.cdUp();
// #elif defined(Q_OS_MAC)
//     dir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
// #endif
//     dir.cd("share");
//     dir.cd("moviemator");
//     dir.cd("qml");
//     return dir;
// }

// QQmlEngine * QmlUtilities::sharedEngine()
// {
//     static QQmlEngine * s_engine = 0;
//     if (!s_engine)
//     {
//         s_engine = new QQmlEngine;
// //        s_engine->setImportPathList(QStringList());
//     }
//     return s_engine;
// }

// QUrl QmlUtilities::blankVui()
// {
//     return QUrl("qrc:/src/qml/vui_droparea.qml");
// }
