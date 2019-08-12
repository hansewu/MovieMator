#include "filteritemmodel.h"

#include <qdebug.h>
#include <qmimedata.h>

FilterItemModel::FilterItemModel(QObject *pParent) :
    BaseItemModel(pParent)
{
    qDebug()<<"sll-----FilterItemModel构造---start";
    qDebug()<<"sll-----FilterItemModel构造---end";
}

QMimeData *FilterItemModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug()<<"sll-----mimeData---start";
    QModelIndex index                   = indexes.first();
    QAbstractItemModel *pItemModel      = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *pStandardItemModel   = static_cast<BaseItemModel *>(pItemModel);
    QStandardItem *pStandardItem        = pStandardItemModel->itemFromIndex(index);

    QVariant userDataVariant        = pStandardItem->data(Qt::UserRole);
    QByteArray userByteArray        = userDataVariant.value<QByteArray>();
    FilterUserData *pFilterUserData = reinterpret_cast<FilterUserData *>(userByteArray.data());

    qDebug()<<"sll----nFilterIndex = "<<pFilterUserData->nFilterIndex;

    QMimeData *pMimeData = new QMimeData;
    pMimeData->setData("filter/filterindex", QString::number(pFilterUserData->nFilterIndex).toUtf8());

    qDebug()<<"sll-----mimeData---end";

    return pMimeData;
}
