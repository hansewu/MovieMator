#include "stickeritemmodel.h"
#include "stickerdockwidget.h"

#include <qdebug.h>
#include <qmimedata.h>

StickerItemModel::StickerItemModel(MainInterface *main, QObject *parent) :
    BaseItemModel(parent),
    m_mainInterface(main)
{
}

void StickerItemModel::updatCurrentSelectedAnimationFilePath(QString &animationPath)
{
    m_currentSelectedAnimationFilePath = animationPath;
}

QMimeData *StickerItemModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug()<<"sll-----mimeData---start";
    QModelIndex index = indexes.first();
    QAbstractItemModel *itemModel = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *standardItemModel = static_cast<BaseItemModel *>(itemModel);
    QStandardItem *standardItem = standardItemModel->itemFromIndex(index);

    QVariant userDataVariant = standardItem->data(Qt::UserRole);
    QByteArray userByteArray = userDataVariant.value<QByteArray>();
    StickerUserData *stickerUserData = reinterpret_cast<StickerUserData *>(userByteArray.data());

    qDebug()<<"sll----imageFilePath = "<<stickerUserData->imageFilePath;
    qDebug()<<"sll----animationFilePath = "<<m_currentSelectedAnimationFilePath;

    Q_ASSERT(m_mainInterface);
    QMimeData *mimeData = nullptr;
    if (m_mainInterface) {
        FILE_HANDLE fileHandle = StickerDockWidget::createFileHandle(m_mainInterface, m_currentSelectedAnimationFilePath, stickerUserData->imageFilePath);
        Q_ASSERT(fileHandle);
        if (fileHandle) {
            mimeData = new QMimeData;
            mimeData->setData(m_mainInterface->getXMLMimeTypeForDragDrop(), m_mainInterface->getXmlForDragDrop(fileHandle).toUtf8());
            mimeData->setText(QString::number(m_mainInterface->getPlayTime(fileHandle)));
        }
    }

    qDebug()<<"sll-----mimeData---end";

    return mimeData;
}
