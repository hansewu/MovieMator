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

#include "templatedock.h"
#include "ui_templatedock.h"
#include "settings.h"
#include "util.h"

#include <QMenu>
#include <QLabel>
#include <Logger.h>
#include <QScrollBar>

TemplateDock::TemplateDock(MainInterface *main, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TemplateDock),
    m_mainWindow(main)
{
    LOG_DEBUG() << "begin";

    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());

    m_listviewList = new QList<TemplateListView*>();
    m_currentListView = nullptr;

    // 模板文件路径
    QDir dir("C:/Projects/MovieMator/templates");

    if(!dir.exists())
    {
        return;
    }

    QFileInfoList folderList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(int i=0; i<folderList.count(); i++)
    {   // 文件夹下没有文件夹只有文件
        ui->comboBox->addItem(folderList.at(i).fileName());

        QFileInfoList childFileList = QDir(folderList.at(i).absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoSymLinks);
        createFileListView(childFileList, this);
    }

    // 不在文件夹内的文件放到最后    
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    if(fileList.count()>0)
    {
        ui->comboBox->addItem("Files");

        createFileListView(fileList, this);
    }

    ui->comboBox->setCurrentIndex(0);

    m_spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_2->addItem(m_spacerItem);

    ui->scrollArea->setStyleSheet("/*background-color:rgb(51,51,51);*/border:none;");
    ui->scrollArea->setWidgetResizable(true);

    LOG_DEBUG() << "end";
}

TemplateDock::~TemplateDock()
{
    qDeleteAll(*m_listviewList);
    m_listviewList->clear();
    delete m_listviewList;
    m_listviewList = nullptr;

    delete m_spacerItem;
    m_spacerItem = nullptr;

    delete ui;
}

void TemplateDock::createFileListView(QFileInfoList &fileList, QObject *parent)
{
    TemplateListModel *model = new TemplateListModel(m_mainWindow, parent);
    for(int i=0; i<fileList.count(); i++)
    {
       if(fileList.at(i).fileName().contains(".mlt"))
       {
            model->append(m_mainWindow->openFile(fileList.at(i).filePath()));
       }
    }

    TemplateListView *listView = new TemplateListView();
    listView->setModel(model);
    listView->setFocusPolicy(Qt::StrongFocus);
    listView->setViewMode(QListView::IconMode);
    listView->setGridSize(QSize(90, 90));
    listView->setResizeMode(QListView::Adjust);
    listView->setWordWrap(true);
    listView->setWrapping(true);
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    listView->setStyleSheet("QListView{selection-background-color:rgb(192,72,44); selection-color: rgb(255,255,255);background-color:rgb(51,51,51);color:rgb(214,214,214);}");
    listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_listviewList->append(listView);

    QLabel *label = new QLabel(ui->comboBox->itemText(ui->comboBox->count()-1));
    ui->verticalLayout_2->addWidget(label);
    ui->verticalLayout_2->addWidget(listView);

    connect(listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(on_listView_clicked(const QModelIndex&)));
    connect(listView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(on_listView_doubleClicked(const QModelIndex&)));
    connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_listView_customContextMenuRequested(const QPoint&)));
}

void TemplateDock::resizeEvent(QResizeEvent* event)
{
    for(int i=0; i<m_listviewList->count(); i++)
    {
        TemplateListView *listView = m_listviewList->at(i);
        if(listView->model()->rowCount()<1)
        {
//            label->setVisible(false);
            listView->setVisible(false);
            continue;
        }
//        label->setVisible(true);
        listView->setVisible(true);
        int wSize = listView->gridSize().width();
        int hSize = listView->gridSize().height();
        int width = listView->size().width();
        int columns = width/wSize<1 ? 1 : width/wSize;
        int rowCount = listView->model()->rowCount();
        int row = rowCount%columns>0 ? (rowCount/columns+1) : (rowCount/columns);
        listView->setFixedHeight(row*hSize);
    }

    on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());

    QDockWidget::resizeEvent(event);
}

void TemplateDock::on_listView_customContextMenuRequested(const QPoint &pos)
{
    for(TemplateListView *listView : *m_listviewList)
    {
        QModelIndex index = listView->currentIndex();
        if(index.isValid())
        {
            m_currentIndex = index;
            m_currentListView = listView;

            QMenu menu(this);
            menu.addAction(ui->actionAddToTimeline);
            menu.exec(pos);

            listView->setCurrentIndex(index);

            return;
        }
    }
}

void TemplateDock::on_actionAddToTimeline_triggered()
{
    QModelIndex index = m_currentIndex;
    if(index.isValid() && index.row()<m_currentListView->model()->rowCount())
    {
        FILE_HANDLE fileHandle = qobject_cast<TemplateListModel*>(m_currentListView->model())->getTemplateFile(index.row());
        m_mainWindow->addToTimeLine(fileHandle);
    }
}

void TemplateDock::on_listView_clicked(const QModelIndex &index)
{
    for(TemplateListView *listView : *m_listviewList)
    {
        if(index.isValid() && index==listView->currentIndex())
        {
            m_currentIndex = index;
            m_currentListView = listView;
//            TemplateListModel *model = qobject_cast<TemplateListModel*>(listView->model());
//            FILE_HANDLE fileHandle = model->getTemplateFile(index.row());
//            m_mainWindow->playFile(fileHandle);
            return;
        }
    }
}

void TemplateDock::on_listView_doubleClicked(const QModelIndex &index)
{
    for(TemplateListView *listView : *m_listviewList)
    {
        if(index.isValid() && index==listView->currentIndex())
        {
            m_currentIndex = index;
            m_currentListView = listView;
            TemplateListModel *model = qobject_cast<TemplateListModel*>(listView->model());
            FILE_HANDLE fileHandle = model->getTemplateFile(index.row());
            m_mainWindow->playFile(fileHandle);
            return;
        }
    }
}

void TemplateDock::on_comboBox_currentIndexChanged(int index)
{
    // 下拉列表框跳转到指定项位置
    if(ui->verticalLayout_2->itemAt(index*2))
    {
        QWidget *widget = ui->verticalLayout_2->itemAt(index*2)->widget();
        if(widget)
        {
            ui->scrollArea->verticalScrollBar()->setValue(widget->y());
        }
    }
}

static TemplateDock *instance = nullptr;
//初始化模块
//参数，main 主程序接口对象
//返回界面对象
QDockWidget *TemplateDock_initModule(MainInterface *main)
{
    if (instance == nullptr)
        instance = new TemplateDock(main);
    return instance;
}

//销毁模块
void TemplateDock_destroyModule()
{

}
