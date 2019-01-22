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

#include "effectdock.h"
#include "ui_effectdock.h"
#include "settings.h"
#include "util.h"

#include <QMenu>
#include <QLabel>
#include <Logger.h>
#include <QScrollBar>

//#include <QDropEvent>
#include <QMimeData>
//#include <QUrl>

#include <QSplitter>

#include <QDomDocument>

EffectDock::EffectDock(MainInterface *main, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::EffectDock),
    m_mainWindow(main)
{
    LOG_DEBUG() << "begin";

    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());

    m_dir = QDir(s_templateDir);
    m_effectFile = nullptr;
//    m_mimeData = new QMimeData;

    if(!m_dir.exists())
    {
        return;
    }

    QSplitter *splitter = new QSplitter();
    splitter->addWidget(ui->listView);
    splitter->addWidget(ui->listView_2);
    splitter->setOrientation(Qt::Vertical);
    ui->verticalLayout->addWidget(splitter);
    splitter->setStyleSheet("QSplitter::handle{background-color:rgba(160, 160, 160, 75%)}");
    splitter->setHandleWidth(3);

    EffectListModel *model = new EffectListModel(main, this);
    QDir dir1(s_templateDir+"/Effects");
    QFileInfoList files = dir1.entryInfoList(QDir::Files | QDir::NoSymLinks);
    if(files.count()>0)
    {
        for(int i=0; i<files.count(); i++)
            model->append(m_mainWindow->openFile(files.at(i).filePath()));
    }
    ui->listView->setModel(model);

    EffectListModel *model2 = new EffectListModel(main, this);
    QDir dir2(s_templateDir+"/Images");
    QFileInfoList files2 = dir2.entryInfoList(QDir::Files | QDir::NoSymLinks);
    if(files2.count()>0)
    {
        for(int i=0; i<files2.count(); i++)
        {
            model2->append(m_mainWindow->openFile(files2.at(i).filePath()));
        }
    }
    ui->listView_2->setModel(model2);

    formatListView(ui->listView);
    formatListView(ui->listView_2);

    LOG_DEBUG() << "end";
}

EffectDock::~EffectDock()
{
//    qDeleteAll(*m_listviewList);
//    m_listviewList->clear();
//    delete m_listviewList;
//    m_listviewList = nullptr;

//    delete m_spacerItem;
//    m_spacerItem = nullptr;

    delete ui;
}

FILE_HANDLE EffectDock::getEffectFile()
{
    return m_effectFile;
}

//void EffectDock::setMimeDataForDrag() const
//{
//    m_mimeData->setData(m_mainWindow->getXMLMimeTypeForDragDrop(), m_mainWindow->getXmlForDragDrop(m_effectFile).toUtf8());
//    m_mimeData->setText(QString::number(m_mainWindow->getPlayTime(m_effectFile)));
//}

void EffectDock::formatListView(QListView *listView)
{
    listView->setFocusPolicy(Qt::StrongFocus);
    listView->setViewMode(QListView::IconMode);
    listView->setGridSize(QSize(120, 100));
    listView->setResizeMode(QListView::Adjust);
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    listView->setContentsMargins(5, 5, 5, 5);

    listView->setStyleSheet("QListView{selection-background-color:rgb(192,72,44); selection-color: rgb(255,255,255);background-color:rgba(160,160,160,0%);color:rgb(214,214,214);}");
    listView->verticalScrollBar()
            ->setStyleSheet("QScrollBar:vertical{background:rgba(0,0,0,0%);margin:0px,0px,0px,0px;}"
                            "QScrollBar::handle:vertical{background:rgba(160,160,160,25%);border-radius:4px;min-height:20;}"
                            "QScrollBar::handle:vertical:hover{background:rgba(160,160,160,50%);border-radius:4px;min-height:20;}");

    connect(listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(on_listView_clicked(const QModelIndex&)));
    connect(listView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(on_listView_doubleClicked(const QModelIndex&)));
    connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_listView_customContextMenuRequested(const QPoint&)));
}

void EffectDock::on_listView_clicked(const QModelIndex &)
{
    setEffectFile();
}

void EffectDock::on_listView_doubleClicked(const QModelIndex &)
{
    setEffectFile();
    if(m_effectFile)
    {
        m_mainWindow->playFile(m_effectFile);
    }
}

void EffectDock::on_listView_customContextMenuRequested(const QPoint &pos)
{
    if(ui->listView->hasFocus())
    {
        QModelIndex index = ui->listView->currentIndex();
        if(index.isValid())
        {
            QMenu menu(this);
            menu.addAction(ui->actionAddToTimeline);
            menu.exec(ui->listView->mapToGlobal(pos));
        }
    }
    else if(ui->listView_2->hasFocus())
    {
        QModelIndex index = ui->listView_2->currentIndex();
        if(index.isValid())
        {
            QMenu menu(this);
            menu.addAction(ui->actionAddToTimeline);
            menu.exec(ui->listView_2->mapToGlobal(pos));
        }
    }
}

void EffectDock::on_actionAddToTimeline_triggered()
{
    setEffectFile();
    if(m_effectFile)
    {
        m_mainWindow->addToTimeLine(m_effectFile);
    }
}

void EffectDock::setEffectFile()
{   // 切换特效或图片时都会触发效果
    QModelIndex currentEffect = ui->listView->currentIndex();
    QModelIndex currentImage = ui->listView_2->currentIndex();
    if(!currentEffect.isValid() && !currentImage.isValid())
    {
        return;
    }
    else if(!currentEffect.isValid())
    {
        m_effectFile = qobject_cast<EffectListModel*>(ui->listView_2->model())->fileAt(currentImage.row());
//        ui->listView->setThumbnail(m_mainWindow->getThumbnail(m_effectFile));
//        ui->listView_2->setThumbnail(m_mainWindow->getThumbnail(m_effectFile));
    }
    else if(!currentImage.isValid())
    {
        m_effectFile = qobject_cast<EffectListModel*>(ui->listView->model())->fileAt(currentEffect.row());
//        ui->listView->setThumbnail(m_mainWindow->getThumbnail(m_effectFile));
//        ui->listView_2->setThumbnail(m_mainWindow->getThumbnail(m_effectFile));
    }
    else // if(currentEffect.isValid() && currentImage.isValid())
    {
        QString effectFile = m_mainWindow->getFileName(qobject_cast<EffectListModel*>(ui->listView->model())->fileAt(currentEffect.row()));
        QString imageFile = m_mainWindow->getFileName(qobject_cast<EffectListModel*>(ui->listView_2->model())->fileAt(currentImage.row()));

        replaceImage(effectFile, imageFile);
//        ui->listView->setThumbnail(m_mainWindow->getThumbnail(qobject_cast<EffectListModel*>(ui->listView_2->model())->fileAt(currentImage.row())));
//        ui->listView_2->setThumbnail(m_mainWindow->getThumbnail(qobject_cast<EffectListModel*>(ui->listView_2->model())->fileAt(currentImage.row())));
    }
//    if(m_effectFile)
//    {
//        ui->listView->setMimeData(m_mimeData);
//        ui->listView_2->setMimeData(m_mimeData);
//    }
}

void EffectDock::replaceImage(QString effectFile, QString imageFile)
{
    // 替换XML内容
    QFile file(effectFile);
    if(!file.exists())
    {
        return;
    }
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();
    QDomNodeList nodeList = doc.elementsByTagName("producer");
    QDomNode domNode;
    bool flagResource = false;
    bool flagHash = false;
    for(int i=0; i<nodeList.count(); i++)
    {
        QDomElement domElement = nodeList.at(i).toElement();
        QDomNodeList elementList = domElement.elementsByTagName("property");
        for(int j=0; j<elementList.count(); j++)
        {
            QDomElement de = elementList.at(j).toElement();
            if(de.attribute("name").contains("resource"))
            {
                domNode = de.toElement().firstChild();
                flagResource = true;
                if(flagHash)
                {
                    break;
                }
                continue;
            }
            if(de.attribute("name").contains("moviemator:hash"))
            {
                flagHash = true;
                if(flagResource)
                {
                    break;
                }
                continue;
            }
        }
        if(flagResource && flagHash)
        {
            domNode.setNodeValue(imageFile);
            m_effectFile = m_mainWindow->createFileWithXMLForDragAndDrop(doc.toString());
            return;
        }
    }
}

static EffectDock *instance = nullptr;
//初始化模块
//参数，main 主程序接口对象
//返回界面对象
QDockWidget *EffectDock_initModule(MainInterface *main)
{
    if (instance == nullptr)
        instance = new EffectDock(main);
    return instance;
}

//销毁模块
void EffectDock_destroyModule()
{

}

