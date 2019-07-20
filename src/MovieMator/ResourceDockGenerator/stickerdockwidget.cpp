#include "stickerdockwidget.h"
#include "util.h"
#include "ui_basedockwidget.h"
#include "translationhelper.h"
#include "resourcedockgenerator_global.h"
#include "stickeritemmodel.h"
#include "uiuserdef.h"
#include "translationhelper.h"

#include <qdir.h>
#include <qdebug.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <qdom.h>

StickerDockWidget::StickerDockWidget(MainInterface *pMainInterface, QWidget *pParent) :
    BaseDockWidget(pParent),
    m_mainInterface(pMainInterface)
{
    qDebug()<<"sll-----StickerDockWidget构造---start";

    QString stickerDir = Util::resourcesPath() + "/template/sticker";
    TranslationHelper::readJsonFile(stickerDir + "imageclass_property_info.json", m_imageClassPropertyInfo);

    qDebug()<<"sll-----StickerDockWidget构造---end";
}

void StickerDockWidget::setupOtherUi()
{
    qDebug()<<"sll-----setupOtherUi---start";

    QHBoxLayout *animationWidgetLayout = new QHBoxLayout();

    //动画标签
    QLabel *animationLabel = new QLabel(tr("Animation: "));
    animationWidgetLayout->addWidget(animationLabel);

    //动画combobox
    m_animationCombobox = new QComboBox();
    m_animationCombobox->setStyleSheet("QComboBox { background-color:rgb(100,100,100);color:rgb(225,225,225); }");
    animationWidgetLayout->addWidget(m_animationCombobox);

    ui->horizontalLayout_titlebar->insertLayout(1, animationWidgetLayout);

    setupAnimationComboboxData();

    connect(m_animationCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAnimationComboBoxCurrentIndexChanged(int)));
    connect(m_animationCombobox, SIGNAL(activated(int)), this, SLOT(onAnimationComboBoxActivated(int)));

    qDebug()<<"sll-----setupOtherUi---end";
}

void StickerDockWidget::resizeEvent(QResizeEvent *event) {
    qDebug()<<"sll-----resizeEvent---start";

    onAnimationComboBoxActivated(m_animationCombobox->currentIndex());
    BaseDockWidget::resizeEvent(event);

    qDebug()<<"sll-----resizeEvent---end";
}

void StickerDockWidget::setupAnimationComboboxData()
{
    qDebug()<<"sll-----setupAnimationComboboxData---start";

    QString stickerDir = Util::resourcesPath() + "/template/sticker";

    QJsonObject animationTranslateInfo;
    TranslationHelper::readJsonFile(stickerDir + "/animation_name_translation_info.json", animationTranslateInfo);

    QDir animationFilesDir(stickerDir + "/animation");
    QFileInfoList animationFilesInfoList = animationFilesDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    foreach (QFileInfo animationFileInfo, animationFilesInfoList)
    {
        QString itemName = TranslationHelper::getTranslationStr(animationFileInfo.baseName(), animationTranslateInfo);
        m_animationCombobox->addItem(itemName, QVariant(animationFileInfo.filePath()));
    }

    qDebug()<<"sll-----setupAnimationComboboxData---end";
}

QIcon StickerDockWidget::getListViewItemIcon(const QString filePath)
{
    QImage iconImage = QImage(LISTVIEW_ITEMICONSIZE_WIDTH,
                              LISTVIEW_ITEMICONSIZE_HEIGHT,
                              QImage::Format_ARGB32);

    QImage image = QImage(filePath);
    if (!image.isNull())
    {
        QPainter painter(&iconImage);
        iconImage.fill(QApplication::palette().base().color().rgb());

        QRect rect = image.rect();
        rect.setWidth(LISTVIEW_ITEMICONSIZE_WIDTH);
        rect.setHeight(LISTVIEW_ITEMICONSIZE_HEIGHT);

        painter.drawImage(rect, image);
        painter.end();
    }
    else
    {
        iconImage.fill(QApplication::palette().base().color().rgb());
    }

    return  QPixmap::fromImage(iconImage);
}

QMap<QString, BaseItemModel *> *StickerDockWidget::createAllClassesItemModel()
{
    qDebug()<<"sll-----createAllClassesItemModel---start";
    QMap<QString, BaseItemModel *> *stickerDockListViewItemModel = new QMap<QString, BaseItemModel *>;

    QString stickerDir = Util::resourcesPath() + "/template/sticker";

    QJsonObject stickerClassNameTranslateInfo;
    TranslationHelper::readJsonFile(stickerDir + "/imageclass_name_translation_info.json", stickerClassNameTranslateInfo);

    QDir stickerImageFileDir(stickerDir + "/image");
    QFileInfoList allClassFolderInfo = stickerImageFileDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo oneClassFolderInfo, allClassFolderInfo)
    {
        QFileInfoList oneClassFileList = QDir(oneClassFolderInfo.absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoSymLinks);

        QString className = TranslationHelper::getTranslationStr(oneClassFolderInfo.fileName(), stickerClassNameTranslateInfo);

        StickerItemModel *itemMode = new StickerItemModel(m_mainInterface, this);
        foreach (QFileInfo imageFileInfo, oneClassFileList)
        {
            QStandardItem *item = new QStandardItem();

            QString fileName = imageFileInfo.baseName();
            item->setText(fileName);

            qDebug()<<"sll-----imageFilePath = "<<imageFileInfo.filePath();
            QIcon icon = getListViewItemIcon(imageFileInfo.filePath());
            item->setIcon(icon);

            QString toolTip = fileName;
            item->setToolTip(toolTip);

            StickerUserData *stickerUserData = new StickerUserData;
            stickerUserData->imageFilePath = imageFileInfo.filePath();
            QByteArray userDataByteArray;
            userDataByteArray.append(reinterpret_cast<char *>(stickerUserData), sizeof(StickerUserData));
            item->setData(userDataByteArray, Qt::UserRole);

            connect(this, SIGNAL(currentSelectedAnimationChanged(QString &)), itemMode, SLOT(updatCurrentSelectedAnimationFilePath(QString &)));

            itemMode->appendRow(item);
        }

        qDebug()<<"sll-----className = "<<className;

        stickerDockListViewItemModel->insert(className, itemMode);
    }

    qDebug()<<"sll-----createAllClassesItemModel---end";
    return stickerDockListViewItemModel;
}

bool StickerDockWidget::hasClass()
{
    qDebug()<<"sll-----hasClass---start";
    qDebug()<<"sll-----hasClass---end";
    return true;
}

void StickerDockWidget::addToTimeline(const QStandardItem *item)
{
    qDebug()<<"sll-----addToTimeline---start";

    QVariant userDataVariant = item->data(Qt::UserRole);
    QByteArray userByteArray = userDataVariant.value<QByteArray>();
    StickerUserData *stickerUserData = reinterpret_cast<StickerUserData *>(userByteArray.data());

    QVariant currentSelecteditemData = m_animationCombobox->currentData();
    QString currentSelectedAnimationFilePath = currentSelecteditemData.toString();

    qDebug()<<"sll-----imageFilePath = "<<stickerUserData->imageFilePath;
    qDebug()<<"sll-----animationFilePath = "<<currentSelectedAnimationFilePath;

    FILE_HANDLE fileHandle = createFileHandle(m_mainInterface, currentSelectedAnimationFilePath,
                                              stickerUserData->imageFilePath);

    Q_ASSERT(fileHandle);
    Q_ASSERT(m_mainInterface);
    if (fileHandle && m_mainInterface)
    {
        m_mainInterface->addToTimeLine(fileHandle);
        m_mainInterface->destroyFileHandle(fileHandle);
    }

    qDebug()<<"sll-----addToTimeline---end";
}

void StickerDockWidget::preview(const QStandardItem *item)
{
    qDebug()<<"sll-----preview---start";
    QVariant userDataVariant = item->data(Qt::UserRole);
    QByteArray userByteArray = userDataVariant.value<QByteArray>();
    StickerUserData *stickerUserData = reinterpret_cast<StickerUserData *>(userByteArray.data());

    m_currentSelectedImageFilePath = stickerUserData->imageFilePath;

    QVariant currentSelecteditemData = m_animationCombobox->currentData();
    QString currentSelectedAnimationFilePath = currentSelecteditemData.toString();

    qDebug()<<"sll-----imageFilePath = "<<stickerUserData->imageFilePath;
    qDebug()<<"sll-----animationFilePath = "<<currentSelectedAnimationFilePath;

    FILE_HANDLE fileHandle = createFileHandle(m_mainInterface, currentSelectedAnimationFilePath,
                                              m_currentSelectedImageFilePath);

    Q_ASSERT(fileHandle);
    Q_ASSERT(m_mainInterface);
    if (fileHandle && m_mainInterface)
    {
        m_mainInterface->playFile(fileHandle);
        m_mainInterface->destroyFileHandle(fileHandle);
    }

    qDebug()<<"sll-----preview---end";
}

QString StickerDockWidget::getImageClassType(QString srcStr)
{
    QJsonObject imageClassPropertyInfo;
    QString stickerDir = Util::resourcesPath() + "/template/sticker";
    TranslationHelper::readJsonFile(stickerDir + "imageclass_property_info.json", imageClassPropertyInfo);

    QString result = "";
    if (imageClassPropertyInfo.isEmpty())
    {
        return result;
    }

    if (imageClassPropertyInfo.contains(srcStr))
    {
        QJsonObject subObj = imageClassPropertyInfo.value(srcStr).toObject();
        result = subObj.value("type").toString();
    }

    return result;
}

//FIXME: 函数过长需要重构
FILE_HANDLE StickerDockWidget::createFileHandle(MainInterface *mainInterface,
                                    const QString &animationFilePath,
                                    const QString &imageFilePath)
{
    qDebug()<<"sll-----createFileHandle---start";

    Q_ASSERT(!animationFilePath.isNull());
//    Q_ASSERT(!imageFilePath.isNull());
    Q_ASSERT(mainInterface);

    FILE_HANDLE fileHandle = nullptr;

    if (mainInterface && !animationFilePath.isNull() && !imageFilePath.isNull())
    {
        QFile file(animationFilePath);
        Q_ASSERT(file.exists());
        if (!file.exists())
        {
            return fileHandle;
        }

        QDomDocument doc;
        QString error;
        if (!doc.setContent(&file, &error))
        {
            qDebug()<<"sll-----QDomDocument error!！";
            qDebug()<<error;
            file.close();
            return fileHandle;
        }
        file.close();

        QDomNodeList nodeList = doc.elementsByTagName("producer");
        bool flagResource = false;
        bool flagHash = false;
        for (int i = 0; i < nodeList.count(); i++)
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
            QPixmap pixmap = QPixmap(imageFilePath);
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

            QDir path = QDir(imageFilePath);
            path.cdUp();
            QString className = path.dirName();

            // 设置size滤镜参数
            if (getImageClassType(className) == "A")
            {
                QDomNodeList filterList = domElement.elementsByTagName("filter");
                for (int k = 0; k < filterList.count(); k++)
                {
                    QDomElement filter = filterList.at(k).toElement();
                    if (filter.text().contains("affineSizePosition"))
                    {
                        QDomNodeList propertyList = filter.elementsByTagName("property");
                        for (int m = 0; m < propertyList.count(); m++)
                        {
                            QDomElement prop = propertyList.at(m).toElement();
                            if (prop.attribute("name").contains("transition.distort"))
                            {
                                domNodedistort = prop.toElement().firstChild();
                                domNodedistort.setNodeValue("1");
                            }
                            if (prop.attribute("name").contains("transition.rect_anim_relative"))
                            {
                                domNodeRect = prop.toElement().firstChild();
                                domNodeRect.setNodeValue("0.0 0.0 1.0 1.0 1");
                            }
                        }
                        break;
                    }
                }
            }
            else if (getImageClassType(className) == "B")
            {
                QDomNodeList filterList = domElement.elementsByTagName("filter");
                for(int k = 0; k < filterList.count(); k++)
                {
                    QDomElement filter = filterList.at(k).toElement();
                    if (filter.text().contains("affineSizePosition"))
                    {
                        QDomNodeList propertyList = filter.elementsByTagName("property");
                        for (int m = 0; m < propertyList.count(); m++)
                        {
                            QDomElement prop = propertyList.at(m).toElement();
                            if (prop.attribute("name").contains("transition.distort"))
                            {
                                domNodedistort = prop.toElement().firstChild();
                                domNodedistort.setNodeValue("0");
                            }
                            if (prop.attribute("name").contains("transition.rect_anim_relative"))
                            {
                                domNodeRect = prop.toElement().firstChild();
                                QString value = prop.toElement().text();
                                if (!value.contains("~="))
                                {
                                    QString newValue = value.split(" ")[0] + " " + value.split(" ")[1] + " " + "0.25 0.25 1";
                                    domNodeRect.setNodeValue(newValue);
                                }
                            }
                        }
                        break;
                    }
                }
            }

            // 设置图片路径
            QDomNodeList elementList = domElement.elementsByTagName("property");
            for (int j = 0; j < elementList.count(); j++)
            {
                QDomElement de = elementList.at(j).toElement();
                if (de.attribute("name").contains("resource"))
                {
                    domNodeResource = de.toElement().firstChild();
                    flagResource = true;
                    if (flagHash)
                    {
                        break;
                    }
                    continue;
                }
                if(de.attribute("name").contains("moviemator:hash"))
                {
                    domNodeHash = de.toElement().firstChild();
                    flagHash = true;
                    if (flagResource)
                    {
                        break;
                    }
                    continue;
                }
            }

            if(flagResource && flagHash)
            {
                domNodeResource.setNodeValue(imageFilePath);
                domNodeHash.setNodeValue(Util::getFileHash(imageFilePath));
                fileHandle = mainInterface->createFileWithXMLForDragAndDrop(doc.toString());
            }
        }
    }

    qDebug()<<"sll-----createFileHandle---end";

    return fileHandle;
}

void StickerDockWidget::onAnimationComboBoxActivated(int index) {
    qDebug()<<"sll-----onAnimationComboBoxActivated---start";
    QVariant itemData = m_animationCombobox->itemData(index);
    QString animationFilePath = itemData.toString();

    qDebug()<<"sll----animationFilePath = "<<animationFilePath;
    emit currentSelectedAnimationChanged(animationFilePath);//用于更新拖拽数据
    qDebug()<<"sll-----onAnimationComboBoxActivated---start";
}

void StickerDockWidget::onAnimationComboBoxCurrentIndexChanged(int index)
{
    qDebug()<<"sll-----onAnimationComboBoxCurrentIndexChanged---start";
    QVariant itemData = m_animationCombobox->itemData(index);
    QString animationFilePath = itemData.toString();

    qDebug()<<"sll----animationFilePath = "<<animationFilePath;
    qDebug()<<"sll----imageFilePath = "<<m_currentSelectedImageFilePath;

    FILE_HANDLE fileHandle = createFileHandle(m_mainInterface, animationFilePath, m_currentSelectedImageFilePath);

//    Q_ASSERT(fileHandle);
    Q_ASSERT(m_mainInterface);
    if (fileHandle && m_mainInterface)
    {
        m_mainInterface->playFile(fileHandle);
        m_mainInterface->destroyFileHandle(fileHandle);
    }

    emit currentSelectedAnimationChanged(animationFilePath);//用于更新拖拽数据

    qDebug()<<"sll-----onAnimationComboBoxCurrentIndexChanged---end";
}

QDockWidget *g_createStickerDock(MainInterface *main)
{
    qDebug()<<"sll-----g_createStickerDock---start";
    StickerDockWidget *stickerDockWidget = new StickerDockWidget(main);
    stickerDockWidget->setupUi();
    qDebug()<<"sll-----g_createStickerDock---end";

    return stickerDockWidget;
}
