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
#include <Logger.h>
#include <QScrollBar>
#include <QDomDocument>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QDebug>
#include <QJsonArray>

EffectDock::EffectDock(MainInterface *main, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::EffectDock),
    m_mainWindow(main)
{
    LOG_DEBUG() << "begin";

    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());

    QString templateDir = Util::resourcesPath() + "/template/";
    m_dir = QDir(templateDir);

    m_effectFile = nullptr;
    m_effectList = new QList<FILE_HANDLE>;
    m_mimeData = new QMimeData;

    m_imageList = new QList<EffectListView*>;
    m_currentListView = nullptr;

    // 特效文件列表
    QDir effectDir(templateDir+"Effects");
    QFileInfoList effectFiles = effectDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    if(effectFiles.count()>0)
    {
        for(int i=0; i<effectFiles.count(); i++)
        {
            m_effectList->append(m_mainWindow->openFile(effectFiles.at(i).filePath()));
            ui->comboBox->addItem(effectFiles[i].fileName().split(".")[0]);
        }
    }
    // 图片文件列表
    QDir imageDir(templateDir+"Images");
    QFileInfoList folderList = imageDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(int i=0; i<folderList.count(); i++)
    {
        QFileInfo folder = folderList.at(i);
        QFileInfoList fileList = QDir(folder.absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoSymLinks);

        if(fileList.count()>0)
        {
            ui->comboBox_2->addItem(folder.fileName());
            createImageFileList(fileList, folder.fileName());
        }
    }

    // 实际上文件夹多了就用不上了
    m_spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_2->addItem(m_spacerItem);

    ui->comboBox->setStyleSheet("QComboBox { background-color:rgb(100,100,100);color:rgb(225,225,225); }");
    ui->comboBox_2->setStyleSheet("QComboBox { background-color:rgb(100,100,100);color:rgb(225,225,225); }");

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setStyleSheet("border:none;background:transparent;");       // 51,51,51
    ui->scrollArea->verticalScrollBar()
            ->setStyleSheet("QScrollBar:vertical{width:8px;background:rgba(0,0,0,0%);margin:0px,0px,0px,0px;}"
                            "QScrollBar::handle:vertical{width:8px;background:rgba(160,160,160,25%);border-radius:4px;min-height:20;}"
                            "QScrollBar::handle:vertical:hover{width:8px;background:rgba(160,160,160,50%);border-radius:4px;min-height:20;}");

    readJsonFile(templateDir + "animation_name_translation_info.json", m_animationNameTranslateInfo);
    readJsonFile(templateDir + "imageclass_name_translation_info.json", m_animationNameTranslateInfo);

    LOG_DEBUG() << "end";
}

EffectDock::~EffectDock()
{
    qDeleteAll(*m_effectList);
    m_effectList->clear();
    delete m_effectList;
    m_effectList = nullptr;

    delete m_mimeData;
    m_mimeData = nullptr;

    qDeleteAll(*m_imageList);
    m_imageList->clear();
    delete m_imageList;
    m_imageList = nullptr;

//    delete m_spacerItem;
//    m_spacerItem = nullptr;

    delete ui;
}

void EffectDock::resizeEvent(QResizeEvent *event)
{
    for(int i=0; i<m_imageList->count(); i++)
    {
        EffectListView *listView = m_imageList->at(i);
        int wSize = listView->gridSize().width();
        int hSize = listView->gridSize().height();
        int width = listView->size().width();
        int columns = width/wSize<1 ? 1: width/wSize;
        int rowCount = listView->model()->rowCount();
        int rows = rowCount%columns>0 ? (rowCount/columns+1) : (rowCount/columns);
        listView->setFixedHeight(rows*hSize);
    }

    on_comboBox_2_currentIndexChanged(ui->comboBox_2->currentIndex());

    QDockWidget::resizeEvent(event);
}

void EffectDock::readJsonFile(QString filePath, QJsonObject &jsonObj) {
    QFile jsonFile(filePath);
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        qDebug()<<"sll-----could't open"<<filePath;
        return;
    }

    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData, &json_error));
    if (json_error.error != QJsonParseError::NoError) {
        qDebug()<<"sll-----json error!";
        return;
    }

    jsonObj = jsonDoc.object();
}

QString EffectDock::getTranslationStr(QString srcStr, QJsonObject translationInfo) {
    if (translationInfo.isEmpty()) {
        return srcStr;
    }

    QString result = srcStr;
    if (translationInfo.contains(srcStr)) {
        QJsonObject subObj = translationInfo.value(srcStr).toObject();
        QString language = QLocale::system().name();
        result = subObj.value(language).toString();
        if (result.isEmpty()) {
            result = srcStr;
        }
    }

    return result;
}

void EffectDock::replaceImage(QString effectFile, QString imageFile)
{   // 替换 XML内容
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

void EffectDock::setMimeDataForDrag()
{
    m_mimeData->setData(m_mainWindow->getXMLMimeTypeForDragDrop(), m_mainWindow->getXmlForDragDrop(m_effectFile).toUtf8());
    m_mimeData->setText(QString::number(m_mainWindow->getPlayTime(m_effectFile)));
}

void EffectDock::createEffectFile()
{
    QString effectFile;
    QString imageFile;
    int comboIndex = ui->comboBox->currentIndex();
    if(comboIndex>=0 && comboIndex<m_effectList->count())
    {
        m_effectFile = m_effectList->at(comboIndex);
        effectFile = m_mainWindow->getFileName(m_effectFile);
    }
    if(m_currentIndex.isValid())
    {
        m_effectFile = qobject_cast<EffectListModel*>(m_currentListView->model())->fileAt(m_currentIndex.row());
        imageFile = m_mainWindow->getFileName(m_effectFile);
    }
    if(!effectFile.isEmpty() && !imageFile.isEmpty())
    {
        replaceImage(effectFile, imageFile);
    }
    if(m_effectFile && m_currentListView)
    {
        setMimeDataForDrag();
        m_currentListView->setMimeData(m_mimeData, m_mainWindow->getXMLMimeTypeForDragDrop());
    }
}

void EffectDock::createImageFileList(QFileInfoList &fileList, QString folderName)
{
    EffectListModel *model = new EffectListModel(m_mainWindow, this);
    for(int i=0; i<fileList.count(); i++)
    {
        model->append(m_mainWindow->openFile(fileList[i].filePath()));
    }

    appendListViewAndLabel(model, folderName);
}

void EffectDock::appendListViewAndLabel(EffectListModel *model, QString itemName)
{
    EffectListView *listView = new EffectListView();
    QLabel *label = new QLabel(itemName, this);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel *image = new QLabel(this);
    image->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    image->setScaledContents(true);     // 可以让图片随 label拉伸
    image->setPixmap(QPixmap(":/icons/light/32x32/line.png"));

    QHBoxLayout *box = new QHBoxLayout(this);
    box->addWidget(label);
    box->addWidget(image);

    listView->setModel(model);

    m_imageList->append(listView);
//    ui->verticalLayout_2->addWidget(label);
    ui->verticalLayout_2->addLayout(box);
    ui->verticalLayout_2->addWidget(listView);

    label->setFixedHeight(40);

    listView->setFocusPolicy(Qt::ClickFocus);
    listView->setViewMode(QListView::IconMode);
    listView->setGridSize(QSize(120, 100));
    listView->setUniformItemSizes(true);
    listView->setResizeMode(QListView::Adjust);
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    listView->setContentsMargins(5, 5, 5, 5);
    listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listView->setStyleSheet("QListView{selection-background-color:rgb(192,72,44); selection-color: rgb(255,255,255);background-color:transparent;color:rgb(214,214,214);}");
    listView->setStyleSheet("QListView::item:selected{background:rgb(192,72,44); color:rgb(255,255,255);}");

    connect(listView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(on_listView_pressed(const QModelIndex&)));
    connect(listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(on_listView_clicked(const QModelIndex&)));
//    connect(listView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(on_listView_doubleClicked(const QModelIndex&)));
    connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_listView_customContextMenuRequested(const QPoint&)));
}

void EffectDock::on_listView_pressed(const QModelIndex &)
{
    for(int i=0; i<m_imageList->count(); i++)
    {
        if(m_imageList->at(i)->hasFocus()){
            m_currentListView = m_imageList->at(i);
            m_currentIndex = m_currentListView->currentIndex();
            createEffectFile();
            return;
        }
    }
}

void EffectDock::on_listView_clicked(const QModelIndex &)
{
    if(m_effectFile)
    {
        m_mainWindow->playFile(m_effectFile);
    }
}

//void EffectDock::on_listView_doubleClicked(const QModelIndex &)
//{
////    createEffectFile();
//    if(m_effectFile)
//    {
//        m_mainWindow->playFile(m_effectFile);
//    }
//}

void EffectDock::on_listView_customContextMenuRequested(const QPoint &pos)
{
    if(m_currentIndex.isValid() && m_currentListView->indexAt(pos).isValid())
    {
        QMenu menu(this);
        menu.addAction(ui->actionAddToTimeline);
        menu.exec(m_currentListView->mapToGlobal(pos));
    }
}

void EffectDock::on_actionAddToTimeline_triggered()
{
//    createEffectFile();
    if(m_effectFile)
    {
        m_mainWindow->addToTimeLine(m_effectFile);
    }
}

void EffectDock::on_comboBox_activated(int index)
{
    if(index>=0 && index<ui->comboBox->count() && m_effectFile)
    {
        m_mainWindow->playFile(m_effectFile);
    }
}

void EffectDock::on_comboBox_currentIndexChanged(int index)
{
    if(index>=0 && index<ui->comboBox->count())
    {
        createEffectFile();
    }
}

void EffectDock::on_comboBox_2_currentIndexChanged(int index)
{
    QLayoutItem *item = ui->verticalLayout_2->itemAt(index*2);
    if(item)
    {
        QWidget *widget = item->layout()->itemAt(0)->widget();  // item->widget();
        if(widget)
        {
            ui->scrollArea->verticalScrollBar()->setValue(widget->y());
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

