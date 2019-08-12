#include "stickeritemmodel.h"
#include "stickerdockwidget.h"

#include <qdebug.h>
#include <qmimedata.h>

StickerItemModel::StickerItemModel(MainInterface *pMainInterface, QObject *pParent) :
    BaseItemModel(pParent),
    m_pMainInterface(pMainInterface)
{
}

void StickerItemModel::updatCurrentSelectedAnimationFilePath(QString &strAnimationPath)
{
    m_strCurrentSelectedAnimationFilePath = strAnimationPath;
}

QMimeData *StickerItemModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug()<<"sll-----mimeData---start";
    QModelIndex index                   = indexes.first();
    QAbstractItemModel *pItemModel      = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *pStandardItemModel   = static_cast<BaseItemModel *>(pItemModel);
    QStandardItem *pStandardItem        = pStandardItemModel->itemFromIndex(index);

    QVariant userDataVariant            = pStandardItem->data(Qt::UserRole);
    QByteArray userByteArray            = userDataVariant.value<QByteArray>();
    StickerUserData *pStickerUserData   = reinterpret_cast<StickerUserData *>(userByteArray.data());

    qDebug()<<"sll----imageFilePath = "<<pStickerUserData->strImageFilePath;
    qDebug()<<"sll----animationFilePath = "<<m_strCurrentSelectedAnimationFilePath;

    Q_ASSERT(m_pMainInterface);
    QMimeData *pMimeData = nullptr;
    if (m_pMainInterface) {
        FILE_HANDLE fileHandle = StickerDockWidget::createFileHandle(m_pMainInterface, m_strCurrentSelectedAnimationFilePath, pStickerUserData->strImageFilePath);
        Q_ASSERT(fileHandle);
        if (fileHandle) {
            pMimeData = new QMimeData;
            pMimeData->setData(m_pMainInterface->getXMLMimeTypeForDragDrop(), m_pMainInterface->getXmlForDragDrop(fileHandle).toUtf8());
            pMimeData->setText(QString::number(m_pMainInterface->getPlayTime(fileHandle)));
        }
    }

    qDebug()<<"sll-----mimeData---end";

    return pMimeData;
}
