#include "textdockwidget.h"
#include "util.h"

#include <qmetatype.h>
#include <qstandarditemmodel.h>
#include <qdir.h>
#include <qdebug.h>

struct TextUserData {
    QString xmlFilePath;
};

Q_DECLARE_METATYPE(TextUserData)

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

QMap<QString, QStandardItemModel *> *TextDockWidget::createAllClassesItemModel() {
    qDebug()<<"sll-----createAllClassesItemModel---start";
    QMap<QString, QStandardItemModel *> *textDockListViewItemModel = new QMap<QString, QStandardItemModel *>;

    QDir textTemplateDir(Util::resourcesPath() + "/template/text");
    QFileInfoList allClassFolderInfo = textTemplateDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo oneClassFolderInfo, allClassFolderInfo) {
        QFileInfoList oneClassFileList = QDir(oneClassFolderInfo.absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoSymLinks);
        QString className = oneClassFolderInfo.fileName();
        QStandardItemModel *itemMode = new QStandardItemModel(this);
        foreach (QFileInfo templateFileInfo, oneClassFileList) {
            QString fileName = templateFileInfo.baseName();
            QIcon icon = QIcon(":///icons/filters/Common.jpg");
            QString toolTip = fileName;
            TextUserData textUserData;
            textUserData.xmlFilePath = templateFileInfo.filePath();
            QVariant userData = QVariant::fromValue(textUserData);
            QStandardItem *item = new QStandardItem();
            item->setText(fileName);
            item->setIcon(icon);
            item->setToolTip(toolTip);
            item->setData(userData, Qt::UserRole);

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

    QVariant data = item->data(Qt::UserRole);
    TextUserData textUserData = data.value<TextUserData>();
    qDebug()<<"sll-----xmlFilePath = "<<textUserData.xmlFilePath;

    qDebug()<<"sll-----addToTimeline---end";
}

void TextDockWidget::preview(const QStandardItem *item) {
    qDebug()<<"sll-----preview---start";
    QVariant data = item->data(Qt::UserRole);
    TextUserData textUserData = data.value<TextUserData>();
    qDebug()<<"sll-----xmlFilePath = "<<textUserData.xmlFilePath;
    qDebug()<<"sll-----preview---start";
}

QDockWidget *createTextDock(MainInterface *main) {
    qDebug()<<"sll-----createTextDock---start";
    TextDockWidget *textDockWidget = new TextDockWidget(main);
    textDockWidget->setupListView();
    qDebug()<<"sll-----createTextDock---end";

    return textDockWidget;
}


