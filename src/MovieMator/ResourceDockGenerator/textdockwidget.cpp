#include "textdockwidget.h"
#include "util.h"
#include "ui_basedockwidget.h"
#include "textitemmodel.h"
#include "translationhelper.h"

#include <qdir.h>
#include <qdebug.h>
#include <settings.h>

TextDockWidget::TextDockWidget(MainInterface *pMainInterface, QWidget *pParent) :
    BaseDockWidget(pParent),
    m_pMainInterface(pMainInterface)
{
}

TextDockWidget::~TextDockWidget()
{
}

UnsortMap<QString, BaseItemModel *> *TextDockWidget::createAllClassesItemModel()
{
    UnsortMap<QString, BaseItemModel *> *pTextDockListViewItemModel = new UnsortMap<QString, BaseItemModel *>;

    QString strTextDir = Util::resourcesPath() + "/template/text";

    QJsonObject textClassNameTranslateInfo;
    TranslationHelper::readJsonFile(strTextDir + "/textclass_name_translation_info.json", textClassNameTranslateInfo);

    QJsonObject textFileNameTranslateInfo;
    TranslationHelper::readJsonFile(strTextDir + "/textfile_name_translation_info.json", textFileNameTranslateInfo);

    QString strTemplateFolder = "/template";
    if((Settings.language() != "zh_CN") && (Settings.language() != "zh"))
    {
        strTemplateFolder = "/template_en";
    }

    QDir textTemplateFileDir(strTextDir + strTemplateFolder);
    QFileInfoList allClassFolderInfo = textTemplateFileDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo oneClassFolderInfo, allClassFolderInfo)
    {
        QFileInfoList oneClassFileList = QDir(oneClassFolderInfo.absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoSymLinks);

        QString strClassName        = TranslationHelper::getTranslationStr(oneClassFolderInfo.fileName(), textClassNameTranslateInfo);
        TextItemModel *pItemMode    = new TextItemModel(m_pMainInterface, this);
        foreach (QFileInfo templateFileInfo, oneClassFileList)
        {
            QStandardItem *pItem = new QStandardItem();

            QString strFileName = TranslationHelper::getTranslationStr(templateFileInfo.baseName(), textFileNameTranslateInfo);
            pItem->setText(strFileName);

            QString thumbnailFolderName = "/thumbnail/";
            if((Settings.language() != "zh_CN") && (Settings.language() != "zh"))
            {
                thumbnailFolderName = "/thumbnail_en/";
            }
            QIcon icon = QIcon(strTextDir + thumbnailFolderName + oneClassFolderInfo.fileName() + "/" + templateFileInfo.baseName() + ".jpg");
            pItem->setIcon(icon);

            QString strToolTip = strFileName;
            pItem->setToolTip(strToolTip);

            TextUserData *pTextUserData = new TextUserData;
            pTextUserData->strXmlFilePath = templateFileInfo.filePath();
            QByteArray userDataByteArray;
            userDataByteArray.append(reinterpret_cast<char *>(pTextUserData), sizeof(TextUserData));
            pItem->setData(userDataByteArray, Qt::UserRole);

            pItemMode->appendRow(pItem);
        }
        pTextDockListViewItemModel->append(strClassName, pItemMode);
    }

    return pTextDockListViewItemModel;
}

void TextDockWidget::addItemToTimeline(const QStandardItem *pItem)
{
    QVariant userDataVariant    = pItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    TextUserData *pTextUserData = reinterpret_cast<TextUserData *>(userByteArray.data());

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pTextUserData->strXmlFilePath);
    if (fileHandel) {
        m_pMainInterface->addToTimeLine(fileHandel);

        m_pMainInterface->destroyFileHandle(fileHandel);
    }
}

void TextDockWidget::preview(const QStandardItem *pItem)
{
    QVariant userDataVariant    = pItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    TextUserData *pTextUserData = reinterpret_cast<TextUserData *>(userByteArray.data());

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pTextUserData->strXmlFilePath);
    if (fileHandel) {
        m_pMainInterface->playFile(fileHandel);

        m_pMainInterface->destroyFileHandle(fileHandel);
    }
}

static TextDockWidget *pTextDockInstance = nullptr;
QDockWidget *RDG_CreateTextDock(MainInterface *pMainInterface)
{
    if(pMainInterface && (pTextDockInstance == nullptr))
    {
        pTextDockInstance = new TextDockWidget(pMainInterface);
        pTextDockInstance->setupUi();
    }
    return pTextDockInstance;
}

