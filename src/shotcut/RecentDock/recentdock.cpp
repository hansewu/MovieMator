/*
 * Copyright (c) 2012-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
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

#include <QSpacerItem>

static const int MaxItems = 20;

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
        if(!s.endsWith(".mmp"))
        {
            FILE_HANDLE fileHandle = m_mainWindow->openFile(s);

            if(fileHandle)
            {
                if(s.endsWith(".avi") || s.endsWith(".mov") || s.endsWith(".mp4"))   // 视频
                {
                    model1->append(fileHandle);
                    m_flag[0] = true;
                }
                else if(s.endsWith(".mp3"))  // 音频
                {
                    model2->append(fileHandle);
                    m_flag[1] = true;
                }
                else/* if(s.endsWith(".png"))*/  // 图片
                {
                    model3->append(fileHandle);
                    m_flag[2] = true;
                }
//*/
            }
        }
    }

//    /*
    for(int i=0; i<num; i++)
    {
        QLabel *label = new QLabel(m_itemNames[i]);
        RecentListView *listView = new RecentListView();

        label->setMinimumHeight(30);

        QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel;
        proxyModel->setSourceModel(m_modelList->at(i));

        m_proxyArray[i] = proxyModel;
        listView->setModel(proxyModel);

        listView->setViewMode(QListView::IconMode);
        listView->setFocusPolicy(Qt::StrongFocus);
        listView->setGridSize(QSize(110, 90));
        listView->setResizeMode(QListView::Adjust);

        listView->setContextMenuPolicy(Qt::CustomContextMenu);
        listView->setStyleSheet("QListView{selection-background-color:rgb(192,72,44); selection-color: rgb(255,255,255);background-color:rgb(51,51,51);color:rgb(214,214,214);}");
        listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        connect(listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(on_listView_clicked(const QModelIndex&)));
        connect(listView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(on_listView_doubleClicked(const QModelIndex&)));
        connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_listView_customContextMenuRequested(const QPoint&)));

        ui->verticalLayout_2->addWidget(label);
        ui->verticalLayout_2->addWidget(listView);

        label->setVisible(false);
        listView->setVisible(false);

        m_listviewList->append(listView);
        m_labelArray[i] = label;

        if(m_flag[i])
        {
            ui->comboBox->addItem(m_itemNames[i]);
            m_map.insert(ui->comboBox->count()-1, m_itemNames[i]);
            label->setVisible(true);
            listView->setVisible(true);
        }
    }
//*/

    m_verticalSpacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_2->addItem(m_verticalSpacerItem);

    ui->comboBox->setCurrentIndex(0);

    ui->comboBox->setFixedHeight(ui->lineEdit->height()-2);
    ui->comboBox->setMinimumWidth(50);

    ui->scrollArea->setStyleSheet("/*background-color:rgb(51,51,51);*/border:none;");
    ui->scrollArea->setWidgetResizable(true);

    LOG_DEBUG() << "end";
}

RecentDock::~RecentDock()
{
    delete m_verticalSpacerItem;
    m_verticalSpacerItem = nullptr;

    qDeleteAll(*m_listviewList);
    m_listviewList->clear();
    delete m_listviewList;
    m_listviewList = nullptr;

    qDeleteAll(*m_modelList);
    m_modelList->clear();
    delete m_modelList;
    m_modelList = nullptr;

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
    if(s.endsWith(".mmp"))
        return;

    FILE_HANDLE fileHandle = m_mainWindow->openFile(s);
    if(fileHandle)
    {
        m_recent.prepend(s);
//        /*
        int index = 0;
        if(s.endsWith(".avi") || s.endsWith(".mov") || s.endsWith(".mp4"))   // 视频
        {
            m_modelList->at(0)->insert(fileHandle, 0);
            m_flag[0] = true;
            index = 0;
        }
        else if(s.endsWith(".mp3"))  // 音频
        {
            m_modelList->at(1)->insert(fileHandle, 0);
            m_flag[1] = true;
            index = 1;
        }
        else/* if(s.endsWith(".png"))*/  // 图片
        {
            m_modelList->at(2)->insert(fileHandle, 0);
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
//               m_labelArray[i]->setText(m_itemNames[i]);
           }
           if(index==i)
           {
               ui->comboBox->setCurrentText(m_itemNames[i]);
           }
        }

        resizeEvent(nullptr);
//        */
    }
    while (m_recent.count() > MaxItems)
    {
        m_recent.removeLast();
//        m_model->remove(m_model->rowCount()-1);
    }
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
    resizeEvent(nullptr);
}

//void RecentDock::on_comboBox_currentIndexChanged(int index)
//{
//    if(ui->verticalLayout_2->itemAt(index*2))
//    {
//        QWidget *widget = ui->verticalLayout_2->itemAt(index*2)->widget();
//        if(widget)
//        {
//            ui->scrollArea->verticalScrollBar()->setValue(widget->y());
//        }
//    }
//}

void RecentDock::on_comboBox_currentTextChanged(const QString &arg1)
{
    for(int i=m_map.key(arg1)*2; i<ui->verticalLayout_2->count()-1; i+=2)
    {
        if(ui->verticalLayout_2->itemAt(i))
        {
            QLabel *label = qobject_cast<QLabel*>(ui->verticalLayout_2->itemAt(i)->widget());
            QListView *listView = qobject_cast<QListView*>(ui->verticalLayout_2->itemAt(i+1)->widget());
            if(label && listView && QString::compare(label->text(),arg1)==0)
            {
                ui->scrollArea->verticalScrollBar()->setValue(label->y());
                listView->setFocus();
                return;
            }
        }
    }
}

void RecentDock::on_listView_activated(const QModelIndex &index)
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_currentListView->model());
    RecentListModel *model = qobject_cast<RecentListModel*>(proxyModel->sourceModel());
    FILE_HANDLE fileHandle = model->fileAt(proxyModel->mapToSource(index).row());
    m_mainWindow->playFile(fileHandle);
}

void RecentDock::on_listView_clicked(const QModelIndex &index)
{
    for(RecentListView *listView : *m_listviewList)
    {
        if(index.isValid() && index==listView->currentIndex())
        {
            m_currentIndex = index;
            m_currentListView = listView;
            return;
        }
    }
}

void RecentDock::on_listView_doubleClicked(const QModelIndex &index)
{
    for(RecentListView *listView : *m_listviewList)
    {
        if(index.isValid() && index==listView->currentIndex())
        {
            m_currentIndex = index;
            m_currentListView = listView;
            QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(listView->model());
            RecentListModel *model = qobject_cast<RecentListModel*>(proxyModel->sourceModel());
            FILE_HANDLE fileHandle = model->fileAt(proxyModel->mapToSource(m_currentIndex).row());
            m_mainWindow->playFile(fileHandle);
            return;
        }
    }
}

void RecentDock::on_listView_customContextMenuRequested(const QPoint &pos)
{
    for(RecentListView *listView : *m_listviewList)
    {
        QModelIndex index = listView->currentIndex();
        if(index.isValid())
        {
            m_currentIndex = index;
            m_currentListView = listView;

            QMenu menu(this);
            menu.addAction(ui->actionRemove);
            //menu.addAction(ui->actionRemoveAll);
            menu.addAction(ui->actionPlay);
//            menu.exec(mapToGlobal(pos));
            menu.exec(listView->mapToGlobal(pos));

            listView->setCurrentIndex(index);

            return;
        }
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
                    QLabel *label = qobject_cast<QLabel*>(ui->verticalLayout_2->itemAt(i*2)->widget());
                    label->setVisible(false);
                    m_currentListView->setVisible(false);

                    int key = m_map.key(label->text());
                    ui->comboBox->removeItem(key);
                    m_map.remove(key);

//                    for(int j=0; j<ui->comboBox->count(); j++)
//                    {
//                        if(ui->comboBox->itemText(j)==label->text())
//                        {
//                            ui->comboBox->removeItem(j);
//                            break;
//                        }
//                    }
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
        ui->comboBox->removeItem(i);
        ui->verticalLayout_2->itemAt(i*2)->widget()->setVisible(false);
        ui->verticalLayout_2->itemAt(i*2+1)->widget()->setVisible(false);
    }

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
    QList<FILE_HANDLE> seleted;
    QModelIndexList seletedIndexes = m_currentListView->getSelected();

    foreach(QModelIndex index, seletedIndexes)
    {
        QModelIndex sourceIndex = qobject_cast<QSortFilterProxyModel*>(m_currentListView->model())->mapToSource(index);
        FILE_HANDLE fileHandle = qobject_cast<RecentListModel*>(qobject_cast<QSortFilterProxyModel*>(m_currentListView->model())->sourceModel())->fileAt(sourceIndex.row());

        seleted.append(fileHandle);
    }
    return seleted;
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

