#include "audiodockwidget.h"
#include "util.h"
#include "ui_basedockwidget.h"
#include "audioitemmodel.h"
#include "translationhelper.h"

#include <qdir.h>
#include <qdebug.h>
#include <settings.h>

AudioDockWidget::AudioDockWidget(MainInterface *pMainInterface, QWidget *pParent) :
    BaseDockWidget(pParent),
    m_pMainInterface(pMainInterface)
{
}

AudioDockWidget::~AudioDockWidget()
{
}

UnsortMap<QString, BaseItemModel *> *AudioDockWidget::createAllClassesItemModel()
{
    UnsortMap<QString, BaseItemModel *> *pTextDockListViewItemModel = new UnsortMap<QString, BaseItemModel *>;

    QString strTextDir = Util::resourcesPath() + "/template/audio";

    QJsonObject textClassNameTranslateInfo;
    TranslationHelper::readJsonFile(strTextDir + "/audioclass_name_translation_info.json", textClassNameTranslateInfo);

    QJsonObject textFileNameTranslateInfo;
    TranslationHelper::readJsonFile(strTextDir + "/audiofile_name_translation_info.json", textFileNameTranslateInfo);

    QString strResourceFolder = "/resource";
//    if((Settings.language() != "zh_CN") && (Settings.language() != "zh"))
//    {
//        strTemplateFolder = "/template_en";
//    }

    QDir textTemplateFileDir(strTextDir + strResourceFolder);
    QFileInfoList allClassFolderInfo = textTemplateFileDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo oneClassFolderInfo, allClassFolderInfo)
    {
        QFileInfoList oneClassFileList = QDir(oneClassFolderInfo.absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoSymLinks);

        QString strClassName        = TranslationHelper::getTranslationStr(oneClassFolderInfo.fileName(), textClassNameTranslateInfo);
        AudioItemModel *pItemMode    = new AudioItemModel(m_pMainInterface, this);
        foreach (QFileInfo templateFileInfo, oneClassFileList)
        {
            QStandardItem *pItem = new QStandardItem();

            QString strFileName = TranslationHelper::getTranslationStr(templateFileInfo.baseName(), textFileNameTranslateInfo);
            pItem->setText(strFileName);

            QString thumbnailFolderName = ":/icons/filters/Audio.png";
//            if((Settings.language() != "zh_CN") && (Settings.language() != "zh"))
//            {
//                thumbnailFolderName = "/thumbnail_en/";
//            }
            QIcon icon = QIcon(thumbnailFolderName);
            pItem->setIcon(icon);

            QString strToolTip = strFileName;
            pItem->setToolTip(strToolTip);

            AudioUserData *pTextUserData = new AudioUserData;
            pTextUserData->strXmlFilePath = templateFileInfo.filePath();
            QByteArray userDataByteArray;
            userDataByteArray.append(reinterpret_cast<char *>(pTextUserData), sizeof(AudioUserData));
            pItem->setData(userDataByteArray, Qt::UserRole);

            pItemMode->appendRow(pItem);
        }

        pTextDockListViewItemModel->append(strClassName, pItemMode);
    }

    return pTextDockListViewItemModel;
}

void AudioDockWidget::addItemToTimeline(const QStandardItem *pItem)
{
    QVariant userDataVariant    = pItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    AudioUserData *pAudioUserData = reinterpret_cast<AudioUserData *>(userByteArray.data());

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pAudioUserData->strXmlFilePath);
    if (fileHandel) {
        m_pMainInterface->addToTimeLine(fileHandel);

        m_pMainInterface->destroyFileHandle(fileHandel);
    }
}

void AudioDockWidget::preview(const QStandardItem *pItem)
{
    QVariant userDataVariant    = pItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    AudioUserData *pAuidoUserData = reinterpret_cast<AudioUserData *>(userByteArray.data());

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pAuidoUserData->strXmlFilePath);
    if (fileHandel) {
        m_pMainInterface->playFile(fileHandel);

        m_pMainInterface->destroyFileHandle(fileHandel);
    }
}

static AudioDockWidget *pAudioDockInstance = nullptr;
QDockWidget *RDG_CreateAudioDock(MainInterface *pMainInterface)
{
    if(pMainInterface && (pAudioDockInstance == nullptr))
    {
        pAudioDockInstance = new AudioDockWidget(pMainInterface);
        pAudioDockInstance->setupUi();
    }
    return pAudioDockInstance;
}
