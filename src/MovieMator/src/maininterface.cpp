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
#include "templateeidtor.h"
#include "util.h"
#include <QDomDocument>
#include <Logger.h>
#include <cmath>

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
//    Q_ASSERT(producer);
//    Q_ASSERT(producer->is_valid());
    if (producer && producer->is_valid()) {
        MLT.setImageDurationFromDefault(producer);
        //if (filepath.endsWith(".mlt"))
        //    producer->set(kShotcutVirtualClip, 1);
    }
    else {
        delete producer;
        producer = nullptr;
    }
    return producer;
}

void MainInterface::destroyFileHandle(FILE_HANDLE fileHandle) {
    if (fileHandle) {
        Mlt::Producer *producer = static_cast<Mlt::Producer*>(fileHandle);
        delete producer;
        producer = nullptr;
        fileHandle = nullptr;
    }
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
    Mlt::Producer *producer = static_cast<Mlt::Producer*>(fileHandle);    //(Mlt::Producer *)fileHandle;
    Q_ASSERT(producer);
    Q_ASSERT(producer->is_valid());
    if (!producer || !producer->is_valid())
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
    Mlt::Producer *producer = static_cast<Mlt::Producer*>(fileHandle);  // (Mlt::Producer *)fileHandle;
    Q_ASSERT(producer);
    Q_ASSERT(producer->is_valid());
    if (!producer || !producer->is_valid())
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
    Mlt::Producer *producer = static_cast<Mlt::Producer*>(fileHandle);  //(Mlt::Producer *)fileHandle;
    Q_ASSERT(producer->is_valid());
    if (!producer->is_valid())
        return -1;

    TimelineDock *timeline = MAIN.timelineDock();

    MAIN.pushCommand(
                new Timeline::AppendClipCommand(*(timeline->model()), timeline->currentTrack(),
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
    Mlt::Producer *producer = static_cast<Mlt::Producer*>(fileHandle);  //(Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return QImage();


    int in = producer->get_in();

    QImage image;
    if (producer->get_data(kThumbnailInProperty))
    {
        QImage* thumb = static_cast<QImage*>(producer->get_data(kThumbnailInProperty));    //(QImage*)producer->get_data(kThumbnailInProperty);
        image = *thumb;
    }
    else
    {
        image = MLT.image(*producer, in, THUMBNAIL_WIDTH*2, THUMBNAIL_HEIGHT*2);
        producer->set(kThumbnailInProperty, new QImage(image), 0, reinterpret_cast<mlt_destructor>(&deleteQImage), nullptr);  // (mlt_destructor) deleteQImage
    }

    return image;
}

QString MainInterface::getFileName(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = static_cast<Mlt::Producer*>(fileHandle);  //(Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return QString();

    QString resource = QString::fromUtf8(producer->get("resource"));
    return resource;
}

FILE_TYPE MainInterface::getFileType(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    FILE_TYPE fileType = FILE_TYPE_NONE;
    Mlt::Producer *pProducer = static_cast<Mlt::Producer*>(fileHandle);
    if (!pProducer || !pProducer->is_valid())
    {
        return fileType;
    }

    int nAudioIndex = pProducer->get_int("audio_index");
    int nVideoIndex = pProducer->get_int("video_index");
    if (nVideoIndex >= 0)
    {
        if (nVideoIndex == 0 && nAudioIndex == 0)
        {
            fileType = FILE_TYPE_IMAGE;
        }
        else
        {
            QString strResource = QString(pProducer->get("resource"));
            if (Util::isAudioFile(strResource))//有封面的音频也会有video_index，因此还需要在做格式
            {
                fileType = FILE_TYPE_AUDIO;
            }
            else
            {
                fileType = FILE_TYPE_VIDEO;
            }
        }
    }
    else if (nAudioIndex >= 0)
    {
        fileType = FILE_TYPE_AUDIO;
    }

    return fileType;
}

//功能：获取文件时长
QString MainInterface::getDuration(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = static_cast<Mlt::Producer*>(fileHandle);  //(Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return QString("");
    return producer->get_length_time();
}

int MainInterface::getPlayTime(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = static_cast<Mlt::Producer*>(fileHandle);  //(Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return -1;
    return producer->get_playtime();
}


QSize MainInterface::getWidthHeight(FILE_HANDLE fileHandle)
{
    Q_ASSERT(fileHandle);
    Mlt::Producer *producer = static_cast<Mlt::Producer*>(fileHandle);  //(Mlt::Producer *)fileHandle;
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
    Mlt::Producer *producer = static_cast<Mlt::Producer*>(fileHandle);  //(Mlt::Producer *)fileHandle;
    if (!producer->is_valid())
        return QString();

    return MLT.XML(producer);
}

//功能：返回xml的mimetype；实现拖放时使用。
const QString& MainInterface::getXMLMimeTypeForDragDrop()
{
    return MLT.MltXMLMimeType();
}


// 功能：通过xml字符串生成FILE_HANDLE；主程序提供接口
FILE_HANDLE MainInterface::createFileWithXMLForDragAndDrop(QString xml)
{
    Mlt::Producer *producer = new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData());
    Q_ASSERT(producer);
    if (!producer) {
        return nullptr;
    }
    if (!producer->is_valid())
    {
        delete producer;
        producer = nullptr;
    }

    QString resource(producer->get("moviemator:template"));
    int filter_count = producer->filter_count();
    bool hasSizeAndPositionFilter = false;
    for (int i = 0; i < filter_count; i++) {
         Mlt::Filter* filter = producer->filter(i);
         char* filter_name = filter->get("moviemator:filter");
         if (QString(filter_name) == "affineSizePosition") {
             hasSizeAndPositionFilter = true;

             delete filter;
             filter = nullptr;

             break;
         }

         delete filter;
         filter = nullptr;
    }

    if (resource == "template" && !hasSizeAndPositionFilter) {
        producer->set(kShotcutVirtualClip, 1);
        //添加模板，自动添加调节大小滤镜
        Mlt::Filter* sizeAndPositionFilter;
        sizeAndPositionFilter = new Mlt::Filter(MLT.profile(), "affine");
        sizeAndPositionFilter->set("moviemator:filter", "affineSizePosition");
        sizeAndPositionFilter->set("transition.fill", 1);
        sizeAndPositionFilter->set("transition.distort", 0);
        QString imageWStr(producer->get("moviemator:imageW"));
        QString imageHStr(producer->get("moviemator:imageH"));
        int imageW = imageWStr.toInt();
        int imageH = imageHStr.toInt();
        double x = (double(MLT.profile().width()) - double(imageW)) / 2 / double(MLT.profile().width());
        double y = (double(MLT.profile().height()) - double(imageH)) / 2 / double(MLT.profile().height());
        double w = double(imageW) / double(MLT.profile().width());
        double h = double(imageH) / double(MLT.profile().height());
        sizeAndPositionFilter->set("transition.rect_anim_relative", x, y, w, h);
        sizeAndPositionFilter->set("transition.valign", "top");
        sizeAndPositionFilter->set("transition.halign", "left");
        sizeAndPositionFilter->set("transition.threads", 0);
        producer->attach(*sizeAndPositionFilter);

        delete sizeAndPositionFilter;
        sizeAndPositionFilter = nullptr;
    }

    return producer;
}

// 功能：把模板中的索引为index的文件替换成文件destFile
// 返回值：成功返回0，失败返回-1
int MainInterface::replaceFileInTemplate(int index, FILE_HANDLE destFile)
{
    Q_UNUSED(index)
    Q_ASSERT(destFile);
    Mlt::Producer *producer = static_cast<Mlt::Producer*>(destFile);  //(Mlt::Producer *)destFile;
    if (!producer->is_valid())
        return -1;
//    TemplateEidtor *templateEditor = MAIN.templateEditor();
//    templateEditor->replaceFileInTemplate(index, producer);
    return 0;
}

// 功能：重置模板文件为缺省文件
// 返回值：成功返回0，失败返回-1
FILE_HANDLE MainInterface::resetFileInTemplate(int index)
{
    Q_UNUSED(index)
//    TemplateEidtor *templateEditor = MAIN.templateEditor();
//    return templateEditor->resetFileToTemplateDefault(index);
    return nullptr;
}

//功能：返回xml的mimetype；实现拖放时使用。
void MainInterface::addFilter(int nFilterIndex)
{
    MAIN.filterController()->addFilter(nFilterIndex);
    TimelineDock *timeline = MAIN.timelineDock();
    timeline->setPosition(timeline->position());
}