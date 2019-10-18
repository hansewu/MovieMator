#include "htmleffectdockwidget.h"
#include "util.h"
#include "ui_basedockwidget.h"
#include "htmleffectitemmodel.h"
#include "translationhelper.h"

#include <qdir.h>
#include <qdebug.h>
#include <settings.h>

HtmlEffectDockWidget::HtmlEffectDockWidget(MainInterface *pMainInterface, QWidget *pParent) :
    BaseDockWidget(pParent),
    m_pMainInterface(pMainInterface)
{
    qDebug()<<"--HtmlEffectDockWidget构造---start";
    qDebug()<<"---HtmlEffectDockWidget构造---end";
}

HtmlEffectDockWidget::~HtmlEffectDockWidget()
{
    qDebug()<<"--HtmlEffectDockWidget析构---start";
    qDebug()<<"---HtmlEffectDockWidget析构---end";
}

UnsortMap<QString, BaseItemModel *> *HtmlEffectDockWidget::createAllClassesItemModel()
{
    qDebug()<<"cx--createAllClassesItemModel---start";
    UnsortMap<QString, BaseItemModel *> *pHtmlEffectDockListViewItemModel = new UnsortMap<QString, BaseItemModel *>;

    QString strHtmlEffectDir = Util::resourcesPath() + "/template/htmlEffect";

    QJsonObject htmlEffectClassNameTranslateInfo;
    TranslationHelper::readJsonFile(strHtmlEffectDir + "/htmlEffectclass_name_translation_info.json", htmlEffectClassNameTranslateInfo);

    QJsonObject htmlEffectFileNameTranslateInfo;
    TranslationHelper::readJsonFile(strHtmlEffectDir + "/htmlEffectfile_name_translation_info.json", htmlEffectFileNameTranslateInfo);

    QString strTemplateFolder = "/template";

    QDir htmlEffectTemplateFileDir(strHtmlEffectDir + strTemplateFolder);
    QFileInfoList allClassFolderInfo = htmlEffectTemplateFileDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo oneClassFolderInfo, allClassFolderInfo)
    {
        QFileInfoList oneClassFileList = QDir(oneClassFolderInfo.absoluteFilePath()).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        QString strClassName        = TranslationHelper::getTranslationStr(oneClassFolderInfo.fileName(), htmlEffectClassNameTranslateInfo);
        HtmlEffectItemModel *pItemMode    = new HtmlEffectItemModel(m_pMainInterface, this);
        foreach (QFileInfo templateFileInfo, oneClassFileList)
        {
            QStandardItem *pItem = new QStandardItem();

            QString strFileName = TranslationHelper::getTranslationStr(templateFileInfo.baseName(), htmlEffectFileNameTranslateInfo);
            pItem->setText(strFileName);

            qDebug()<<"--icon = "<<templateFileInfo.filePath() + "/" + templateFileInfo.baseName() + ".jpg";
            QIcon icon = QIcon(templateFileInfo.filePath() + "/" + templateFileInfo.baseName() + ".jpg");
            pItem->setIcon(icon);

            QString strToolTip = strFileName;
            pItem->setToolTip(strToolTip);

            HtmlEffectUserData *pHtmlEffectUserData = new HtmlEffectUserData;
            pHtmlEffectUserData->strXmlFilePath = templateFileInfo.filePath() + "/" + templateFileInfo.baseName() + ".xml";
            QByteArray userDataByteArray;
            userDataByteArray.append(reinterpret_cast<char *>(pHtmlEffectUserData), sizeof(HtmlEffectUserData));
            pItem->setData(userDataByteArray, Qt::UserRole);

            pItemMode->appendRow(pItem);

            qDebug()<<"--xmlFilePath = "<<templateFileInfo.filePath();
        }

        qDebug()<<"--className = "<<strClassName;

        pHtmlEffectDockListViewItemModel->append(strClassName, pItemMode);
    }

    qDebug()<<"--createAllClassesItemModel---end";
    return pHtmlEffectDockListViewItemModel;
}

void HtmlEffectDockWidget::addItemToTimeline(const QStandardItem *pItem)
{
    qDebug()<<"--addToTimeline---start";

    QVariant userDataVariant    = pItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    HtmlEffectUserData *pHtmlEffectUserData = reinterpret_cast<HtmlEffectUserData *>(userByteArray.data());

    qDebug()<<"---xmlFilePath = "<<pHtmlEffectUserData->strXmlFilePath;

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pHtmlEffectUserData->strXmlFilePath);
    if (fileHandel) {
        m_pMainInterface->addToTimeLine(fileHandel);

        m_pMainInterface->destroyFileHandle(fileHandel);
    }

    qDebug()<<"---addToTimeline---end";
}

void HtmlEffectDockWidget::preview(const QStandardItem *pItem)
{
    qDebug()<<"--preview---start";
    QVariant userDataVariant    = pItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    HtmlEffectUserData *pHtmlEffectUserData = reinterpret_cast<HtmlEffectUserData *>(userByteArray.data());

    qDebug()<<"--xmlFilePath = "<<pHtmlEffectUserData->strXmlFilePath;

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pHtmlEffectUserData->strXmlFilePath);
    if (fileHandel) {
        m_pMainInterface->playFile(fileHandel);

        m_pMainInterface->destroyFileHandle(fileHandel);
    }

    qDebug()<<"---preview---end";
}

static HtmlEffectDockWidget *pHtmlEffectDockInstance = nullptr;
QDockWidget *RDG_CreateHtmlEffectDock(MainInterface *pMainInterface)
{
    if(pMainInterface && (pHtmlEffectDockInstance == nullptr))
    {
        pHtmlEffectDockInstance = new HtmlEffectDockWidget(pMainInterface);
        pHtmlEffectDockInstance->setupUi();
    }
    return pHtmlEffectDockInstance;
}


