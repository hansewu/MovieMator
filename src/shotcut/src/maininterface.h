#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <QObject>

typedef void * FILE_HANDLE; 
class MainInterface
{
public:
    static MainInterface& singleton();
//    MainInterface();
//    virtual ~MainInterface(){}
    //功能：播放文件。
    //参数：filepath文件路径。
     //返回： NULL 失败，其他 成功
    virtual FILE_HANDLE openFile(QString filepath);

    //0 成功 其他失败
    int openFileAsProject(QString filepath);

    //返回： NULL 失败，其他 成功
    FILE_HANDLE getCurrentOpenedFile();

     //返回： 0 成功， 其他 失败
    virtual int playFile(FILE_HANDLE fileHandle);

    //功能：显示属性面板。
    //参数：filepath文件路径
    void showProperties(FILE_HANDLE fileHandle);

    //功能：将文件添加到时间线
    //参数：filepath文件路径
    //返回： 0 成功， 其他 失败
    int addToTimeLine(FILE_HANDLE fileHandle);

    //功能：获取文件的缩略图
    //QImage getThumbnail(QString filepath);
    //加入线程获取thumbnail
   // void startGetThumbnail(FILE_HANDLE fileHandle, (void )(tell_thumbnail *)(QString filePath, QImage thumbImage));//(QString filepath);
   QImage getThumbnail(FILE_HANDLE fileHandle);

    //功能：获取文件时长
    QString getDuration(FILE_HANDLE fileHandle);


    QSize getWidthHeight(FILE_HANDLE fileHandle);

    //功能：从文件生成XML字符串；实现拖放时使用
    QString getXmlForDragDrop(FILE_HANDLE fileHandle);

    //功能：返回xml mime字符串；实现拖放时使用。
    const QString& getXMLMimeTypeForDragDrop();
};

#define MAININTERFACE MainInterface::singleton()

#endif // MAININTERFACE_H
