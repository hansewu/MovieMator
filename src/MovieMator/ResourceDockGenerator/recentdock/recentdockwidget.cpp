#include "recentdockwidget.h"
#include "recentitemmodel.h"

#include "ui_basedockwidget.h"
#include "baselistview.h"
#include "lineeditclear.h"

#include "util.h"
#include "settings.h"

#include <QDir>
#include <QMenu>
#include <QDebug>
#include <QScrollBar>

RecentDockWidget::RecentDockWidget(MainInterface *pMainInterface, QWidget *pParent) :
    BaseDockWidget (pParent),
    m_pMainInterface(pMainInterface)
{
    m_recent            = Settings.recent();

    m_strItemName[0]    = tr("0 Samples");
    m_strItemName[1]    = tr("1 Videos");
    m_strItemName[2]    = tr("2 Audios");
    m_strItemName[3]    = tr("3 Images");

    m_pRemoveAction     = new QAction(this);
    m_pRemoveAllAction  = new QAction(this);

    m_pRemoveAction->setObjectName(QStringLiteral("actionRemove"));
    m_pRemoveAction->setText(tr("Remove"));
    m_pRemoveAllAction->setObjectName(QStringLiteral("actionRemoveAll"));
    m_pRemoveAllAction->setText(tr("Remove All"));

    connect(m_pRemoveAction, SIGNAL(triggered()), this, SLOT(on_actionRemove_triggered()));
    connect(m_pRemoveAllAction, SIGNAL(triggered()), this, SLOT(on_actionRemoveAll_triggered()));

    connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisibility(bool)));
}

RecentDockWidget::~RecentDockWidget()
{
//    delete m_pRemoveAction;
//    m_pRemoveAction = nullptr;
//    delete m_pRemoveAllAction;
//    m_pRemoveAllAction = nullptr;

    for(int i = 0; i < nCount; i++)
    {
        RecentItemModel *pModel = static_cast<RecentItemModel*>(m_pProxyModelArray[i]->sourceModel());
        if(pModel)
        {
            while(pModel->rowCount() > 0)
            {
                pModel->removeRow(0);
            }
            delete pModel;
        }
        delete m_pProxyModelArray[i];
        m_pProxyModelArray[i] = nullptr;
    }
}

bool RecentDockWidget::hasClass()
{
    return true;
}

void RecentDockWidget::setupOtherUi()
{
    BaseDockWidget::setupOtherUi();

    LineEditClear *pSearchLineEdit = new LineEditClear(this);
    pSearchLineEdit->setObjectName(QStringLiteral("searchLineEdit"));
    pSearchLineEdit->setPlaceholderText(tr("Search"));

    connect(pSearchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(on_lineEdit_textChanged(QString)));

    ui->horizontalLayout_titlebar->addWidget(pSearchLineEdit);
}

void RecentDockWidget::add(const QString &strFile)
{
    if(strFile.startsWith(QDir::tempPath(), Qt::CaseInsensitive))
    {
        return;
    }
    if(m_recent.contains(strFile))
    {
        return;
    }
    if( strFile.endsWith(".mmp", Qt::CaseInsensitive) ||
        strFile.endsWith(".mlt", Qt::CaseInsensitive) ||
        strFile.endsWith(".xml", Qt::CaseInsensitive) )
    {
        return;
    }

    int nModelIndex = setItemModelInfo(strFile);
    if(nModelIndex >= 0)
    {
        m_recent.prepend(strFile);
        Settings.setRecent(m_recent);

        if(ui->comboBox_class->findText(m_strItemName[nModelIndex]) < 0)
        {
            ui->comboBox_class->clear();
            showModelTitle(nModelIndex);

            for(int i = 0; i < nCount; i++)
            {
                if(m_pProxyModelArray[i]->sourceModel()->rowCount() > 0)
                {
                    ui->comboBox_class->addItem(m_strItemName[i]);
                }
            }
        }

        ui->comboBox_class->setCurrentText(m_strItemName[nModelIndex]);

        resizeEvent(nullptr);
        onClassComboBoxActivated(ui->comboBox_class->currentIndex());
    }
}

QString RecentDockWidget::remove(const QString &strFile)
{
    QString strName = Util::baseName(strFile);

    if(m_recent.removeOne(strFile))
    {
        Settings.setRecent(m_recent);

        FILE_HANDLE fileHandle = m_pMainInterface->openFile(strFile);
        if(fileHandle)
        {
            FILE_TYPE fileType = m_pMainInterface->getFileType(fileHandle);
            m_pMainInterface->destroyFileHandle(fileHandle);

            BaseListView *pListView = m_pAllClassesListView->value(m_strItemName[fileType]);
            if(pListView == nullptr ||
               (pListView->model() == nullptr) ||
               (pListView->model()->rowCount() <= 0))
            {
                return strName;
            }

            QAbstractItemModel *pModel = const_cast<QAbstractItemModel*>(pListView->model());
            BaseItemModel *pItemModel  = static_cast<BaseItemModel*>(pModel);
            for(int j = 0; j < pListView->model()->rowCount(); j++)
            {
                QStandardItem *pItem = pItemModel->item(j);
                if(pItem)
                {
                    QVariant userDataVariant = pItem->data(Qt::UserRole);
                    QByteArray userByteArray = userDataVariant.value<QByteArray>();
                    FileUserData *pUserData  = reinterpret_cast<FileUserData *>(userByteArray.data());

                    if(pUserData->strFilePath == strFile)
                    {
                        pItemModel->removeRow(j);

                        resizeEvent(nullptr);
                        onClassComboBoxActivated(ui->comboBox_class->currentIndex());

                        return strName;
                    }
                }
            }
        }
    }

    return strName;
}

void RecentDockWidget::setProxyModel()
{
    for(int i = 0; i < nCount; i++)
    {
        m_pAllClassesListView->value(m_strItemName[i])->setModel(m_pProxyModelArray[i]);
    }
}

void RecentDockWidget::addBlackVideo()
{
    QDir dir(Util::resourcesPath() + "/template/sampleVideo");
    if(dir.exists())
    {
        QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
        if(fileList.count() > 0)
        {
            for(QFileInfo file : fileList)
            {
                setItemModelInfo(file.filePath());
            }
        }
    }
}

int RecentDockWidget::setItemModelInfo(const QString &strFile)
{
    int nType              = -1;

    FILE_HANDLE fileHandle = m_pMainInterface->openFile(strFile);
    if(fileHandle)
    {
        QString strFileName  = Util::baseName(strFile);
        QStandardItem *pItem = new QStandardItem();
        pItem->setText(strFileName.split(".")[0]);  // 去除后缀
        pItem->setToolTip(strFileName);

        FILE_TYPE fileType = m_pMainInterface->getFileType(fileHandle);

        QIcon icon;
        if(fileType == FILE_TYPE_AUDIO)
        {   // 音频
            icon = QIcon(QPixmap().fromImage(QImage(":/icons/filters/Audio.png")));
        }
        else
        {
            icon = QIcon(QPixmap().fromImage(m_pMainInterface->getThumbnail(fileHandle)));
        }
        pItem->setIcon(icon);

        m_pMainInterface->destroyFileHandle(fileHandle);

        FileUserData *pFileUserData = new FileUserData();
        pFileUserData->strFilePath  = strFile;

        QByteArray userDataByteArray;
        userDataByteArray.append(reinterpret_cast<char *>(pFileUserData), sizeof(FileUserData));
        pItem->setData(userDataByteArray, Qt::UserRole);

        if(strFile.contains(Util::resourcesPath() + "/template/sampleVideo", Qt::CaseInsensitive))
        {   // 黑场视频
            nType = 0;
        }
        else
        {
            nType = fileType;
        }

        RecentItemModel *pModel = static_cast<RecentItemModel*>(m_pProxyModelArray[nType]->sourceModel());
        if(pModel)
        {
            pModel->insertRow(0, pItem);
        }
    }

    return nType;
}

FILE_HANDLE RecentDockWidget::getFileHandle(const QStandardItem *pItem)
{
    FILE_HANDLE fileHandle = nullptr;
    if(pItem)
    {
        QVariant userDataVariant    = pItem->data(Qt::UserRole);
        QByteArray userByteArray    = userDataVariant.value<QByteArray>();
        FileUserData *pFileUserData = reinterpret_cast<FileUserData *>(userByteArray.data());

        fileHandle = m_pMainInterface->openFile(pFileUserData->strFilePath);
    }

    return fileHandle;
}

void RecentDockWidget::hideModelTitle(int nIndex)
{
    QLayoutItem *pLayoutItem = ui->verticalLayout_scrollarea->itemAt(nIndex * 2);
    Q_ASSERT(pLayoutItem);
    if (pLayoutItem)
    {
        if (! pLayoutItem->layout())
        {
            return;
        }
        for(int i = 0; i < 2; i++)
        {
            QLayoutItem *pItem = pLayoutItem->layout()->itemAt(i);
            Q_ASSERT(pItem);
            if (! pItem)
            {
                return;
            }

            QWidget *pWidget = pItem->widget();
            Q_ASSERT(pWidget);
            if(pWidget)
            {
                pWidget->setVisible(false);
            }
        }
    }

    m_pAllClassesListView->value(m_strItemName[nIndex])->setVisible(false);
}

void RecentDockWidget::hideModelTitle(BaseItemModel *pModel)
{
    for(int i = 0; i < nCount; i++)
    {
        if(m_pProxyModelArray[i]->sourceModel() == pModel)
        {
            int nIndex = ui->comboBox_class->findText(m_strItemName[i]);
            if(nIndex >= 0)
            {
                ui->comboBox_class->removeItem(nIndex);
            }

            hideModelTitle(i);

            return;
        }
    }
}

void RecentDockWidget::showModelTitle(int nIndex)
{
    QLayoutItem *pLayoutItem = ui->verticalLayout_scrollarea->itemAt(nIndex * 2);
    Q_ASSERT(pLayoutItem);
    if (pLayoutItem)
    {
        if (! pLayoutItem->layout())
        {
            return;
        }
        for(int i = 0; i < 2; i++)
        {
            QLayoutItem *pItem = pLayoutItem->layout()->itemAt(i);
            Q_ASSERT(pItem);
            if (! pItem)
            {
                return;
            }

            QWidget *pWidget = pItem->widget();
            Q_ASSERT(pWidget);
            if(pWidget)
            {
                pWidget->setVisible(true);
            }
        }
    }

    m_pAllClassesListView->value(m_strItemName[nIndex])->setVisible(true);
}

void RecentDockWidget::preview(const QStandardItem *pItem)
{
    FILE_HANDLE fileHandle = getFileHandle(pItem);
    Q_ASSERT(fileHandle);
    if (fileHandle)
    {
        m_pMainInterface->playFile(fileHandle);

        m_pMainInterface->destroyFileHandle(fileHandle);
    }
}

void RecentDockWidget::addToTimeline(const QStandardItem *pItem)
{
    FILE_HANDLE fileHandle = getFileHandle(pItem);
    Q_ASSERT(fileHandle);
    if (fileHandle)
    {
        m_pMainInterface->addToTimeLine(fileHandle);

        m_pMainInterface->destroyFileHandle(fileHandle);
    }
}

QModelIndex RecentDockWidget::proxyToSource(const QModelIndex &index)
{
    if(index.isValid())
    {
        QAbstractItemModel *pModel         = const_cast<QAbstractItemModel*>(index.model());
        QSortFilterProxyModel *pProxyModel = qobject_cast<QSortFilterProxyModel*>(pModel);
        return pProxyModel->mapToSource(index);
    }
    else
    {
        return index;
    }
}

void RecentDockWidget::addItemToTimeline(const QModelIndex &index)
{
    QModelIndex modelIndex = proxyToSource(index);
    if(modelIndex.isValid())
    {
        QAbstractItemModel *pItemModel       = const_cast<QAbstractItemModel *>(modelIndex.model());
        BaseItemModel *pStandardItemModel    = static_cast<BaseItemModel *>(pItemModel);
        QStandardItem *pStandardItem         = pStandardItemModel->itemFromIndex(modelIndex);

        if(pStandardItem)
        {
            addToTimeline(pStandardItem);
        }
    }
}

void RecentDockWidget::clickedItem(const QModelIndex &index)
{
    QModelIndex modelIndex = proxyToSource(index);
    if(modelIndex.isValid())
    {
        QAbstractItemModel *pItemModel = const_cast<QAbstractItemModel *>(modelIndex.model());
        BaseItemModel *pBaseItemModel  = static_cast<BaseItemModel *>(pItemModel);
        QStandardItem *pStandardItem   = pBaseItemModel->itemFromIndex(modelIndex);

        m_pCurrentItem                 = pStandardItem;

        QMap<QString, BaseListView *>::const_iterator iter;
        for (iter = m_pAllClassesListView->constBegin(); iter != m_pAllClassesListView->constEnd(); iter++)
        {
            BaseListView *pListView = iter.value();
            if(pListView)
            {
                BaseItemModel *pTempItemModel = static_cast<BaseItemModel *>(pListView->model());
                if (pTempItemModel != pBaseItemModel)
                {
                    pListView->clearSelection();
                }
            }
        }

        if(pStandardItem)
        {
            preview(pStandardItem);
        }
    }
}

void RecentDockWidget::showMenu(const QModelIndex &index)
{
    if(index.isValid())
    {
        QMenu menu(this);
        menu.addAction(m_pRemoveAction);
        menu.addAction(m_pRemoveAllAction);
        menu.exec(QCursor::pos());
    }
}

QMap<QString, BaseItemModel *> * RecentDockWidget::createAllClassesItemModel()
{
    QMap<QString, BaseItemModel*> *pFileDockListViewItemModel = new QMap<QString, BaseItemModel*>();
    for( int i = 0; i < nCount; i++ )
    {
        RecentItemModel *pItemModel = new RecentItemModel(m_pMainInterface, this);
        pFileDockListViewItemModel->insert(m_strItemName[i], pItemModel);
        pFileDockListViewItemModel->setSharable(true);

        QSortFilterProxyModel *pProxyModel = new QSortFilterProxyModel(this);
        pProxyModel->setSourceModel(pItemModel);
        pProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);   // 忽略大小写
        m_pProxyModelArray[i] = pProxyModel;
    }

    for( QString strFile : m_recent )
    {
        if( ! strFile.endsWith(".mmp", Qt::CaseInsensitive) &&
            ! strFile.endsWith(".mlt", Qt::CaseInsensitive) &&
            ! strFile.endsWith(".xml", Qt::CaseInsensitive) )
        {
            setItemModelInfo(strFile);
        }
    }

    addBlackVideo();

    return pFileDockListViewItemModel;
}

void RecentDockWidget::on_actionRemove_triggered()
{
    if(m_pCurrentItem)
    {
        BaseItemModel *pItemModel  = static_cast<BaseItemModel*>(m_pCurrentItem->model());
        int nRow                   = m_pCurrentItem->row();
        QVariant userDataVariant   = m_pCurrentItem->data(Qt::UserRole);
        QByteArray userByteArray   = userDataVariant.value<QByteArray>();
        FileUserData *pUserData    = reinterpret_cast<FileUserData *>(userByteArray.data());

        if(pUserData->strFilePath.contains(Util::resourcesPath() + "/template/sampleVideo", Qt::CaseInsensitive))
        {
            ui->comboBox_class->setCurrentIndex(0);
        }
        else
        {
            FILE_HANDLE fileHandle = m_pMainInterface->openFile(pUserData->strFilePath);
            if(fileHandle)
            {
                FILE_TYPE fileType = m_pMainInterface->getFileType(fileHandle);

                ui->comboBox_class->setCurrentText(m_strItemName[fileType]);

                m_pMainInterface->destroyFileHandle(fileHandle);
            }
        }

        if(m_recent.removeOne(pUserData->strFilePath))
        {
            Settings.setRecent(m_recent);
        }

        if(pItemModel->removeRow(nRow))
        {
            if(pItemModel->rowCount() <= 0)
            {
                hideModelTitle(pItemModel);
            }

            m_pCurrentItem = nullptr;

            resizeEvent(nullptr);

            onClassComboBoxActivated(ui->comboBox_class->currentIndex());
        }
    }
}

void RecentDockWidget::on_actionRemoveAll_triggered()
{
    m_recent.clear();
    Settings.setRecent(m_recent);

    m_pCurrentItem = nullptr;

    // 清空 listView的 model
    for(int i = 0; i < nCount; i++)
    {
        RecentItemModel *pModel = static_cast<RecentItemModel*>(m_pProxyModelArray[i]->sourceModel());
        if(pModel)
        {
            while(pModel->rowCount() > 0)
            {
                pModel->removeRow(0);
            }
        }

        hideModelTitle(i);
    }
    ui->comboBox_class->clear();

    resizeEvent(nullptr);
}

void RecentDockWidget::onClassComboBoxActivated(int nIndex)
{
    for(int i = nIndex; i < nCount; i++)
    {
        QLayoutItem *pLayoutItem = ui->verticalLayout_scrollarea->itemAt(i * 2);
        if (pLayoutItem)
        {
            if (pLayoutItem->layout() == nullptr)
            {
                return;
            }
            if (pLayoutItem->layout()->itemAt(0) == nullptr)
            {
                return;
            }

            QWidget *pWidget = pLayoutItem->layout()->itemAt(0)->widget();
            if(pWidget)
            {
                QLabel *pLabel = qobject_cast<QLabel*>(pWidget);
                if(pLabel->text() == ui->comboBox_class->itemText(nIndex))
                {
                    ui->scrollArea->verticalScrollBar()->setValue(pWidget->y());

                    return;
                }
            }
        }
    }
}

void RecentDockWidget::on_lineEdit_textChanged(const QString &strSearch)
{
    for(int i = 0; i < nCount; i++)
    {
        Q_ASSERT(m_pProxyModelArray[i]);
        if(m_pProxyModelArray[i])
        {
            m_pProxyModelArray[i]->setFilterFixedString(strSearch);
        }
    }

    resizeEvent(nullptr);

    onClassComboBoxActivated(ui->comboBox_class->currentIndex());
}

void RecentDockWidget::onDockWidgetVisibility(bool bVisible)
{
    if(bVisible)
    {
        resizeEvent(nullptr);
    }
}

static RecentDockWidget *pInstance = nullptr;

QDockWidget *g_createRecentDock(MainInterface *pMainInterface)
{
    if(pMainInterface && (pInstance == nullptr))
    {
        pInstance = new RecentDockWidget(pMainInterface);
        pInstance->setupUi();
        pInstance->setProxyModel();
    }
    return pInstance;
}

void g_addFileToRecentDock(const QString &strFilePath)
{
    if(pInstance)
    {
        pInstance->add(strFilePath);
    }
}

void g_removeFileFromRecentDock(const QString &strFilePath)
{
    if(pInstance)
    {
        pInstance->remove(strFilePath);
    }
}
