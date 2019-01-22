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

#include "maininterface.h"
#include "mainwindow.h"
#include <Mlt.h>
#include <mltcontroller.h>
#include <QUndoStack>
#include "docks/timelinedock.h"
#include "commands/timelinecommands.h"
#include <shotcut_mlt_properties.h>
#include "controllers/filtercontroller.h"

MainInterface& MainInterface::singleton()
{
    static MainInterface* instance = new MainInterface();
    return *instance;
}
//功能：播放文件。
//参数：filepath文件路径。
 //返回： NULL 失败，其他 成功
FILE_HANDLE MainInterface::openFile(QString filepath)
{
    Mlt::Producer *producer = new Mlt::Producer(MLT.profile(), filepath.toUtf8().constData());
    if (producer->is_valid()) {
        MLT.setImageDurationFromDefault(producer);
    }
    else {
        delete producer;
        producer = 0;
    }
    return producer;
}

//0 成功 其他失败
int MainInterface::openFileAsProject(QString filepath)
{
    MAIN.open(filepath);
    return 0;
}

//返回： NULL 失败，其他 成功
FILE_HANDLE MainInterface::getCurrentOpenedFile()
{
    return MLT.producer();
}

 //返回： 0 成功， 其他 失败
int MainInterface::playFile(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = (Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return -1;

    QString xml = MLT.XML(producer);
    Mlt::Producer* p = new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData());

    MAIN.open(p);

    return 0;
}

//功能：显示属性面板。
//参数：filepath文件路径
void MainInterface::showProperties(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = (Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return;

    QString xml = MLT.XML(producer);
    Mlt::Producer* p = new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData());

    MAIN.loadProducerWidget(p);
    MAIN.onPropertiesDockTriggered();
}

//功能：将文件添加到时间线
//参数：filepath文件路径
//返回： 0 成功， 其他 失败
int MainInterface::addToTimeLine(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = (Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return -1;

    TimelineDock *timeline = MAIN.timelineDock();

    MAIN.undoStack()->push(
                new Timeline::AppendCommand(*(timeline->model()), timeline->currentTrack(),
                                            MLT.XML(producer)));
    return 0;
}


static const int THUMBNAIL_WIDTH = 80;
static const int THUMBNAIL_HEIGHT = 45;

static void deleteQImage(QImage* image)
{
    delete image;
}
//功能：获取文件的缩略图
//QImage getThumbnail(QString filepath);
//加入线程获取thumbnail
// void startGetThumbnail(FILE_HANDLE fileHandle, (void )(tell_thumbnail *)(QString filePath, QImage thumbImage));//(QString filepath);
QImage MainInterface::getThumbnail(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = (Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return QImage();


    int in = producer->get_in();

    QImage image;
    if (producer->get_data(kThumbnailInProperty))
    {
        QImage* thumb = (QImage*)producer->get_data(kThumbnailInProperty);
        image = *thumb;
    }
    else
    {
        image = MLT.image(*producer, in, THUMBNAIL_WIDTH*2, THUMBNAIL_HEIGHT*2);
        producer->set(kThumbnailInProperty, new QImage(image), 0, (mlt_destructor) deleteQImage, nullptr);
    }

    return image;
}

QString MainInterface::getFileName(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = (Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return QString();

    QString resource = QString::fromUtf8(producer->get("resource"));
    return resource;
}

//功能：获取文件时长
QString MainInterface::getDuration(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = (Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return QString("");
    return producer->get_length_time();
}

int MainInterface::getPlayTime(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = (Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return -1;
    return producer->get_playtime();
}


QSize MainInterface::getWidthHeight(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = (Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return QSize();

    int width = producer->get_int("meta.media.width");
    int height = producer->get_int("meta.media.height");
    return QSize(width, height);
}

//功能：从文件生成XML字符串；实现拖放时使用
QString MainInterface::getXmlForDragDrop(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = (Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return QString();

    return MLT.XML(producer);
}

//功能：返回xml的mimetype；实现拖放时使用。
const QString& MainInterface::getXMLMimeTypeForDragDrop()
{
    return MLT.MltXMLMimeType();
}

//功能：返回xml的mimetype；实现拖放时使用。
void MainInterface::addFilter(int nFilterIndex)
{
    MAIN.filterController()->addFilter(nFilterIndex);
}
