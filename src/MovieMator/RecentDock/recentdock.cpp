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

    m_listviewList = new QList<RecentListView*>;
    m_modelList = new QList<RecentListModel*>;
    m_currentListView = nullptr;

    RecentListModel *model1 = new RecentListModel(main, this);
    RecentListModel *model2 = new RecentListModel(main, this);
    RecentListModel *model3 = new RecentListModel(main, this);

    m_modelList->append(model1);
    m_modelList->append(model2);
    m_modelList->append(model3);

    foreach (QString s, m_recent) {
        // 工程文件不添加到历史记录列表里
        if(!s.endsWith(".mmp") && !s.endsWith(".xml") && !s.endsWith(".mlt"))
        {
            FILE_HANDLE fileHandle = m_mainWindow->openFile(s);
            if(fileHandle)
            {
                if (m_mainWindow->getFileType(fileHandle) == FILE_TYPE_VIDEO) {
                    model1->append(fileHandle);
                    m_flag[0] = true;
                } else if (m_mainWindow->getFileType(fileHandle) == FILE_TYPE_AUDIO) {
                    model2->append(fileHandle);
                    m_flag[1] = true;
                } else if (m_mainWindow->getFileType(fileHandle) == FILE_TYPE_IMAGE) {
                    model3->append(fileHandle);
                    m_flag[2] = true;
                }
            }
        }
    }

    for(int i=0; i<num; i++)
    {
        QLabel *label = new QLabel(m_itemNames[i], this);
        label->setMinimumHeight(40);
        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QLabel *image = new QLabel(this);
        image->setScaledContents(true);
        image->setPixmap(QPixmap(":/icons/light/32x32/line.png"));
        image->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        QHBoxLayout *box = new QHBoxLayout(this);
        box->addWidget(label);
        box->addWidget(image);
        QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel;
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        proxyModel->setSourceModel(m_modelList->at(i));
        RecentListView *listView = new RecentListView();
        listView->setModel(proxyModel);
        listView->setViewMode(QListView::IconMode);
        listView->setFocusPolicy(Qt::ClickFocus);
        listView->setGridSize(QSize(95, 90));     // 120, 100
        listView->setUniformItemSizes(true);
        listView->setResizeMode(QListView::Adjust);
        listView->setContentsMargins(0, 5, 0, 5);
        listView->setContextMenuPolicy(Qt::CustomContextMenu);
        listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listView->setStyleSheet(
                    "QListView::item:selected{background-color:rgb(192,72,44);color:rgb(255,255,255);}"
                    "QListView{background-color:transparent;color:rgb(214,214,214);}");

        connect(listView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_listView_pressed(const QModelIndex&)));
        connect(listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(on_listView_clicked(const QModelIndex&)));
        connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_listView_customContextMenuRequested(const QPoint&)));

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

    m_verticalSpacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_2->addItem(m_verticalSpacerItem);

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

    LOG_DEBUG() << "end";
}

RecentDock::~RecentDock()
{
//    delete m_verticalSpacerItem;
//    m_verticalSpacerItem = nullptr;

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

void RecentDock::resizeEvent(QResizeEvent* event)
{
    for(RecentListView *listView : *m_listviewList)
    {
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
        int hSize = listView->gridSize().height();
        int width = listView->size().width();
        int columns = width/wSize<1 ? 1 : width/wSize;
        int rowCount = listView->model()->rowCount();
        int row = rowCount%columns>0 ? (rowCount/columns+1) : (rowCount/columns);
        listView->setFixedHeight(row*hSize);
    }
//    on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());
    QDockWidget::resizeEvent(event);
}

void RecentDock::add(const QString &s)
{
    if (s.startsWith(QDir::tempPath()))
        return;
    if (m_recent.contains(s))
        return;

    // 工程文件不添加到历史记录里
    if(s.endsWith(".mmp") || s.endsWith(".xml") || s.endsWith(".mlt"))
        return;

    FILE_HANDLE fileHandle = m_mainWindow->openFile(s);
    if(fileHandle)
    {
        int index = 0;
        FILE_TYPE file_type = m_mainWindow->getFileType(fileHandle);
        if (file_type == FILE_TYPE_VIDEO) {
            m_modelList->at(0)->insert(fileHandle, 0);
            m_recent.prepend(s);
            m_flag[0] = true;
            index = 0;
        } else if (file_type == FILE_TYPE_AUDIO) {
            m_modelList->at(1)->insert(fileHandle, 0);
            m_recent.prepend(s);
            m_flag[1] = true;
            index = 1;
        } else if (file_type == FILE_TYPE_IMAGE) {
            m_modelList->at(2)->insert(fileHandle, 0);
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
    for(RecentListModel *model : *m_modelList)
    {
        for(int i=0; i<model->rowCount(); i++)
        {
            if(QString::compare(s, model->fileName(i))==0)
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
        m_proxyArray[i]->setFilterFixedString(search);
    }
    if(m_currentListView)
    {
        m_currentListView->clearSelection();
    }
    resizeEvent(nullptr);
}

void RecentDock::on_comboBox_currentTextChanged(const QString &arg1)
{
    for(int i=m_map.key(arg1); i<num; i++)
    {
        if(m_labelArray[i]->text()==arg1)
        {
            ui->scrollArea->verticalScrollBar()->setValue(m_labelArray[i]->y());
            return;
        }
    }
}

void RecentDock::on_listView_activated(const QModelIndex &index)
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_currentListView->model());
    RecentListModel *model = proxyModel ? qobject_cast<RecentListModel*>(proxyModel->sourceModel()) : nullptr;
    if(model)
    {
        FILE_HANDLE fileHandle = model->fileAt(proxyModel->mapToSource(index).row());
        m_mainWindow->playFile(fileHandle);
    }
}

void RecentDock::on_listView_pressed(const QModelIndex &index)
{
    for(RecentListView *listView : *m_listviewList)
    {
        if(index.isValid() && index==listView->currentIndex())
        {
            if(m_currentListView && m_currentListView!=listView)
            {
                m_currentListView->clearSelection();
            }
            m_currentIndex = index;
            m_currentListView = listView;
            return;
        }
    }
}

void RecentDock::on_listView_clicked(const QModelIndex &index)
{
    if(m_currentIndex.isValid() && m_currentListView && index==m_currentIndex)
    {
        QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_currentListView->model());
        RecentListModel *model = proxyModel ? qobject_cast<RecentListModel*>(proxyModel->sourceModel()) : nullptr;
        if(model)
        {
            FILE_HANDLE fileHandle = model->fileAt(proxyModel->mapToSource(m_currentIndex).row());
            m_mainWindow->playFile(fileHandle);
        }
    }
}

void RecentDock::on_listView_customContextMenuRequested(const QPoint &pos)
{
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
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_currentListView->model());
    RecentListModel *model = qobject_cast<RecentListModel*>(proxyModel->sourceModel());
    int row = proxyModel->mapToSource(m_currentIndex).row();
    if(m_recent.removeOne(model->fileName(row)))
    {
        Settings.setRecent(m_recent);
        model->remove(row);

        if(model->rowCount()==0)
        {
            for(int i=0; i<num; i++){
                if(m_listviewList->at(i)==m_currentListView)
                {
                    m_flag[i] = false;
                    m_currentListView->setVisible(false);
                    m_labelArray[i]->setVisible(false);
                    m_imageArray[i]->setVisible(false);
                    QString itemName = m_labelArray[i]->text();
                    m_map.remove(m_map.key(itemName));
                    ui->comboBox->removeItem(ui->comboBox->findText(itemName));
                    break;
                }
            }
        }
        resizeEvent(nullptr);
    }
}

void RecentDock::on_actionRemoveAll_triggered()
{
    for(int i=0; i<num; i++)
    {
        m_flag[i] = false;
        m_modelList->at(i)->clear();
        m_labelArray[i]->setVisible(false);
        m_imageArray[i]->setVisible(false);
        m_listviewList->at(i)->setVisible(false);
    }

    ui->comboBox->clear();
    m_map.clear();

    m_recent.clear();
    Settings.setRecent(m_recent);
}

void RecentDock::on_actionPlay_triggered()
{
    on_listView_activated(m_currentIndex);
}

void RecentDock::on_actionProperties_triggered()
{

}

QList<FILE_HANDLE> RecentDock::getSelected()
{
    QList<FILE_HANDLE> selected;
    if(m_currentListView)
    {
        QModelIndexList selectedIndexes = m_currentListView->getSelected();

        foreach(QModelIndex index, selectedIndexes)
        {
            QModelIndex sourceIndex = qobject_cast<QSortFilterProxyModel*>(m_currentListView->model())->mapToSource(index);
            FILE_HANDLE fileHandle = qobject_cast<RecentListModel*>(qobject_cast<QSortFilterProxyModel*>(m_currentListView->model())->sourceModel())->fileAt(sourceIndex.row());

            selected.append(fileHandle);
        }
    }
    return selected;
}

static RecentDock *instance = 0;
//初始化模块
//参数，main 主程序接口对象
//返回界面对象
QDockWidget *RecentDock_initModule(MainInterface *main)
{
    if (instance == NULL)
        instance = new RecentDock(main);
    return instance;
}

//销毁模块
void RecentDock_destroyModule()
{

}

//获取选中的文件列表
QList<FILE_HANDLE> RecentDock_getSelectedFiles()
{
    return instance->getSelected();
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

