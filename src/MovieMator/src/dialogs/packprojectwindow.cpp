#include "packprojectwindow.h"

#include "settings.h"
#include "mltcontroller.h"
#include "../models/multitrackmodel.h"

#include <QTimer>
#include <QDebug>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

int progressAndCancelCallBack(int nValue, void *pUserData)
{
    PackProjectWindow *pWindow = static_cast<PackProjectWindow*>(pUserData);
    if(pWindow)
    {
        qint64 nCompressed = pWindow->getCompressed();
        if(nCompressed == -1)
        {   // 手动取消
            pWindow->setCompressed(0);
        }
        else
        {
            nCompressed += nValue;
            pWindow->setCompressed(nCompressed);
            return 0;
        }
    }

    return 1;
}

PackProjectThread::PackProjectThread(void *pUserData, const QString &strZipFile,
                                     const QStringList &filesList, QObject *pParent)
    : QThread(pParent)
    , m_pUserData(pUserData)
    , m_strZipFile(strZipFile)
    , m_strListFiles(filesList)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

PackProjectThread::~PackProjectThread()
{

}

void PackProjectThread::run()
{
    if(MultitrackModel::zipPackResourceList(m_strZipFile, m_strListFiles, progressAndCancelCallBack, m_pUserData) == 0)
    {
        emit compressSucceed();
    }
    else
    {
        emit compressFailed();
    }
}

PackProjectWindow::PackProjectWindow(QWidget *pParent)
    : QProgressDialog (pParent)
{
    setLabelText(tr("Packaging..."));
    setCancelButton(nullptr);
    setFixedSize(QSize(500, 90));
#ifdef Q_OS_WIN
    setWindowFlags(Qt::Sheet);
#endif
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setRange(0, 100);
    setValue(0);
    setModal(true);

    connect(this, SIGNAL(rejected()), this, SLOT(onProgressDialogCanceled()));

    m_nCompressed    = 0;
    m_nMaxCompressed = 0;

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimeOut()));

    reset();    // cancel force show
}

PackProjectWindow::~PackProjectWindow()
{

}

// 计算文件夹大小
qint64 dirSize(const QString &strPath)
{
    QDir dir(strPath);
    qint64 nSize = 0;
    foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::Files))
    {
        nSize += fileInfo.size();
    }

    foreach(QString strSubDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {   //若存在子目录，则递归调用dirFileSize()函数
        nSize += dirSize(strPath + QDir::separator() + strSubDir);
    }

    return nSize;
}

void PackProjectWindow::packProject()
{
    m_nCompressed    = 0;
    m_nMaxCompressed = 0;

    // open save dialog
    m_strZipFile = QFileDialog::getSaveFileName(nullptr, tr("Package Project"),
                                        Settings.savePath(), tr("Zip Files (*.zip)"));

    // save mmp file
    QDir dirTemp = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
    m_strMmpFile = dirTemp.filePath(QFileInfo(m_strZipFile).baseName() + ".mmp");
    MLT.saveXML(m_strMmpFile);

    // get resource list from mmp file
    QStringList filesList = MultitrackModel::getResourceList(m_strMmpFile.toUtf8().data());

    // zip
    if(!m_strZipFile.isEmpty() && !filesList.isEmpty())
    {
        for(QString strFile : filesList)
        {
            QFileInfo fileInfo(strFile);
            qint64 nSize = fileInfo.size();

            if(fileInfo.isDir())
            {
                nSize = dirSize(strFile);
            }

            m_nMaxCompressed += nSize;
        }

        PackProjectThread *pThread = new PackProjectThread(this, m_strZipFile, filesList);
        connect(pThread, SIGNAL(compressFailed()), this, SLOT(onCompressFailed()));
        connect(pThread, SIGNAL(compressSucceed()), this, SLOT(onCompressSucceed()));
        pThread->start();

        m_pTimer->start(500);

        show();
    }
}

void PackProjectWindow::closeEvent(QCloseEvent *pEvent)
{
    int nResult = QMessageBox::information(this, tr("Stop packaging"), tr("Are you sure to stop packaging project?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(nResult == QMessageBox::Yes)
    {
        m_pTimer->stop();
        m_nCompressed = -1;
    }
    else
    {
        pEvent->ignore();
        if(m_nMaxCompressed != 0)
        {
            show();
        }
    }
}

void PackProjectWindow::onProgressDialogCanceled()
{
    int nResult = QMessageBox::information(this, tr("Stop packaging"), tr("Are you sure to stop the packaging project?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(nResult == QMessageBox::Yes)
    {
        m_pTimer->stop();
        m_nCompressed = -1;
    }
    else
    {
        if(m_nMaxCompressed != 0)
        {
            show();
        }
    }
}

void PackProjectWindow::onCompressSucceed()
{
    m_pTimer->stop();
    emit cancel();

    m_nCompressed    = 0;
    m_nMaxCompressed = 0;

    QFile::remove(m_strMmpFile);
}

void PackProjectWindow::onCompressFailed()
{
    m_pTimer->stop();
    emit cancel();

    m_nCompressed    = 0;
    m_nMaxCompressed = 0;

    QFile::remove(m_strMmpFile);
    QFile::remove(m_strZipFile);
}

void PackProjectWindow::onTimeOut()
{
    int nValue = int(m_nCompressed * 100 / m_nMaxCompressed);
    setValue(nValue);
}

