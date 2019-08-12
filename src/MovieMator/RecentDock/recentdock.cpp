/*
 * Copyright (c) 2012-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: WanYuanCN <ebthon@hotmail.com>
 * Author: wyl <wyl@pylwyl.local>
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

#include "recentdock.h"
#include "settings.h"
#include "ui_recentdock.h"
#include "util.h"

#include <QDir>
#include <Logger.h>
#include <QMenu>
#include <QScrollBar>

//static const int MaxItems = 20;

RecentDock::RecentDock(MainInterface *main, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::RecentDock),
    m_mainWindow(main)
{
    LOG_DEBUG() << "begin";
    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());
    m_recent = Settings.recent();

    QString style1 = "QLineEdit {padding-right: 20px; background-image: url(:/icons/light/32x32/search-icon.png); background-repeat: norepeat; background-position: center; border: 1px; border-radius: 3px; background-color: rgb(100,100,100) ; }";
    QString style2= "QLineEdit {padding-right: 20px; border: 1px; border-radius: 3px; background-color: rgb(100,100,100) ; }";
    ui->lineEdit->setStyleSheets(style1, style2);

    m_listviewList     = new QList<RecentListView*>;
    m_modelList        = new QList<RecentListModel*>;
    m_currentListView  = nullptr;

    loadRecentFile();

    addBlackVideo();

    for(int i=0; i<num; i++)
    {
        QLabel *label = new QLabel(m_itemNames[i], this);
        label->setMinimumHeight(40);
        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QLabel *image = new QLabel(this);
        image->setScaledContents(true);
        image->setPixmap(QPixmap(":/icons/light/32x32/line.png"));
        image->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        QHBoxLayout *box = new QHBoxLayout();
        box->addWidget(label);
        box->addWidget(image);
        QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel;
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        proxyModel->setSourceModel(m_modelList->at(i));
        RecentListView *listView = new RecentListView();
        listView->setModel(proxyModel);
        listView->setViewMode(QListView::IconMode);
        listView->setFocusPolicy(Qt::NoFocus);
        listView->setGridSize(QSize(95, 90));     // 120, 100
        listView->setUniformItemSizes(true);
        listView->setResizeMode(QListView::Adjust);
        listView->setContentsMargins(0, 5, 0, 5);
        listView->setContextMenuPolicy(Qt::CustomContextMenu);
        listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listView->setStyleSheet(
                    "QListView::item:selected{background-color:rgb(192,72,44);color:rgb(255,255,255);border-radius:4px;}"
                    "QListView{background-color:transparent;color:rgb(214,214,214);}");

        connect(listView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(onListviewPressed(const QModelIndex&)));
        connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onListviewCustomContextMenuRequested(const QPoint&)));



        label->setVisible(false);
        image->setVisible(false);
        listView->setVisible(false);

        m_labelArray[i] = label;
        m_imageArray[i] = image;
        m_proxyArray[i] = proxyModel;
        m_listviewList->append(listView);

        ui->verticalLayout_2->addLayout(box);
        ui->verticalLayout_2->addWidget(listView);

        if(m_flag[i])
        {
            ui->comboBox->addItem(m_itemNames[i]);
            m_map.insert(ui->comboBox->count()-1, m_itemNames[i]);
            label->setVisible(true);
            image->setVisible(true);
            listView->setVisible(true);
        }
    }

    QSpacerItem *verticalSpacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_2->addItem(verticalSpacerItem);

    ui->comboBox->setFixedHeight(ui->lineEdit->height());
    ui->comboBox->setMinimumWidth(50);
    ui->comboBox->setStyleSheet("QComboBox{ background-color:rgb(100,100,100);color:rgb(225,225,225); }");
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setStyleSheet("border:none;");
    ui->scrollArea->verticalScrollBar()
            ->setStyleSheet("QScrollBar:vertical{width:8px;background:rgba(0,0,0,0%);margin:0px,0px,0px,0px;}"
                            "QScrollBar::handle:vertical{width:8px;background:rgba(160,160,160,25%);border-radius:4px;min-height:20;}"
                            "QScrollBar::handle:vertical:hover{width:8px;background:rgba(160,160,160,50%);border-radius:4px;min-height:20;}");


    // 添加到时间线的按钮，整个界面只有一个，而不是为每个 item都安置一个
    m_addToTimelineButton = new QPushButton(this);  // QIcon(":/icons/light/32x32/filter_add.png")
    m_addToTimelineButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/filter_add.png)}"
                                         "QPushButton:pressed{ border-image: url(:/icons/light/32x32/filter_add-a.png)}");
    m_addToTimelineButton->setFixedSize(QSize(27, 26));
    m_addToTimelineButton->setVisible(false);
    m_addToTimelineButton->setFocusPolicy(Qt::NoFocus);
    connect(m_addToTimelineButton, SIGNAL(clicked()), this, SLOT(addToTimeline()));

    LOG_DEBUG() << "end";
}

RecentDock::~RecentDock()
{
    qDeleteAll(*m_modelList);
    m_modelList->clear();
    delete m_modelList;
    m_modelList = nullptr;

    qDeleteAll(*m_listviewList);
    m_listviewList->clear();
    delete m_listviewList;
    m_listviewList = nullptr;

    delete ui;
}

void RecentDock::loadRecentFile() {
    RecentListModel *model1 = new RecentListModel(m_mainWindow, this);
    RecentListModel *model2 = new RecentListModel(m_mainWindow, this);
    RecentListModel *model3 = new RecentListModel(m_mainWindow, this);

    m_modelList->append(model1);
    m_modelList->append(model2);
    m_modelList->append(model3);

    foreach (QString s, m_recent) {
        // 工程文件不添加到历史记录列表里
        if(!s.endsWith(".mmp") && !s.endsWith(".xml") && !s.endsWith(".mlt"))
        {
            FILE_HANDLE fileHandle = m_mainWindow->openFile(s);
//            Q_ASSERT(fileHandle);     // 如果文件名s被改过了就无法打开
            if(fileHandle)
            {
                FileItemInfo *itemInfo = new FileItemInfo();
                Q_ASSERT(itemInfo);
                itemInfo->setFilePath(s);
                Q_ASSERT(m_mainWindow);
                itemInfo->setFileThumbnail(m_mainWindow->getThumbnail(fileHandle));
                if (m_mainWindow->getFileType(fileHandle) == FILE_TYPE_VIDEO) {
                    itemInfo->setFileType(FILE_TYPE_VIDEO);
                    model1->append(itemInfo);
                    m_flag[0] = true;
                } else if (m_mainWindow->getFileType(fileHandle) == FILE_TYPE_AUDIO) {
                    itemInfo->setFileType(FILE_TYPE_AUDIO);
                    model2->append(itemInfo);
                    m_flag[1] = true;
                } else if (m_mainWindow->getFileType(fileHandle) == FILE_TYPE_IMAGE) {
                    itemInfo->setFileType(FILE_TYPE_IMAGE);
                    model3->append(itemInfo);
                    m_flag[2] = true;
                }

                m_mainWindow->destroyFileHandle(fileHandle);
            }
        }
    }
}

void RecentDock::resizeEvent(QResizeEvent* event)
{
    Q_ASSERT(m_listviewList);
    if(!m_listviewList)
    {
        return;
    }
    for(RecentListView *listView : *m_listviewList)
    {
        Q_ASSERT(listView);
        if(!listView)
        {
            continue;
        }
        if(qobject_cast<RecentListModel*>(qobject_cast<QSortFilterProxyModel*>(listView->model())->sourceModel())->rowCount()<1)
//        if(qobject_cast<QSortFilterProxyModel*>(listView->model())->rowCount()<1)      // 可以过滤
        {
//            m_labelArray[0]->setVisible(false);
            listView->setVisible(false);
            continue;
        }
        listView->setVisible(true);
        listView->setFixedWidth(ui->scrollArea->width()-5);
        int wSize = listView->gridSize().width()/* +5*/;
        Q_ASSERT(wSize);
        if(wSize<=0)
        {
            continue;
        }
        int hSize = listView->gridSize().height();
        int width = listView->size().width();
        int columns = width/wSize<1 ? 1 : width/wSize;
        int rowCount = listView->model()->rowCount();
        int row = rowCount%columns>0 ? (rowCount/columns+1) : (rowCount/columns);
        listView->setFixedHeight(row*hSize);

        listView->setColumnCount(columns);
        if(listView==m_currentListView && m_currentIndex.isValid())
        {
            positionAddToTimelineButton();
        }
    }
//    on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());
    QDockWidget::resizeEvent(event);
}

void  RecentDock::positionAddToTimelineButton()
{
    if(!m_currentListView || !m_currentIndex.isValid())
        return;

    int columnCount = m_currentListView->getColumnCount();

//    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_currentListView->model());
//    Q_ASSERT(proxyModel);
//    RecentListModel *model = proxyModel ? qobject_cast<RecentListModel*>(proxyModel->sourceModel()) : nullptr;
//    Q_ASSERT(model);
//    if(!model)
//    {
//        return;
//    }

    int count = m_currentIndex.row() + 1;   //proxyModel->mapToSource(m_currentIndex).row() + 1;
    int row = (count % columnCount > 0) ? (count / columnCount + 1) : (count / columnCount);
    int column = count - (row-1)*columnCount;

    int gridWidth = m_currentListView->gridSize().width()
                    + m_currentListView->contentsMargins().left()       // 0
                    + m_currentListView->contentsMargins().right();     // 0
    int gridHeight = m_currentListView->gridSize().height()
                    + m_currentListView->contentsMargins().top()        // 5
                    + m_currentListView->contentsMargins().bottom();    // 5

    int width = m_addToTimelineButton->width();
    int height = m_addToTimelineButton->height();

    int x = column * gridWidth - width -5;
    int y = (row-1) * gridHeight;

    m_addToTimelineButton->setGeometry(x, y, width, height);
}

void RecentDock::addBlackVideo()
{
    Q_ASSERT(m_modelList->at(0));
    RecentListModel *pList = m_modelList->at(0);
    if(!pList)
    {
        return;
    }

    QString strTemplateDir = Util::resourcesPath() + "/template/";
    QDir dir(strTemplateDir + "samplevideo");
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    if(files.count() > 0)
    {
        for(QFileInfo file : files)
        {
            FILE_HANDLE fileHandle = m_mainWindow->openFile(file.filePath());
            if(fileHandle)
            {
                pList->append(fileHandle);
            }
        }
    }

    if(pList->rowCount() > 0)
    {
        m_flag[0] = true;
    }
    else
    {
        m_flag[0] = false;
    }
}

void RecentDock::addToTimeline()
{
    if (!m_currentListView || m_currentListView->model()->rowCount()<=0 || !m_currentIndex.isValid())
    {
        return;
    }

    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_currentListView->model());
    Q_ASSERT(proxyModel);
    RecentListModel *model = proxyModel ? qobject_cast<RecentListModel*>(proxyModel->sourceModel()) : nullptr;
    Q_ASSERT(model);
    if(model)
    {
        FILE_HANDLE fileHandle = model->fileAt(proxyModel->mapToSource(m_currentIndex).row());
        Q_ASSERT(fileHandle);
        Q_ASSERT(m_mainWindow);
        if(!fileHandle || !m_mainWindow)
        {
            return;
        }
        m_mainWindow->addToTimeLine(fileHandle);
    }
}

void RecentDock::add(const QString &s)
{
    Q_ASSERT(m_modelList);
    if(!m_modelList)
    {
        return;
    }
    if (s.startsWith(QDir::tempPath()))
        return;
    if (m_recent.contains(s))
        return;

    // 工程文件不添加到历史记录里
    if(s.endsWith(".mmp") || s.endsWith(".xml") || s.endsWith(".mlt"))
        return;

    FILE_HANDLE fileHandle = m_mainWindow->openFile(s);
    Q_ASSERT(fileHandle);
    if(fileHandle)
    {
        int index = 0;
        FILE_TYPE file_type = m_mainWindow->getFileType(fileHandle);
        FileItemInfo *itemInfo = new FileItemInfo();
        Q_ASSERT(itemInfo);
        itemInfo->setFilePath(s);
        itemInfo->setFileType(file_type);
        itemInfo->setFileThumbnail(m_mainWindow->getThumbnail(fileHandle));
        if (file_type == FILE_TYPE_VIDEO) {
            Q_ASSERT(m_modelList->at(0));
            if(!m_modelList->at(0))
            {
                delete itemInfo;
                itemInfo = nullptr;
                m_mainWindow->destroyFileHandle(fileHandle);
                return;
            }
            m_modelList->at(0)->insert(itemInfo, 0);
            m_recent.prepend(s);
            m_flag[0] = true;
            index = 0;
        } else if (file_type == FILE_TYPE_AUDIO) {
            Q_ASSERT(m_modelList->at(1));
            if(!m_modelList->at(1))
            {
                delete itemInfo;
                itemInfo = nullptr;
                m_mainWindow->destroyFileHandle(fileHandle);
                return;
            }
            m_modelList->at(1)->insert(itemInfo, 0);
            m_recent.prepend(s);
            m_flag[1] = true;
            index = 1;
        } else if (file_type == FILE_TYPE_IMAGE) {
            Q_ASSERT(m_modelList->at(2));
            if(!m_modelList->at(2))
            {
                delete itemInfo;
                itemInfo = nullptr;
                m_mainWindow->destroyFileHandle(fileHandle);
                return;
            }
            m_modelList->at(2)->insert(itemInfo, 0);
            m_recent.prepend(s);
            m_flag[2] = true;
            index = 2;
        }

        ui->comboBox->clear();
        m_map.clear();
        for(int i=0; i<num; i++)
        {
           if(m_flag[i])
           {
               Q_ASSERT(m_listviewList->at(i));
               Q_ASSERT(m_labelArray[i]);
               Q_ASSERT(m_imageArray[i]);
               if(!m_listviewList->at(i) || !m_labelArray[i] || !m_imageArray[i])
               {
                   delete itemInfo;
                   itemInfo = nullptr;
                   m_mainWindow->destroyFileHandle(fileHandle);
                   return;
               }
               ui->comboBox->addItem(m_itemNames[i]);
               m_map.insert(ui->comboBox->count()-1, m_itemNames[i]);
               m_listviewList->at(i)->setVisible(true);
               m_labelArray[i]->setVisible(true);
               m_imageArray[i]->setVisible(true);
           }
           if(index==i)
           {
               ui->comboBox->setCurrentText(m_itemNames[i]);
           }
        }

        resizeEvent(nullptr);

        m_mainWindow->destroyFileHandle(fileHandle);
        m_addToTimelineButton->setVisible(false);
    }
//    while (m_recent.count() > MaxItems)
//    {
//        m_recent.removeLast();
//        m_model->remove(m_model->rowCount()-1);
//    }
    Settings.setRecent(m_recent);
}

QString RecentDock::remove(const QString &s)
{
    m_recent.removeOne(s);
    Settings.setRecent(m_recent);

    bool flag = false;
    Q_ASSERT(m_modelList);
    if(!m_modelList)
    {
        return Util::baseName(s);
    }
    for(RecentListModel *model : *m_modelList)
    {
        Q_ASSERT(model);
        if(!model)
        {
            continue;
        }
        for(int i=0; i<model->rowCount(); i++)
        {
            if(QString::compare(s,model->fileAt(i)->filePath())==0)
            {
                model->remove(i);
                flag = true;
                break;
            }
        }
        if(flag)
        {
            break;
        }
    }

    QString name = Util::baseName(s);

    return name;
}

void RecentDock::on_lineEdit_textChanged(const QString& search)
{
    for(int i=0; i<num; i++)
    {
        Q_ASSERT(m_proxyArray[i]);
        if(!m_proxyArray[i])
        {
            continue;
        }
        m_proxyArray[i]->setFilterFixedString(search);
    }
//    Q_ASSERT(m_currentListView);  // 不用加，为空表示没有选中状态需要清空
    if(m_currentListView)
    {
        m_currentListView->clearSelection();
    }
    resizeEvent(nullptr);

    m_addToTimelineButton->setVisible(false);
}

void RecentDock::on_comboBox_activated(const QString &arg1)
{
    for(int i=m_map.key(arg1); i<num; i++)
    {
        Q_ASSERT(m_labelArray[i]);
        if(!m_labelArray[i])
        {
            continue;
        }
        if(m_labelArray[i]->text()==arg1)
        {
            ui->scrollArea->verticalScrollBar()->setValue(m_labelArray[i]->y());
            return;
        }
    }
}

void RecentDock::onListviewActivated(const QModelIndex &index)
{
    Q_UNUSED(index);
//    Q_ASSERT(m_currentListView);  // 不能加，默认下可以为空，不播放文件
    if (!m_currentListView || m_currentListView->model()->rowCount()<=0 || !m_currentIndex.isValid()) {
        return;
    }

    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_currentListView->model());
    Q_ASSERT(proxyModel);
    RecentListModel *model = proxyModel ? qobject_cast<RecentListModel*>(proxyModel->sourceModel()) : nullptr;
    Q_ASSERT(model);
    if(model)
    {
//        FILE_HANDLE fileHandle = model->fileAt(proxyModel->mapToSource(m_currentIndex).row());
        FileItemInfo *itemInfo = model->fileAt(proxyModel->mapToSource(m_currentIndex).row());
        Q_ASSERT(itemInfo);
        Q_ASSERT(m_mainWindow);
        FILE_HANDLE fileHandle = m_mainWindow->openFile(itemInfo->filePath());
        Q_ASSERT(fileHandle);
        if(!fileHandle || !m_mainWindow)
        {
            return;
        }
        m_mainWindow->playFile(fileHandle);

        m_mainWindow->destroyFileHandle(fileHandle);
    }
}

void RecentDock::onListviewPressed(const QModelIndex &index)
{
//    Q_ASSERT(m_listviewList);
//    if(!m_listviewList)
//    {
//        return;
//    }
//    for(RecentListView *listView : *m_listviewList)
//    {
//        Q_ASSERT(listView);
//        if(!listView)
//        {
//            continue;
//        }
//        Q_ASSERT(index.isValid());
//        if(index.isValid() && index==listView->currentIndex())
//        {
//            if(m_currentListView && m_currentListView!=listView)
//            {
//                m_currentListView->clearSelection();
//            }
//            m_currentIndex = index;
//            m_currentListView = listView;
////            return;
//            break;
//        }
//    }

    if(!sender())
        return;

    RecentListView *listView = static_cast<RecentListView*>(sender());
    if(m_currentListView && m_currentListView!=listView)
    {
        m_currentListView->clearSelection();
    }
    m_currentListView = listView;
    m_currentIndex = index;

    if(m_currentListView && m_currentIndex.isValid())
    {
        m_addToTimelineButton->setParent(m_currentListView);
        m_addToTimelineButton->setVisible(true);
        positionAddToTimelineButton();
    }

    onListviewActivated(QModelIndex());     // 按下就播放
}

void RecentDock::onListviewCustomContextMenuRequested(const QPoint &pos)
{
    Q_ASSERT(m_currentListView);
    Q_ASSERT(m_currentIndex.isValid());
    if(m_currentIndex.isValid() && m_currentListView->indexAt(pos).isValid())
    {
        QMenu menu(this);
        menu.addAction(ui->actionRemove);
        menu.addAction(ui->actionRemoveAll);
        menu.addAction(ui->actionPlay);
        menu.exec(m_currentListView->mapToGlobal(pos));
    }
}

void RecentDock::on_actionRemove_triggered()
{
    Q_ASSERT(m_currentListView);
//    Q_ASSERT(m_currentIndex.isValid());//当list为空时，点击toolbar上的remove会出现m_currentIndex无效
    if(!m_currentListView || !m_currentIndex.isValid())
    {
        return;
    }

    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_currentListView->model());
    Q_ASSERT(proxyModel);
    if(!proxyModel)
    {
        return;
    }
    RecentListModel *model = qobject_cast<RecentListModel*>(proxyModel->sourceModel());
    Q_ASSERT(model);
    if(!model)
    {
        return;
    }
    int row = proxyModel->mapToSource(m_currentIndex).row();
    if(m_recent.removeOne(model->fileAt(row)->filePath()))
    {
        Settings.setRecent(m_recent);
        model->remove(row);

        m_currentIndex = m_currentListView->currentIndex();

        if(model->rowCount()==0)
        {
            for(int i=0; i<num; i++){
                if(m_listviewList->at(i)==m_currentListView)
                {
                    m_flag[i] = false;
                    m_currentListView->setVisible(false);
                    Q_ASSERT(m_labelArray[i]);
                    if(m_labelArray[i])
                    {
                        m_labelArray[i]->setVisible(false);
                        QString itemName = m_labelArray[i]->text();
                        m_map.remove(m_map.key(itemName));
                        ui->comboBox->removeItem(ui->comboBox->findText(itemName));
                    }
                    Q_ASSERT(m_imageArray[i]);
                    if(m_imageArray[i])
                    {
                        m_imageArray[i]->setVisible(false);
                    }
                    break;
                }
            }
        }
        resizeEvent(nullptr);
    }
}

void RecentDock::on_actionRemoveAll_triggered()
{
    Q_ASSERT(m_modelList);
    Q_ASSERT(m_listviewList);
    if(!m_modelList || !m_listviewList)
    {
        return;
    }
    for(int i=0; i<num; i++)
    {
        m_flag[i] = false;
        Q_ASSERT(m_modelList->at(i));
        if(m_modelList->at(i))
        {
            m_modelList->at(i)->clear();
        }
        Q_ASSERT(m_labelArray[i]);
        if(m_labelArray[i])
        {
            m_labelArray[i]->setVisible(false);
        }
        Q_ASSERT(m_imageArray[i]);
        if(m_imageArray[i])
        {
            m_imageArray[i]->setVisible(false);
        }
        Q_ASSERT(m_listviewList->at(i));
        if(m_listviewList->at(i))
        {
            m_listviewList->at(i)->setVisible(false);
        }
    }

    m_currentIndex = m_currentListView->currentIndex();

    ui->comboBox->clear();
    m_map.clear();

    m_recent.clear();
    Settings.setRecent(m_recent);
}

void RecentDock::on_actionPlay_triggered()
{
    Q_ASSERT(m_currentIndex.isValid());
    if(!m_currentIndex.isValid())
    {
        return;
    }
    onListviewActivated(m_currentIndex);
}

void RecentDock::on_actionProperties_triggered()
{

}

void RecentDock::on_RecentDock_visibilityChanged(bool visible)
{
    if (visible) {
        onListviewActivated(QModelIndex());

        resizeEvent(nullptr);   // 切换dock后listView大小会变化
    }
}

static RecentDock *instance = nullptr;
//初始化模块
//参数，main 主程序接口对象
//返回界面对象
QDockWidget *RecentDock_initModule(MainInterface *main)
{
    if (instance == nullptr)
        instance = new RecentDock(main);
    return instance;
}

//销毁模块
void RecentDock_destroyModule()
{

}

//添加文件
void RecentDock_add(QString filePath)
{
    instance->add(filePath);
}

//删除文件
void RecentDock_remove(QString filePath)
{
    instance->remove(filePath);
}

//删除选中项
void RecentDock_removeSelectedItem() {
    instance->on_actionRemove_triggered();
}

