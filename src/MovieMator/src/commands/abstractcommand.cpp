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

static bool g_iSInUndoRedoProcess = false;

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
    Q_ASSERT(g_iSInUndoRedoProcess == false);
}

void AbstractCommand::redo()
{
    g_iSInUndoRedoProcess = true;
    m_originalXml = MLT.XML(MAIN.timelineDock()->model()->tractor());

    MLT.consumer()->set_cancelled(1);
    w_enter_critical();
    MLT.consumer()->set_cancelled(0);

    this->redo_impl();
    w_leave_critical();

    g_iSInUndoRedoProcess = false;
}

void AbstractCommand::undo()
{
    g_iSInUndoRedoProcess = true;
    MLT.consumer()->set_cancelled(1);
    w_enter_critical();
    MLT.consumer()->set_cancelled(0);
    this->undo_impl();
    w_leave_critical();

    m_currentXml = MLT.XML(MAIN.timelineDock()->model()->tractor());
#if defined(NDEBUG)
    saveXmlFile(m_originalXml,m_currentXml,text());
//    Q_ASSERT(m_currentXml == m_originalXml);
#endif
    g_iSInUndoRedoProcess = false;
}


