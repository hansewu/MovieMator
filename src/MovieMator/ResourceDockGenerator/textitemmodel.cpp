#include "textitemmodel.h"
#include "maininterface.h"

#include <qmimedata.h>
#include <qdebug.h>

TextItemModel::TextItemModel(MainInterface *pMainInterface, QObject *pParent) :
    BaseItemModel(pParent),
    m_pMainInterface(pMainInterface)
{
    qDebug()<<"sll-----TextItemModel构造---start";
    qDebug()<<"sll-----TextItemModel构造---end";
}

QMimeData *TextItemModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug()<<"sll-----mimeData---start";
    QModelIndex index                   = indexes.first();
    QAbstractItemModel *pItemModel      = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *pStandardItemModel   = static_cast<BaseItemModel *>(pItemModel);
    QStandardItem *pStandardItem        = pStandardItemModel->itemFromIndex(index);

    QVariant userDataVariant    = pStandardItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    TextUserData *pTextUserData = reinterpret_cast<TextUserData *>(userByteArray.data());

    qDebug()<<"sll----xmlFilePath = "<<pTextUserData->strXmlFilePath;

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pTextUserData->strXmlFilePath);
    QMimeData *pMimeData = new QMimeData;
    if (fileHandel) {
        m_pMainInterface->setTextTemplateNameOnTimeline(fileHandel, pStandardItem->text());
         pMimeData->setData(m_pMainInterface->getXMLMimeTypeForDragDrop(),
                           m_pMainInterface->getXmlForDragDrop(fileHandel).toUtf8());
         pMimeData->setText(QString::number(m_pMainInterface->getPlayTime(fileHandel)));


        m_pMainInterface->destroyFileHandle(fileHandel);
    }

    return pMimeData;
}
