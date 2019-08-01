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
    m_pMainInterface(pMainInterface)
{
    qDebug()<<"sll-----StickerDockWidget构造---start";
    qDebug()<<"sll-----StickerDockWidget构造---end";
}

void StickerDockWidget::setupOtherUi()
{
    qDebug()<<"sll-----setupOtherUi---start";

    QHBoxLayout *pAnimationWidgetLayout = new QHBoxLayout();

    //动画标签
    QLabel *pAnimationLabel = new QLabel(tr("Animation:"));
    pAnimationWidgetLayout->addWidget(pAnimationLabel);

    //动画combobox
    m_pAnimationCombobox = new QComboBox();
    m_pAnimationCombobox->setStyleSheet("QComboBox { background-color:rgb(100,100,100);color:rgb(225,225,225); }");
    pAnimationWidgetLayout->addWidget(m_pAnimationCombobox);
    pAnimationWidgetLayout->setSpacing(10);

    ui->horizontalLayout_titlebar->addLayout(pAnimationWidgetLayout);

    ui->horizontalSpacer->changeSize(10, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    setupAnimationComboboxData();

    connect(m_pAnimationCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAnimationComboBoxCurrentIndexChanged(int)));
    connect(m_pAnimationCombobox, SIGNAL(activated(int)), this, SLOT(onAnimationComboBoxActivated(int)));

    qDebug()<<"sll-----setupOtherUi---end";
}

void StickerDockWidget::resizeEvent(QResizeEvent *pEvent)
{
    qDebug()<<"sll-----resizeEvent---start";

    onAnimationComboBoxActivated(m_pAnimationCombobox->currentIndex());
    BaseDockWidget::resizeEvent(pEvent);

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
        QString strItemName = TranslationHelper::getTranslationStr(animationFileInfo.baseName(), animationTranslateInfo);
        m_pAnimationCombobox->addItem(strItemName, QVariant(animationFileInfo.filePath()));
    }

    qDebug()<<"sll-----setupAnimationComboboxData---end";
}

QIcon StickerDockWidget::getListViewItemIcon(const QString strFilePath)
{
    QImage iconImage = QImage(LISTVIEW_ITEMICONSIZE_WIDTH,
                              LISTVIEW_ITEMICONSIZE_HEIGHT,
                              QImage::Format_ARGB32);

    QImage image = QImage(strFilePath);
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

    return QPixmap::fromImage(iconImage);
}

QMap<QString, BaseItemModel *> *StickerDockWidget::createAllClassesItemModel()
{
    qDebug()<<"sll-----createAllClassesItemModel---start";

    QMap<QString, BaseItemModel *> *pStickerDockListViewItemModel = new QMap<QString, BaseItemModel *>;

    QString strStickerDir = Util::resourcesPath() + "/template/sticker";

    QJsonObject stickerClassNameTranslateInfo;
    TranslationHelper::readJsonFile(strStickerDir + "/imageclass_name_translation_info.json", stickerClassNameTranslateInfo);

    QDir stickerImageFileDir(strStickerDir + "/image");
    QFileInfoList allClassFolderInfo = stickerImageFileDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo oneClassFolderInfo, allClassFolderInfo)
    {
        QFileInfoList oneClassFileList = QDir(oneClassFolderInfo.absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoSymLinks);

        QString strClassName = TranslationHelper::getTranslationStr(oneClassFolderInfo.fileName(), stickerClassNameTranslateInfo);

        StickerItemModel *pItemMode = new StickerItemModel(m_pMainInterface, this);
        foreach (QFileInfo imageFileInfo, oneClassFileList)
        {
            QStandardItem *pItem = new QStandardItem();

            QString strFileName = imageFileInfo.baseName();
            pItem->setText(strFileName);

            qDebug()<<"sll-----imageFilePath = "<<imageFileInfo.filePath();
            QIcon icon = getListViewItemIcon(imageFileInfo.filePath());
            pItem->setIcon(icon);

            QString strToolTip = strFileName;
            pItem->setToolTip(strToolTip);

            StickerUserData *pStickerUserData = new StickerUserData;
            pStickerUserData->strImageFilePath = imageFileInfo.filePath();
            QByteArray userDataByteArray;
            userDataByteArray.append(reinterpret_cast<char *>(pStickerUserData), sizeof(StickerUserData));
            pItem->setData(userDataByteArray, Qt::UserRole);

            connect(this, SIGNAL(currentSelectedAnimationChanged(QString &)), pItemMode, SLOT(updatCurrentSelectedAnimationFilePath(QString &)));

            pItemMode->appendRow(pItem);
        }

        qDebug()<<"sll-----className = "<<strClassName;

        pStickerDockListViewItemModel->insert(strClassName, pItemMode);
    }

    qDebug()<<"sll-----createAllClassesItemModel---end";
    return pStickerDockListViewItemModel;
}

bool StickerDockWidget::hasClass()
{
    qDebug()<<"sll-----hasClass---start";
    qDebug()<<"sll-----hasClass---end";
    return true;
}

void StickerDockWidget::addToTimeline(const QStandardItem *pItem)
{
    qDebug()<<"sll-----addToTimeline---start";

    QVariant userDataVariant            = pItem->data(Qt::UserRole);
    QByteArray userByteArray            = userDataVariant.value<QByteArray>();
    StickerUserData *pStickerUserData   = reinterpret_cast<StickerUserData *>(userByteArray.data());

    QVariant currentSelecteditemData            = m_pAnimationCombobox->currentData();
    QString strCurrentSelectedAnimationFilePath = currentSelecteditemData.toString();

    qDebug()<<"sll-----imageFilePath = "<<pStickerUserData->strImageFilePath;
    qDebug()<<"sll-----animationFilePath = "<<strCurrentSelectedAnimationFilePath;

    FILE_HANDLE fileHandle = createFileHandle(m_pMainInterface, strCurrentSelectedAnimationFilePath,
                                              pStickerUserData->strImageFilePath);

    Q_ASSERT(fileHandle);
    Q_ASSERT(m_pMainInterface);
    if (fileHandle && m_pMainInterface)
    {
        m_pMainInterface->addToTimeLine(fileHandle);
        m_pMainInterface->destroyFileHandle(fileHandle);
    }

    qDebug()<<"sll-----addToTimeline---end";
}

void StickerDockWidget::preview(const QStandardItem *pItem)
{
    qDebug()<<"sll-----preview---start";

    QVariant userDataVariant            = pItem->data(Qt::UserRole);
    QByteArray userByteArray            = userDataVariant.value<QByteArray>();
    StickerUserData *pStickerUserData   = reinterpret_cast<StickerUserData *>(userByteArray.data());

    m_strCurrentSelectedImageFilePath = pStickerUserData->strImageFilePath;

    QVariant currentSelecteditemData            = m_pAnimationCombobox->currentData();
    QString strCurrentSelectedAnimationFilePath = currentSelecteditemData.toString();

    qDebug()<<"sll-----imageFilePath = "<<pStickerUserData->strImageFilePath;
    qDebug()<<"sll-----animationFilePath = "<<strCurrentSelectedAnimationFilePath;

    FILE_HANDLE fileHandle = createFileHandle(m_pMainInterface, strCurrentSelectedAnimationFilePath,
                                              m_strCurrentSelectedImageFilePath);

    Q_ASSERT(fileHandle);
    Q_ASSERT(m_pMainInterface);
    if (fileHandle && m_pMainInterface)
    {
        m_pMainInterface->playFile(fileHandle);
        m_pMainInterface->destroyFileHandle(fileHandle);
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
FILE_HANDLE StickerDockWidget::createFileHandle(MainInterface *pMainInterface,
                                                const QString &strAnimationFilePath,
                                                const QString &strImageFilePath)
{
    qDebug()<<"sll-----createFileHandle---start";

    Q_ASSERT(!strAnimationFilePath.isNull());
//    Q_ASSERT(!imageFilePath.isNull());
    Q_ASSERT(pMainInterface);

    FILE_HANDLE fileHandle = nullptr;

    if (pMainInterface && !strAnimationFilePath.isNull() && !strImageFilePath.isNull())
    {
        QFile file(strAnimationFilePath);
        Q_ASSERT(file.exists());
        if (!file.exists())
        {
            return fileHandle;
        }

        QDomDocument doc;
        QString strError;
        if (!doc.setContent(&file, &strError))
        {
            qDebug()<<"sll-----QDomDocument error!！";
            qDebug()<<strError;
            file.close();
            return fileHandle;
        }
        file.close();

        QDomNodeList nodeList = doc.elementsByTagName("producer");
        bool bFlagResource = false;
        bool bFlagHash = false;
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
            QPixmap pixmap = QPixmap(strImageFilePath);
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

            QDir path = QDir(strImageFilePath);
            path.cdUp();
            QString strClassName = path.dirName();

            // 设置size滤镜参数
            if (getImageClassType(strClassName) == "A")
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
            else if (getImageClassType(strClassName) == "B")
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
                    bFlagResource = true;
                    if (bFlagHash)
                    {
                        break;
                    }
                    continue;
                }
                if(de.attribute("name").contains("moviemator:hash"))
                {
                    domNodeHash = de.toElement().firstChild();
                    bFlagHash = true;
                    if (bFlagResource)
                    {
                        break;
                    }
                    continue;
                }
            }

            if(bFlagResource && bFlagHash)
            {
                domNodeResource.setNodeValue(strImageFilePath);
                domNodeHash.setNodeValue(Util::getFileHash(strImageFilePath));
                fileHandle = pMainInterface->createFileWithXMLForDragAndDrop(doc.toString());
            }
        }
    }

    qDebug()<<"sll-----createFileHandle---end";

    return fileHandle;
}

void StickerDockWidget::onAnimationComboBoxActivated(int nIndex) {
    qDebug()<<"sll-----onAnimationComboBoxActivated---start";
    QVariant itemData               = m_pAnimationCombobox->itemData(nIndex);
    QString strAnimationFilePath    = itemData.toString();

    qDebug()<<"sll----animationFilePath = "<<strAnimationFilePath;
    emit currentSelectedAnimationChanged(strAnimationFilePath);//用于更新拖拽数据
    qDebug()<<"sll-----onAnimationComboBoxActivated---start";
}

void StickerDockWidget::onAnimationComboBoxCurrentIndexChanged(int nIndex)
{
    qDebug()<<"sll-----onAnimationComboBoxCurrentIndexChanged---start";
    QVariant itemData               = m_pAnimationCombobox->itemData(nIndex);
    QString strAnimationFilePath    = itemData.toString();

    qDebug()<<"sll----animationFilePath = "<<strAnimationFilePath;
    qDebug()<<"sll----imageFilePath = "<<m_strCurrentSelectedImageFilePath;

    FILE_HANDLE fileHandle = createFileHandle(m_pMainInterface, strAnimationFilePath, m_strCurrentSelectedImageFilePath);

//    Q_ASSERT(fileHandle);
    Q_ASSERT(m_pMainInterface);
    if (fileHandle && m_pMainInterface)
    {
        m_pMainInterface->playFile(fileHandle);
        m_pMainInterface->destroyFileHandle(fileHandle);
    }

    emit currentSelectedAnimationChanged(strAnimationFilePath);//用于更新拖拽数据

    qDebug()<<"sll-----onAnimationComboBoxCurrentIndexChanged---end";
}

QDockWidget *g_createStickerDock(MainInterface *pMainInterface)
{
    qDebug()<<"sll-----g_createStickerDock---start";
    StickerDockWidget *pStickerDockWidget = new StickerDockWidget(pMainInterface);
    pStickerDockWidget->setupUi();
    qDebug()<<"sll-----g_createStickerDock---end";

    return pStickerDockWidget;
}
