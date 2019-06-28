#include "abstractcommand.h"

#include <Mlt.h>
#include <Logger.h>
#include <mltcontroller.h>
#include <QDir>
#include <QDateTime>
#include "util.h"
#include <QDomDocument>
#include "mainwindow.h"
#include "docks/timelinedock.h"

void saveXmlFile(QString original,QString currrent,QString commandName)
{   
    QDomDocument doc;
    if(doc.setContent(original)){
        original = doc.toString();
    }
    if(doc.setContent(currrent)){
        currrent = doc.toString();
    }

    QString folderName = commandName;

    QDateTime now = QDateTime::currentDateTime();
    QString strNow =now.toString("MMdd-hhmmss");
    folderName = strNow + folderName;

    if(original != currrent){
        folderName = folderName + "-Error";
    }

    QDir dir = Util::logFolderPath();
    dir.mkdir(folderName);
    dir.cd(folderName);

    QString originalPath = dir.absoluteFilePath("original.mmp");
    QFile originalFile(originalPath);
    originalFile.open(QIODevice::WriteOnly | QIODevice::Text);
    originalFile.write(original.toUtf8());
    originalFile.close();

    QString currentPath = dir.absoluteFilePath("current.mmp");
    QFile currentFile(currentPath);
    currentFile.open(QIODevice::WriteOnly | QIODevice::Text);
    currentFile.write(currrent.toUtf8());
    currentFile.close();

    if(original != currrent){
        if (!MLT.open(originalPath)) {
            MAIN.open(MLT.producer());
        }
    }

    qDebug()<<"original:";
    qDebug()<<original;
    qDebug()<<"currrent:";
    qDebug()<<currrent;
}

AbstractCommand::AbstractCommand(QUndoCommand * parent) : QUndoCommand (parent)
{

}

void AbstractCommand::redo()
{
    m_originalXml = MLT.XML(MAIN.timelineDock()->model()->tractor());

    MLT.consumer()->set_cancelled(1);
    w_enter_critical();
    MLT.consumer()->set_cancelled(0);

    this->redo_impl();
    w_leave_critical();


}

void AbstractCommand::undo()
{
    MLT.consumer()->set_cancelled(1);
    w_enter_critical();
    MLT.consumer()->set_cancelled(0);
    this->undo_impl();
    w_leave_critical();

    m_currentXml = MLT.XML(MAIN.timelineDock()->model()->tractor());
#ifndef NDEBUG
    saveXmlFile(m_originalXml,m_currentXml,text());
//    Q_ASSERT(m_currentXml == m_originalXml);
#endif
}


