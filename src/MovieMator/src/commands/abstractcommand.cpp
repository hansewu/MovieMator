#include "abstractcommand.h"

#include <Mlt.h>
#include <Logger.h>
#include <mltcontroller.h>
#include <QDir>
#include <QDateTime>
#include "util.h"
#include <QDomDocument>

bool g_isInUndoRedoProcess = false;

AbstractCommand::AbstractCommand(MultitrackModel &modelMultitrack, QUndoCommand * pParent)
    : QUndoCommand (pParent)
    , m_modelMultitrack(modelMultitrack)
    , m_selectionOld(m_modelMultitrack.selection())
    , m_bisFirstRedo(true)
{
    //Q_ASSERT(g_isInUndoRedoProcess == false); //UpdateClipCommand 创建后并不会立即push，此处添加assert创建UpdateClipCommand时会出错。在pushcommand的时候添加。
}

void AbstractCommand::redo()
{
    g_isInUndoRedoProcess = true;

//#ifndef NDEBUG
//    m_strXmlOriginal = MLT.XML(MAIN.timelineDock()->model()->tractor());
//#endif

    //设置操作时的选中状态
    if (m_bisFirstRedo == false)
    {
        m_modelMultitrack.setSelection(m_selectionOld);
    }

    //取消consumer
    MLT.consumer()->set_cancelled(1);
    w_enter_critical();
    MLT.consumer()->set_cancelled(0);

    this->redo_impl();
    w_leave_critical();

    //记录操作之后的选中状态
    if (m_bisFirstRedo == true)
    {
        m_selectionNew = m_modelMultitrack.selection();
        m_bisFirstRedo = false;
    }
    else
    {
        m_modelMultitrack.setSelection(m_selectionNew);
    }

    g_isInUndoRedoProcess = false;
}

void AbstractCommand::undo()
{
    g_isInUndoRedoProcess = true;

    //恢复选中状态
    m_modelMultitrack.setSelection(m_selectionNew);

    MLT.consumer()->set_cancelled(1);
    w_enter_critical();
    MLT.consumer()->set_cancelled(0);
    this->undo_impl();
    w_leave_critical();

    //恢复选中状态
    m_modelMultitrack.setSelection(m_selectionOld);
//#ifndef NDEBUG
//    m_strXmlCurrent = MLT.XML(MAIN.timelineDock()->model()->tractor());
//    saveStateAsXmlFile(m_strXmlOriginal,m_strXmlCurrent,text());
////    Q_ASSERT(m_strXmlCurrent == m_strXmlOriginal);
//#endif
    g_isInUndoRedoProcess = false;
}


//static void saveStateAsXmlFile(QString original,QString currrent,QString commandName)
//{
//    QDomDocument doc;
//    if(doc.setContent(original))
//    {
//        original = doc.toString();
//    }
//    if(doc.setContent(currrent))
//    {
//        currrent = doc.toString();
//    }

//    QString folderName = commandName;

//    QDateTime now = QDateTime::currentDateTime();
//    QString strNow =now.toString("MMdd-hhmmss");
//    folderName = strNow + folderName;

//    if(original != currrent)
//    {
//        folderName = folderName + "-Error";
//    }

//    QDir dir = Util::logFolderPath();
//    dir.mkdir(folderName);
//    dir.cd(folderName);

//    QString originalPath = dir.absoluteFilePath("original.mmp");
//    QFile originalFile(originalPath);
//    originalFile.open(QIODevice::WriteOnly | QIODevice::Text);
//    originalFile.write(original.toUtf8());
//    originalFile.close();

//    QString currentPath = dir.absoluteFilePath("current.mmp");
//    QFile currentFile(currentPath);
//    currentFile.open(QIODevice::WriteOnly | QIODevice::Text);
//    currentFile.write(currrent.toUtf8());
//    currentFile.close();

//    qDebug()<<"original:";
//    qDebug()<<original;
//    qDebug()<<"currrent:";
//    qDebug()<<currrent;
//}
