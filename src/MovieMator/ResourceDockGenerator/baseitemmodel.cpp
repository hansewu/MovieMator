#include "baseitemmodel.h"

#include<qdebug.h>

BaseItemModel::BaseItemModel(QObject *parent)
    : QStandardItemModel(parent){
    qDebug()<<"sll-----BaseItemModel构造---start";
    qDebug()<<"sll-----BaseItemModel构造---end";
}

QMimeData *BaseItemModel::mimeData(const QModelIndexList &indexes) const {
    qDebug()<<"sll-----mimeData---start";
    qDebug()<<"sll-----mimeData---end";
    return QStandardItemModel::mimeData(indexes);
}
