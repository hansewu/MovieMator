#include "textdockwidget.h"
#include "util.h"
#include "ui_basedockwidget.h"
#include "textitemmodel.h"
#include "translationhelper.h"

#include <qdir.h>
#include <qdebug.h>

TextDockWidget::TextDockWidget(MainInterface *pMainInterface, QWidget *pParent) :
    BaseDockWidget(pParent),
    m_pMainInterface(pMainInterface)
{
    qDebug()<<"sll-----TextDockWidget构造---start";
    qDebug()<<"sll-----TextDockWidget构造---end";
}

TextDockWidget::~TextDockWidget()
{
    qDebug()<<"sll-----TextDockWidget析构---start";
    qDebug()<<"sll-----TextDockWidget析构---end";
}

QMap<QString, BaseItemModel *> *TextDockWidget::createAllClassesItemModel()
{
    qDebug()<<"sll-----createAllClassesItemModel---start";
    QMap<QString, BaseItemModel *> *pTextDockListViewItemModel = new QMap<QString, BaseItemModel *>;

    QString strTextDir = Util::resourcesPath() + "/template/text";

    QJsonObject textClassNameTranslateInfo;
    TranslationHelper::readJsonFile(strTextDir + "/textclass_name_translation_info.json", textClassNameTranslateInfo);

    QJsonObject textFileNameTranslateInfo;
    TranslationHelper::readJsonFile(strTextDir + "/textfile_name_translation_info.json", textFileNameTranslateInfo);

    QDir textTemplateFileDir(strTextDir + "/template");
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

            qDebug()<<"sll----------"<<strTextDir + "/thumbnail/" + oneClassFolderInfo.fileName() + "/" + templateFileInfo.baseName() + ".jpg";
            QIcon icon = QIcon(strTextDir + "/thumbnail/" + oneClassFolderInfo.fileName() + "/" + templateFileInfo.baseName() + ".jpg");
            pItem->setIcon(icon);

            QString strToolTip = strFileName;
            pItem->setToolTip(strToolTip);

            TextUserData *pTextUserData = new TextUserData;
            pTextUserData->strXmlFilePath = templateFileInfo.filePath();
            QByteArray userDataByteArray;
            userDataByteArray.append(reinterpret_cast<char *>(pTextUserData), sizeof(TextUserData));
            pItem->setData(userDataByteArray, Qt::UserRole);

            pItemMode->appendRow(pItem);

            qDebug()<<"sll-----xmlFilePath = "<<templateFileInfo.filePath();
        }

        qDebug()<<"sll-----className = "<<strClassName;

        pTextDockListViewItemModel->insert(strClassName, pItemMode);
    }

    qDebug()<<"sll-----createAllClassesItemModel---end";
    return pTextDockListViewItemModel;
}

void TextDockWidget::addItemToTimeline(const QStandardItem *pItem)
{
    qDebug()<<"sll-----addToTimeline---start";

    QVariant userDataVariant    = pItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    TextUserData *pTextUserData = reinterpret_cast<TextUserData *>(userByteArray.data());

    qDebug()<<"sll-----xmlFilePath = "<<pTextUserData->strXmlFilePath;

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pTextUserData->strXmlFilePath);
    if (fileHandel) {
        m_pMainInterface->setTextTemplateNameOnTimeline(fileHandel, pItem->text());
        m_pMainInterface->addToTimeLine(fileHandel);

        m_pMainInterface->destroyFileHandle(fileHandel);
    }

    qDebug()<<"sll-----addToTimeline---end";
}

void TextDockWidget::preview(const QStandardItem *pItem)
{
    qDebug()<<"sll-----preview---start";
    QVariant userDataVariant    = pItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    TextUserData *pTextUserData = reinterpret_cast<TextUserData *>(userByteArray.data());

    qDebug()<<"sll-----xmlFilePath = "<<pTextUserData->strXmlFilePath;

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pTextUserData->strXmlFilePath);
    if (fileHandel) {
        m_pMainInterface->playFile(fileHandel);

        m_pMainInterface->destroyFileHandle(fileHandel);
    }

    qDebug()<<"sll-----preview---end";
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

