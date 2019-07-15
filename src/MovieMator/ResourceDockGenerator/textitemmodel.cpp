#include "textitemmodel.h"
#include "maininterface.h"

#include <qmimedata.h>
#include <qdebug.h>

TextItemModel::TextItemModel(MainInterface *main, QObject *parent) :
    BaseItemModel(parent),
    m_mainInterface(main) {
    qDebug()<<"sll-----TextItemModel构造---start";
    qDebug()<<"sll-----TextItemModel构造---end";
}

QMimeData *TextItemModel::mimeData(const QModelIndexList &indexes) const {
    qDebug()<<"sll-----mimeData---start";
    QModelIndex index = indexes.first();
    QAbstractItemModel *itemModel = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *standardItemModel = static_cast<BaseItemModel *>(itemModel);
    QStandardItem *standardItem = standardItemModel->itemFromIndex(index);

    QVariant userDataVariant = standardItem->data(Qt::UserRole);
    QByteArray userByteArray = userDataVariant.value<QByteArray>();
    TextUserData *textUserData = reinterpret_cast<TextUserData *>(userByteArray.data());


    QMimeData *mimeData = new QMimeData;
    mimeData->setData(m_mainInterface->getXMLMimeTypeForDragDrop(), textUserData->xmlFilePath.toUtf8());

    qDebug()<<"sll----xmlFilePath = "<<textUserData->xmlFilePath;

    qDebug()<<"sll-----mimeData---start";

    return mimeData;
}
