/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: WanYuanCN <ebthon@hotmail.com>
 * Author: Dragon-S <15919917852@163.com>
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
#include <QImage>

EffectDock::EffectDock(MainInterface *main, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::EffectDock),
    m_mainWindow(main)
{
    LOG_DEBUG() << "begin";

    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());

    QString templateDir = Util::resourcesPath() + "/template/";

    m_effectFile = nullptr;
    m_effectList = new QList<QString>;
    m_mimeData = new QMimeData;

    m_imageList = new QList<EffectListView*>;
    m_currentListView = nullptr;

    readJsonFile(templateDir + "animation_name_translation_info.json", m_animationNameTranslateInfo);
    readJsonFile(templateDir + "imageclass_name_translation_info.json", m_imageClassNameTranslateInfo);

    // 特效文件列表
    QDir effectDir(templateDir+"Effects");
    QFileInfoList effectFiles = effectDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    if(effectFiles.count()>0)
    {
        for(int i=0; i<effectFiles.count(); i++)
        {
            m_effectList->append(effectFiles.at(i).filePath());
            QString itemName = getTranslationStr(effectFiles[i].fileName().split(".")[0], m_animationNameTranslateInfo);
            ui->comboBox->addItem(itemName);
        }

        if(effectFiles[0].fileName().split(".")[0]=="blur" && effectFiles.count()>1)
        {   // 模糊效果blur不作为默认效果
            m_effectList->removeFirst();
            m_effectList->insert(1, effectFiles[0].filePath());
            ui->comboBox->removeItem(0);
            ui->comboBox->insertItem(1, getTranslationStr(effectFiles[0].fileName().split(".")[0], m_animationNameTranslateInfo));
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
            QString itemName = getTranslationStr(folder.fileName(), m_imageClassNameTranslateInfo);
            ui->comboBox_2->addItem(itemName);
            createImageFileList(fileList, itemName);
        }
    }

    // 实际上文件夹多了就用不上了
    QSpacerItem *spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_2->addItem(spacerItem);

    ui->comboBox->setMinimumContentsLength(10);
    ui->comboBox->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    ui->comboBox->setFixedHeight(ui->comboBox_2->height());
    ui->comboBox->setStyleSheet("QComboBox { background-color:rgb(100,100,100);color:rgb(225,225,225); }");
    ui->comboBox_2->setStyleSheet("QComboBox { background-color:rgb(100,100,100);color:rgb(225,225,225); }");

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setStyleSheet("border:none;");
    ui->scrollArea->verticalScrollBar()
            ->setStyleSheet("QScrollBar:vertical{width:8px;background:rgba(0,0,0,0%);margin:0px,0px,0px,0px;}"
                            "QScrollBar::handle:vertical{width:8px;background:rgba(160,160,160,25%);border-radius:4px;min-height:20;}"
                            "QScrollBar::handle:vertical:hover{width:8px;background:rgba(160,160,160,50%);border-radius:4px;min-height:20;}");

    LOG_DEBUG() << "end";
}

EffectDock::~EffectDock()
{
//    qDeleteAll(*m_effectList);
//    m_effectList->clear();
//    delete m_effectList;
//    m_effectList = nullptr;

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
    Q_ASSERT(m_imageList);
    if(!m_imageList)
    {
        return;
    }
    for(EffectListView *listView : *m_imageList)
    {
        Q_ASSERT(listView);
        if(!listView)
        {
            continue;
        }
        listView->setFixedWidth(ui->scrollArea->width() -5);
        int wSize = listView->gridSize().width();
        if(wSize<=0)
        {
            continue;
        }
        int hSize = listView->gridSize().height();
        int width = listView->size().width();
        int columns = width/wSize<1 ? 1: width/wSize;
        int rowCount = listView->model()->rowCount();
        int rows = rowCount%columns>0 ? (rowCount/columns+1) : (rowCount/columns);
        listView->setFixedHeight(rows*hSize);
    }
    on_comboBox_2_activated(ui->comboBox_2->currentIndex());
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
            result = subObj.value("en").toString();;
        }
    }

    return result;
}

void EffectDock::resetImage(QString effectFile, QString imageFile)
{
    // 替换 XML内容
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
    bool flagResource = false;
    bool flagHash = false;
    for(int i=0; i<nodeList.count(); i++)
    {
        QDomElement domElement = nodeList.at(i).toElement();

        //添加模板标记
        QDomElement templateDom = doc.createElement("property");
        templateDom.setAttribute("name","moviemator:template");
        QDomText templatePropertyValue = doc.createTextNode("template");
        templateDom.appendChild(templatePropertyValue);
        domElement.appendChild(templateDom);

        //添加图片宽
        QDomElement imageW = doc.createElement("property");
        imageW.setAttribute("name","moviemator:imageW");
        QPixmap pixmap = QPixmap(imageFile);
        int width = pixmap.width();
        QDomText imageWDom = doc.createTextNode(QString::number(width));
        imageW.appendChild(imageWDom);
        domElement.appendChild(imageW);

        //添加图片宽
        QDomElement imageH = doc.createElement("property");
        imageH.setAttribute("name","moviemator:imageH");
        int height = pixmap.height();
        QDomText imageHDom = doc.createTextNode(QString::number(height));
        imageH.appendChild(imageHDom);
        domElement.appendChild(imageH);

        QDomNode domNodeResource;
        QDomNode domNodeHash;
        QDomNode domNodeRect;
        QDomNode domNodedistort;

        // 设置size滤镜参数
        if(imageFile.contains("frame")){
            QDomNodeList filterList = domElement.elementsByTagName("filter");
            for(int k=0; k<filterList.count(); k++)
            {
                QDomElement filter = filterList.at(k).toElement();
                if(filter.text().contains("affineSizePosition")){
                    QDomNodeList propertyList = filter.elementsByTagName("property");
                    for(int m=0; m<propertyList.count(); m++)
                    {
                        QDomElement prop = propertyList.at(m).toElement();
                        if(prop.attribute("name").contains("transition.distort")){
                            domNodedistort = prop.toElement().firstChild();
                            domNodedistort.setNodeValue("1");
                        }
                        if(prop.attribute("name").contains("transition.rect_anim_relative")){
                            domNodeRect = prop.toElement().firstChild();
                            domNodeRect.setNodeValue("0.0 0.0 1.0 1.0 1");
                        }
                    }
                    break;
                }
            }
        }

        // 设置图片路径
        QDomNodeList elementList = domElement.elementsByTagName("property");
        for(int j=0; j<elementList.count(); j++)
        {
            QDomElement de = elementList.at(j).toElement();
            if(de.attribute("name").contains("resource"))
            {
                domNodeResource = de.toElement().firstChild();
                flagResource = true;
                if(flagHash)
                {
                    break;
                }
                continue;
            }
            if(de.attribute("name").contains("moviemator:hash"))
            {
                domNodeHash = de.toElement().firstChild();
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
            domNodeResource.setNodeValue(imageFile);
            domNodeHash.setNodeValue(Util::getFileHash(imageFile));
            Q_ASSERT(m_mainWindow);
            if(!m_mainWindow)
            {
                return;
            }
            m_effectFile = m_mainWindow->createFileWithXMLForDragAndDrop(doc.toString());
            return;
        }
    }
}

void EffectDock::setMimeDataForDrag()
{
    Q_ASSERT(m_effectFile);
    Q_ASSERT(m_mainWindow);
    Q_ASSERT(m_mimeData);
    if(!m_effectFile || !m_mainWindow || !m_mimeData)
    {
        return;
    }
    m_mimeData->setData(m_mainWindow->getXMLMimeTypeForDragDrop(), m_mainWindow->getXmlForDragDrop(m_effectFile).toUtf8());
    m_mimeData->setText(QString::number(m_mainWindow->getPlayTime(m_effectFile)));
}

void EffectDock::createEffectFile()
{
    Q_ASSERT(m_effectList);
    if(!m_effectList)
    {
        return;
    }
    QString effectFile;
    QString imageFile;
    int comboIndex = ui->comboBox->currentIndex();
    if(comboIndex>=0 && comboIndex<m_effectList->count())
    {
//        m_effectFile = m_effectList->at(comboIndex);
        effectFile = m_effectList->at(comboIndex);
    }
    if(m_currentListView && m_currentIndex.isValid() && m_mainWindow)
    {
        m_effectFile = qobject_cast<EffectListModel*>(m_currentListView->model())->fileAt(m_currentIndex.row());
        imageFile = m_mainWindow->getFileName(m_effectFile);
    }
    if(!effectFile.isEmpty() && !imageFile.isEmpty())
    {
        resetImage(effectFile, imageFile);
    }
    if(m_effectFile && m_currentListView)
    {
        setMimeDataForDrag();
        m_currentListView->setMimeData(m_mimeData, m_mainWindow->getXMLMimeTypeForDragDrop());
    }
}

void EffectDock::createImageFileList(QFileInfoList &fileList, QString folderName)
{
    Q_ASSERT(m_mainWindow);
    if(!m_mainWindow)
    {
        return;
    }
    EffectListModel *model = new EffectListModel(m_mainWindow, this);
    for(int i=0; i<fileList.count(); i++)
    {
        model->append(m_mainWindow->openFile(fileList[i].filePath()));
    }

    appendListViewAndLabel(model, folderName);
}

void EffectDock::appendListViewAndLabel(EffectListModel *model, QString itemName)
{   Q_ASSERT(model);
    Q_ASSERT(m_imageList);
    if(!model || !m_imageList)
    {
        return;
    }
    EffectListView *listView = new EffectListView();
    QLabel *label = new QLabel(itemName, this);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel *image = new QLabel(this);
    image->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    image->setScaledContents(true);     // 可以让图片随 label拉伸
    image->setPixmap(QPixmap(":/icons/light/32x32/line.png"));

    QHBoxLayout *box = new QHBoxLayout();
    box->addWidget(label);
    box->addWidget(image);

    listView->setModel(model);

    m_imageList->append(listView);
//    ui->verticalLayout_2->addWidget(label);
    ui->verticalLayout_2->addLayout(box);
    ui->verticalLayout_2->addWidget(listView);

    label->setFixedHeight(40);

    listView->setFont(QFont(font().family(), 8));   // 改变字体大小
    listView->setFocusPolicy(Qt::ClickFocus);
    listView->setViewMode(QListView::IconMode);
    listView->setGridSize(QSize(95, 90));         // 120,100    ,300/3-5
    listView->setUniformItemSizes(true);
    listView->setResizeMode(QListView::Adjust);
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    listView->setContentsMargins(0, 5, 0, 5);
    listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listView->setStyleSheet(
                "QListView::item:selected{background-color:rgb(192,72,44); color:rgb(255,255,255);}"
                "QListView{background-color:transparent;color:rgb(214,214,214);}");

    connect(listView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(onListviewPressed(const QModelIndex&)));
    connect(listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onListviewClicked(const QModelIndex&)));
    connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onListviewCustomContextMenuRequested(const QPoint&)));
}

void EffectDock::onListviewPressed(const QModelIndex &)
{
    Q_ASSERT(m_imageList);
    if(!m_imageList)
    {
        return;
    }
    for(EffectListView *listView : *m_imageList)
    {
        Q_ASSERT(listView);
        if(!listView)
        {
            continue;
        }
        if(listView->hasFocus())
        {
            if(m_currentListView && m_currentListView!=listView)
            {
                m_currentListView->clearSelection();
            }
            m_currentListView = listView;
            m_currentIndex = m_currentListView->currentIndex();
            createEffectFile();
            return;
        }
    }
}

void EffectDock::onListviewClicked(const QModelIndex &)
{
//    Q_ASSERT(m_effectFile);   // 不能加，默认下可以为空，也就是不播放文件
    Q_ASSERT(m_mainWindow);
    if(m_effectFile && m_mainWindow)
    {
        m_mainWindow->playFile(m_effectFile);
    }
}

void EffectDock::onListviewCustomContextMenuRequested(const QPoint &pos)
{
    Q_ASSERT(m_currentIndex.isValid());
    Q_ASSERT(m_currentListView);
    if(m_currentIndex.isValid() && m_currentListView && m_currentListView->indexAt(pos)==m_currentIndex)
    {
        QMenu menu(this);
        menu.addAction(ui->actionAddToTimeline);
        menu.exec(m_currentListView->mapToGlobal(pos));
    }
}

void EffectDock::on_actionAddToTimeline_triggered()
{
    Q_ASSERT(m_effectFile);
    Q_ASSERT(m_mainWindow);
    if(m_effectFile && m_mainWindow)
    {
        m_mainWindow->addToTimeLine(m_effectFile);
    }
}

void EffectDock::on_comboBox_activated(int index)
{
    Q_ASSERT(index>=0);
    Q_ASSERT(index<ui->comboBox->count());
    Q_ASSERT(m_mainWindow);
//    Q_ASSERT(m_effectFile);     // 不能加，默认下可以为空，不播放文件
    if(index>=0 && index<ui->comboBox->count() && m_effectFile && m_mainWindow)
    {
        m_mainWindow->playFile(m_effectFile);
    }
}

void EffectDock::on_comboBox_currentIndexChanged(int index)
{
    Q_ASSERT(index>=0);
    Q_ASSERT(index<ui->comboBox->count());
    if(index>=0 && index<ui->comboBox->count())
    {
        createEffectFile();
    }
}

void EffectDock::on_comboBox_2_activated(int index)
{
    QLayoutItem *item = ui->verticalLayout_2->itemAt(index*2);
    if(item)
    {
        QWidget *widget = item->layout()->itemAt(0)->widget();
        if(widget)
        {
            ui->scrollArea->verticalScrollBar()->setValue(widget->y());
        }
    }
}

void EffectDock::on_EffectDock_visibilityChanged(bool visible)
{
    if (visible) {
        onListviewClicked(QModelIndex());

        resizeEvent(nullptr);   // 切换dock后listView大小会随dock变化
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

