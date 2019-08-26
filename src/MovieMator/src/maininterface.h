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

#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <QObject>
#include "models/metadatamodel.h"

enum FILE_TYPE {
    FILE_TYPE_NONE,
    FILE_TYPE_VIDEO,
    FILE_TYPE_AUDIO,
    FILE_TYPE_IMAGE,
};

typedef void * FILE_HANDLE; 
class MainInterface
{
public:
    static MainInterface& singleton();
//    MainInterface();
    virtual ~MainInterface(){}  // 有虚函数就需要虚析构函数，用来消除警告
    //功能：播放文件。
    //参数：filepath文件路径。
     //返回： NULL 失败，其他 成功
    virtual FILE_HANDLE openFile(QString filepath);

    virtual void destroyFileHandle(FILE_HANDLE &fileHandle);

    //0 成功 其他失败
    virtual int openFileAsProject(QString filepath);

    //返回： NULL 失败，其他 成功
    virtual FILE_HANDLE getCurrentOpenedFile();

     //返回： 0 成功， 其他 失败
    virtual int playFile(FILE_HANDLE fileHandle);

    //功能：显示属性面板。
    //参数：filepath文件路径
    virtual void showProperties(FILE_HANDLE fileHandle);

    //功能：将文件添加到时间线
    //参数：filepath文件路径
    //返回： 0 成功， 其他 失败
    virtual int addToTimeLine(FILE_HANDLE fileHandle);

    //功能：获取文件的缩略图
    //QImage getThumbnail(QString filepath);
    //加入线程获取thumbnail
   // void startGetThumbnail(FILE_HANDLE fileHandle, (void )(tell_thumbnail *)(QString filePath, QImage thumbImage));//(QString filepath);
   virtual QImage getThumbnail(FILE_HANDLE fileHandle);

   virtual QString getFileName(FILE_HANDLE fileHandle);

    //功能：获取文件类型
    virtual FILE_TYPE getFileType(FILE_HANDLE fileHandle);

    //功能：获取文件时长
    virtual QString getDuration(FILE_HANDLE fileHandle);

    virtual int getPlayTime(FILE_HANDLE fileHandle);


    virtual QSize getWidthHeight(FILE_HANDLE fileHandle);

    //功能：从文件生成XML字符串；实现拖放时使用
    virtual QString getXmlForDragDrop(FILE_HANDLE fileHandle);

    //功能：返回xml mime字符串；实现拖放时使用。
    virtual const QString& getXMLMimeTypeForDragDrop();

    // 功能：通过xml字符串生成FILE_HANDLE；主程序提供接口
    virtual FILE_HANDLE createFileWithXMLForDragAndDrop(QString xml);

    // 功能：把模板中的索引为index的文件替换成文件destFile
    // 返回值：成功返回0，失败返回-1
    virtual int replaceFileInTemplate(int index, FILE_HANDLE destFile);

    // 功能：重置模板文件为缺省文件
    // 返回值：成功返回0，失败返回-1
    virtual FILE_HANDLE resetFileInTemplate(int index);

    //添加滤镜到选定的clip上
    virtual void addFilter(int nFilterIndex = -1);
};

#define MAININTERFACE MainInterface::singleton()

#endif // MAININTERFACE_H
