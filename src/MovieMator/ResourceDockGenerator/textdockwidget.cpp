#include "textdockwidget.h"
#include "util.h"
#include "ui_basedockwidget.h"
#include "textitemmodel.h"
#include "translationhelper.h"

#include <qdir.h>
#include <qdebug.h>

TextDockWidget::TextDockWidget(MainInterface *main, QWidget *parent) :
    BaseDockWidget(parent),
    m_mainInterface(main) {
    qDebug()<<"sll-----TextDockWidget构造---start";
    qDebug()<<"sll-----TextDockWidget构造---end";
}

TextDockWidget::~TextDockWidget() {
    qDebug()<<"sll-----TextDockWidget析构---start";
    qDebug()<<"sll-----TextDockWidget析构---end";
}

QMap<QString, BaseItemModel *> *TextDockWidget::createAllClassesItemModel() {
    qDebug()<<"sll-----createAllClassesItemModel---start";
    QMap<QString, BaseItemModel *> *textDockListViewItemModel = new QMap<QString, BaseItemModel *>;

    QString textDir = Util::resourcesPath() + "/template/text";

    QJsonObject textClassNameTranslateInfo;
    TranslationHelper::readJsonFile(textDir + "/textclass_name_translation_info.json", textClassNameTranslateInfo);

    QJsonObject textFileNameTranslateInfo;
    TranslationHelper::readJsonFile(textDir + "/textfile_name_translation_info.json", textFileNameTranslateInfo);

    QDir textTemplateFileDir(textDir + "/template");
    QFileInfoList allClassFolderInfo = textTemplateFileDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo oneClassFolderInfo, allClassFolderInfo) {
        QFileInfoList oneClassFileList = QDir(oneClassFolderInfo.absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoSymLinks);

        QString className = TranslationHelper::getTranslationStr(oneClassFolderInfo.fileName(), textClassNameTranslateInfo);

        TextItemModel *itemMode = new TextItemModel(m_mainInterface, this);
        foreach (QFileInfo templateFileInfo, oneClassFileList) {
            QStandardItem *item = new QStandardItem();

            QString fileName = TranslationHelper::getTranslationStr(templateFileInfo.baseName(), textFileNameTranslateInfo);
            item->setText(fileName);

            qDebug()<<"sll----------"<<textDir + "/thumbnail/" + oneClassFolderInfo.fileName() + "/" + templateFileInfo.baseName() + ".jpg";
//            QIcon icon = QIcon(textDir + "/thumbnail/" + oneClassFolderInfo.fileName() + "/" + templateFileInfo.baseName() + ".jpg");
            QIcon icon = QIcon(":///icons/filters/Common.jpg");
            item->setIcon(icon);

            QString toolTip = fileName;
            item->setToolTip(toolTip);

            TextUserData *textUserData = new TextUserData;
            textUserData->xmlFilePath = templateFileInfo.filePath();
            QByteArray userDataByteArray;
            userDataByteArray.append(reinterpret_cast<char *>(textUserData), sizeof(TextUserData));
            item->setData(userDataByteArray, Qt::UserRole);

            itemMode->appendRow(item);

            qDebug()<<"sll-----xmlFilePath = "<<templateFileInfo.filePath();
        }

        qDebug()<<"sll-----className = "<<className;

        textDockListViewItemModel->insert(className, itemMode);
    }

    qDebug()<<"sll-----createAllClassesItemModel---end";
    return textDockListViewItemModel;
}

bool TextDockWidget::hasClass() {
    qDebug()<<"sll-----hasClass---start";
    qDebug()<<"sll-----hasClass---end";
    return true;
}

void TextDockWidget::addToTimeline(const QStandardItem *item) {
    qDebug()<<"sll-----addToTimeline---start";

    QVariant userDataVariant = item->data(Qt::UserRole);
    QByteArray userByteArray = userDataVariant.value<QByteArray>();
    TextUserData *textUserData = reinterpret_cast<TextUserData *>(userByteArray.data());

    qDebug()<<"sll-----xmlFilePath = "<<textUserData->xmlFilePath;

    FILE_HANDLE fileHandel = m_mainInterface->openFile(textUserData->xmlFilePath);
    if (fileHandel) {
        m_mainInterface->addToTimeLine(fileHandel);

        m_mainInterface->destroyFileHandle(fileHandel);
    }

    qDebug()<<"sll-----addToTimeline---end";
}

void TextDockWidget::preview(const QStandardItem *item) {
    qDebug()<<"sll-----preview---start";
    QVariant userDataVariant = item->data(Qt::UserRole);
    QByteArray userByteArray = userDataVariant.value<QByteArray>();
    TextUserData *textUserData = reinterpret_cast<TextUserData *>(userByteArray.data());

    qDebug()<<"sll-----xmlFilePath = "<<textUserData->xmlFilePath;

    FILE_HANDLE fileHandel = m_mainInterface->openFile(textUserData->xmlFilePath);
    if (fileHandel) {
        m_mainInterface->playFile(fileHandel);

        m_mainInterface->destroyFileHandle(fileHandel);
    }

    qDebug()<<"sll-----preview---end";
}

QDockWidget *createTextDock(MainInterface *main) {
    qDebug()<<"sll-----createTextDock---start";
    TextDockWidget *textDockWidget = new TextDockWidget(main);
    textDockWidget->setupUi();
    qDebug()<<"sll-----createTextDock---end";

    return textDockWidget;
}


