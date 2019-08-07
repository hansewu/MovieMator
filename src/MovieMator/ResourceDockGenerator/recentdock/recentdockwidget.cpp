#include "recentdockwidget.h"
#include "recentitemmodel.h"

#include "ui_basedockwidget.h"
#include "baselistview.h"
#include "lineeditclear.h"
#include "uiuserdef.h"

#include "util.h"
#include "settings.h"

#include <QDir>
#include <QMenu>
#include <QDebug>
#include <QPainter>
#include <QScrollBar>

RecentDockWidget::RecentDockWidget(MainInterface *pMainInterface, QWidget *pParent) :
    BaseDockWidget (pParent),
    m_pMainInterface(pMainInterface)
{
    m_recent            = Settings.recent();
    m_listItemNames     = {tr("0 Samples"), tr("1 Videos"), tr("2 Audios"), tr("3 Images")};

    m_pRemoveAction     = new QAction(this);
    m_pRemoveAllAction  = new QAction(this);

    m_pRemoveAction->setObjectName(QStringLiteral("actionRemove"));
    m_pRemoveAction->setText(tr("Remove"));
    m_pRemoveAllAction->setObjectName(QStringLiteral("actionRemoveAll"));
    m_pRemoveAllAction->setText(tr("Remove All"));

    connect(m_pRemoveAction, SIGNAL(triggered()), this, SLOT(on_actionRemove_triggered()));
    connect(m_pRemoveAllAction, SIGNAL(triggered()), this, SLOT(on_actionRemoveAll_triggered()));
}

RecentDockWidget::~RecentDockWidget()
{
    for(int i = 0; i < m_listProxyModel.count(); i++)
    {
        RecentItemModel *pModel = static_cast<RecentItemModel*>(m_listProxyModel[i]->sourceModel());
        if(pModel)
        {
            while(pModel->rowCount() > 0)
            {
                pModel->removeRow(0);
            }
            delete pModel;
        }
        delete m_listProxyModel[i];
    }
}

void RecentDockWidget::setupTopBarUi()
{
    BaseDockWidget::setupTopBarUi();

    LineEditClear *pSearchLineEdit = new LineEditClear(this);
    pSearchLineEdit->setObjectName(QStringLiteral("searchLineEdit"));
    pSearchLineEdit->setPlaceholderText(tr("Search"));
    pSearchLineEdit->setFixedHeight(ui->comboBox_class->height());
    pSearchLineEdit->setMaximumWidth(350);
    pSearchLineEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    connect(pSearchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(on_lineEdit_textChanged(QString)));

    ui->horizontalLayout_titlebar->addWidget(pSearchLineEdit);
    ui->horizontalLayout_titlebar->setStretchFactor(pSearchLineEdit, 2);

    ui->horizontalSpacer->changeSize(10, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

void RecentDockWidget::add(const QString &strFile)
{
    if(strFile.startsWith(QDir::tempPath(), Qt::CaseInsensitive))
    {
        return;
    }
    if(strFile.startsWith(Util::resourcesPath(), Qt::CaseInsensitive))
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

    int nTypeIndex = setItemModelInfo(strFile);
    if(nTypeIndex >= 0)
    {
        m_recent.prepend(strFile);
        Settings.setRecent(m_recent);

        if(ui->comboBox_class->findText(m_listItemNames[nTypeIndex]) < 0)
        {
            ui->comboBox_class->clear();
            showModelTitle(nTypeIndex);

            for(int i = 0; i < m_listProxyModel.count(); i++)
            {
                if(m_listProxyModel[i]->sourceModel()->rowCount() > 0)
                {
                    ui->comboBox_class->addItem(m_listItemNames[i]);
                }
            }
        }

        ui->comboBox_class->setCurrentText(m_listItemNames[nTypeIndex]);

        updateListViewSize();
    }
}

void RecentDockWidget::setProxyModel()
{
    Q_ASSERT(m_listItemNames.count() == m_listProxyModel.count());
    if(m_listItemNames.count() == m_listProxyModel.count())
    {
        for(int i = 0; i < m_listItemNames.count(); i++)
        {
            m_pAllClassesListView->value(m_listItemNames[i])->setModel(m_listProxyModel[i]);

            if(m_listProxyModel[i]->sourceModel()->rowCount() <= 0)
            {
                hideModelTitle(i);
            }
        }
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

QIcon RecentDockWidget::getItemIcon(const QString &strFile)
{
    FILE_HANDLE fileHandle = m_pMainInterface->openFile(strFile);
    if(fileHandle)
    {
        FILE_TYPE fileType = m_pMainInterface->getFileType(fileHandle);

        QImage iconImage = QImage(LISTVIEW_ITEMICONSIZE_WIDTH,
                                  LISTVIEW_ITEMICONSIZE_HEIGHT,
                                  QImage::Format_ARGB32);
        QImage image;
        if(fileType == FILE_TYPE_AUDIO)
        {   // 音频
            image = QImage(":/icons/filters/Audio.png");
        }
        else if(fileType == FILE_TYPE_VIDEO ||
                strFile.contains(Util::resourcesPath(), Qt::CaseInsensitive))
        {   // 视频和黑场视频
            image = m_pMainInterface->getThumbnail(fileHandle);
        }
        else
        {   //图片
            image = QImage(strFile);
        }

        if (!image.isNull())
        {
            if(strFile.contains(Util::resourcesPath(), Qt::CaseInsensitive))
            {   // 黑场视频
                iconImage.fill(image.pixel(image.width()/2, image.height()/2));
            }
            else
            {
                QPainter painter(&iconImage);
                iconImage.fill(QApplication::palette().base().color().rgb());
                QRect rect = image.rect();
                rect.setWidth(LISTVIEW_ITEMICONSIZE_WIDTH);
                rect.setHeight(LISTVIEW_ITEMICONSIZE_HEIGHT);
                painter.drawImage(rect, image);
                painter.end();
            }
        }
        else
        {
            iconImage.fill(QApplication::palette().base().color().rgb());
        }

        m_pMainInterface->destroyFileHandle(fileHandle);

        return QPixmap::fromImage(iconImage);
    }

    return QIcon(strFile);
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

        QIcon icon = getItemIcon(strFile);
        pItem->setIcon(icon);

        m_pMainInterface->destroyFileHandle(fileHandle);

        FileUserData *pFileUserData = new FileUserData();
        pFileUserData->strFilePath  = strFile;

        QByteArray userDataByteArray;
        userDataByteArray.append(reinterpret_cast<char *>(pFileUserData), sizeof(FileUserData));
        pItem->setData(userDataByteArray, Qt::UserRole);

        if(strFile.contains(Util::resourcesPath(), Qt::CaseInsensitive))
        {   // 黑场视频
            nType = 0;
        }
        else
        {
            nType = fileType;
        }

        RecentItemModel *pModel = static_cast<RecentItemModel*>(m_listProxyModel[nType]->sourceModel());
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

    int nItemIndex = ui->comboBox_class->findText(m_listItemNames[nIndex]);
    if(nItemIndex >= 0)
    {
        ui->comboBox_class->removeItem(nItemIndex);
    }

    m_pAllClassesListView->value(m_listItemNames[nIndex])->setVisible(false);
}

void RecentDockWidget::hideModelTitle(BaseItemModel *pModel)
{
    Q_ASSERT(m_listProxyModel.count() == m_listItemNames.count());
    if(m_listItemNames.count() != m_listProxyModel.count())
    {
        return;
    }

    for(int i = 0; i < m_listProxyModel.count(); i++)
    {
        if(m_listProxyModel[i]->sourceModel() == pModel)
        {
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

    m_pAllClassesListView->value(m_listItemNames[nIndex])->setVisible(true);
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

void RecentDockWidget::addItemToTimeline(const QStandardItem *pItem)
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

void RecentDockWidget::onLeftClickedAddButtonInItem(const QModelIndex &index)
{
    QModelIndex modelIndex = proxyToSource(index);
    if(modelIndex.isValid())
    {
        QAbstractItemModel *pItemModel       = const_cast<QAbstractItemModel *>(modelIndex.model());
        BaseItemModel *pStandardItemModel    = static_cast<BaseItemModel *>(pItemModel);
        QStandardItem *pStandardItem         = pStandardItemModel->itemFromIndex(modelIndex);

        if(pStandardItem)
        {
            addItemToTimeline(pStandardItem);
        }
    }
}

void RecentDockWidget::onLeftClickedItem(const QModelIndex &index)
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

void RecentDockWidget::onClassComboBoxActivated(int nIndex)
{
    qDebug()<<"sll---------onClassComboBoxActivated--start";
    for(int i = nIndex; i < m_listItemNames.count(); i++)
    {
        QLayoutItem *pLayoutItem = ui->verticalLayout_scrollarea->itemAt(i * 2);
        if (pLayoutItem == nullptr)
        {
            break;
        }
        if (pLayoutItem)
        {
            if (pLayoutItem->layout() == nullptr)
            {
                break;
            }
            if (pLayoutItem->layout()->itemAt(0) == nullptr)
            {
                break;
            }

            QWidget *pWidget = pLayoutItem->layout()->itemAt(0)->widget();
            if(pWidget)
            {
                QLabel *pLabel = qobject_cast<QLabel*>(pWidget);
                if(pLabel->text() == ui->comboBox_class->itemText(nIndex))
                {
                    ui->scrollArea->verticalScrollBar()->setValue(pWidget->y());
                    break;
                }
            }
        }
    }
    qDebug()<<"sll---------onClassComboBoxActivated--end";
}

void RecentDockWidget::onRightClickedItem(const QModelIndex &index, const QPoint &position)
{
    QModelIndex modelIndex = proxyToSource(index);
    if(modelIndex.isValid())
    {
        QAbstractItemModel *pItemModel = const_cast<QAbstractItemModel *>(modelIndex.model());
        BaseItemModel *pBaseItemModel  = static_cast<BaseItemModel *>(pItemModel);
        QStandardItem *pStandardItem   = pBaseItemModel->itemFromIndex(modelIndex);

        showMeun(pStandardItem, position);
    }
}

QMap<QString, BaseItemModel *> * RecentDockWidget::createAllClassesItemModel()
{
    QMap<QString, BaseItemModel*> *pFileDockListViewItemModel = new QMap<QString, BaseItemModel*>();
    for( int i = 0; i < m_listItemNames.count(); i++ )
    {
        RecentItemModel *pItemModel = new RecentItemModel(m_pMainInterface, this);
        pFileDockListViewItemModel->insert(m_listItemNames[i], pItemModel);
        pFileDockListViewItemModel->setSharable(true);

        QSortFilterProxyModel *pProxyModel = new QSortFilterProxyModel(this);
        pProxyModel->setSourceModel(pItemModel);
        pProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);   // 忽略大小写
        m_listProxyModel.append(pProxyModel);
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

void RecentDockWidget::showMeun(const QStandardItem *pItem, const QPoint &position)
{
    Q_UNUSED(position);
    if(pItem)
    {
        QMenu menu(this);
        menu.addAction(m_pRemoveAction);
        menu.addAction(m_pRemoveAllAction);
        menu.exec(QCursor::pos());
    }
}

void RecentDockWidget::on_actionRemove_triggered()
{
    if(m_pCurrentItem)
    {
        BaseItemModel *pItemModel  = static_cast<BaseItemModel*>(m_pCurrentItem->model());
        QVariant userDataVariant   = m_pCurrentItem->data(Qt::UserRole);
        QByteArray userByteArray   = userDataVariant.value<QByteArray>();
        FileUserData *pUserData    = reinterpret_cast<FileUserData *>(userByteArray.data());

        if(pUserData->strFilePath.contains(Util::resourcesPath(), Qt::CaseInsensitive))
        {
            ui->comboBox_class->setCurrentIndex(0);
        }
        else
        {
            FILE_HANDLE fileHandle = m_pMainInterface->openFile(pUserData->strFilePath);
            if(fileHandle)
            {
                FILE_TYPE fileType = m_pMainInterface->getFileType(fileHandle);

                ui->comboBox_class->setCurrentText(m_listItemNames[fileType]);

                m_pMainInterface->destroyFileHandle(fileHandle);
            }
        }

        if(m_recent.removeOne(pUserData->strFilePath))
        {
            Settings.setRecent(m_recent);
        }

        if(pItemModel->removeRow(m_pCurrentItem->row()))
        {
            if(pItemModel->rowCount() <= 0)
            {
                hideModelTitle(pItemModel);
            }

            m_pCurrentItem = nullptr;

            updateListViewSize();
        }
    }
}

void RecentDockWidget::on_actionRemoveAll_triggered()
{
    m_recent.clear();
    Settings.setRecent(m_recent);

    m_pCurrentItem = nullptr;

    // 清空 listView的 model
    for(int i = 0; i < m_listProxyModel.count(); i++)
    {
        RecentItemModel *pModel = static_cast<RecentItemModel*>(m_listProxyModel[i]->sourceModel());
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

    updateListViewSize();
}

void RecentDockWidget::on_lineEdit_textChanged(const QString &strSearch)
{
    for(int i = 0; i < m_listProxyModel.count(); i++)
    {
        Q_ASSERT(m_listProxyModel[i]);
        if(m_listProxyModel[i])
        {
            m_listProxyModel[i]->setFilterFixedString(strSearch);
        }
    }

    //FIXME:此方法相对与目前已知的方法（定时器、线程），相对较好，但还需要优化（如何能准去知道dock中的widget位置已更新完成？）
    //实现当搜索框中的关键此被清空时，依然能跳转到搜索前分类的位置
    //实际上可以直接调用onClassComboBoxActivated函数进行跳转：onClassComboBoxActivated(ui->comboBox_class->currentIndex());
    //但是由于在清除搜索框内容时，会不断的刷新可显示item（增加或者删除），更新需要一定的时间，但目前无法知道什么时候完成，
    //造成在直接调用onClassComboBoxActivated跳转时，dock中的widget位置还未刷新完成，从而跳转失败（之前的位置）
    //由于dock的resize操作可以立即重绘dock中的控件，因此目前模拟了一个dock resize的操作，先改变一个像素，然后在复原，主要用于触发resizeevent事件.进行立即重绘
    //备注：也尝试过定时器，用一个大约10毫秒的定时器也可解决此问题，但由于设备性能不同，刷新完成时间不好确定，定时器方法不可缺
    QSize originSize = size();
    resize(originSize.width() + 1, originSize.height());
    resize(originSize);
}

static RecentDockWidget *pInstance = nullptr;

QDockWidget *RDG_CreateRecentDock(MainInterface *pMainInterface)
{
    if(pMainInterface && (pInstance == nullptr))
    {
        pInstance = new RecentDockWidget(pMainInterface);
        pInstance->setupUi();
        pInstance->setProxyModel();
    }
    return pInstance;
}

void RDG_AddFileToRecentDock(const QString &strFilePath)
{
    if(pInstance)
    {
        pInstance->add(strFilePath);
    }
}
