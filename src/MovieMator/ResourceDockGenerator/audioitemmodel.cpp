#include "audioitemmodel.h"
#include "maininterface.h"

#include <qmimedata.h>
#include <qdebug.h>

AudioItemModel::AudioItemModel(MainInterface *pMainInterface, QObject *pParent) :
    BaseItemModel(pParent),
    m_pMainInterface(pMainInterface)
{
}

QMimeData *AudioItemModel::mimeData(const QModelIndexList &indexes) const
{
    QModelIndex index                   = indexes.first();
    QAbstractItemModel *pItemModel      = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *pStandardItemModel   = static_cast<BaseItemModel *>(pItemModel);
    QStandardItem *pStandardItem        = pStandardItemModel->itemFromIndex(index);

    QVariant userDataVariant    = pStandardItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    AudioUserData *pTextUserData = reinterpret_cast<AudioUserData *>(userByteArray.data());

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pTextUserData->strXmlFilePath);
    QMimeData *pMimeData = new QMimeData;
    if (fileHandel) {
         pMimeData->setData(m_pMainInterface->getXMLMimeTypeForDragDrop(),
                           m_pMainInterface->getXmlForDragDrop(fileHandel).toUtf8());
         pMimeData->setText(QString::number(m_pMainInterface->getPlayTime(fileHandel)));


        m_pMainInterface->destroyFileHandle(fileHandel);
    }

    return pMimeData;
}
