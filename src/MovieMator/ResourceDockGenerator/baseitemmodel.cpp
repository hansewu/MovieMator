#include "baseitemmodel.h"

#include<qdebug.h>

BaseItemModel::BaseItemModel(QObject *pParent)
    : QStandardItemModel(pParent)
{
    qDebug()<<"sll-----BaseItemModel构造---start";
    qDebug()<<"sll-----BaseItemModel构造---end";
}

QMimeData *BaseItemModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug()<<"sll-----mimeData---start";
    qDebug()<<"sll-----mimeData---end";
    return QStandardItemModel::mimeData(indexes);
}

QVariant BaseItemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole)
    {
        //设置listview中的item上的文字左对齐
        return Qt::AlignLeft;//设置listview中的item上的文字左对齐
    }
    else
    {
        return QStandardItemModel::data(index, role);
    }
}
