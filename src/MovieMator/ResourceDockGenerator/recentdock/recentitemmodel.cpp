#include "recentitemmodel.h"

#include <qdebug.h>
#include <qmimedata.h>

RecentItemModel::RecentItemModel(MainInterface *pMainInterface, QObject *pParent) :
    BaseItemModel(pParent),
    m_pMainInterface(pMainInterface)
{

}

QMimeData *RecentItemModel::mimeData(const QModelIndexList &indexes) const
{
    QModelIndex index                   = indexes.first();
    QAbstractItemModel *pItemModel      = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *pStandardItemModel   = static_cast<BaseItemModel *>(pItemModel);
    QStandardItem *pStandardItem        = pStandardItemModel->itemFromIndex(index);

    QVariant userDataVariant            = pStandardItem->data(Qt::UserRole);
    QByteArray userByteArray            = userDataVariant.value<QByteArray>();
    FileUserData *pFileUserData         = reinterpret_cast<FileUserData *>(userByteArray.data());

    Q_ASSERT(m_pMainInterface);
    QMimeData *pMimeData = nullptr;
    if (m_pMainInterface)
    {
        FILE_HANDLE fileHandle = m_pMainInterface->openFile(pFileUserData->strFilePath);
        Q_ASSERT(fileHandle);
        if (fileHandle)
        {
            pMimeData = new QMimeData;
            pMimeData->setData(m_pMainInterface->getXMLMimeTypeForDragDrop(), m_pMainInterface->getXmlForDragDrop(fileHandle).toUtf8());
            pMimeData->setText(QString::number(m_pMainInterface->getPlayTime(fileHandle)));

            m_pMainInterface->destroyFileHandle(fileHandle);
        }
    }

    return pMimeData;
}
