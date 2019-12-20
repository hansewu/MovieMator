#include "filteritemmodel.h"

#include <qdebug.h>
#include <qmimedata.h>

FilterItemModel::FilterItemModel(QObject *pParent) :
    BaseItemModel(pParent)
{
}

QMimeData *FilterItemModel::mimeData(const QModelIndexList &indexes) const
{
    QModelIndex index                   = indexes.first();
    QAbstractItemModel *pItemModel      = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *pStandardItemModel   = static_cast<BaseItemModel *>(pItemModel);
    QStandardItem *pStandardItem        = pStandardItemModel->itemFromIndex(index);

    QVariant userDataVariant        = pStandardItem->data(Qt::UserRole);
    QByteArray userByteArray        = userDataVariant.value<QByteArray>();
    FilterUserData *pFilterUserData = reinterpret_cast<FilterUserData *>(userByteArray.data());

    QMimeData *pMimeData = new QMimeData;
    pMimeData->setData("filter/filterindex", QString::number(pFilterUserData->nFilterIndex).toUtf8());

    return pMimeData;
}
