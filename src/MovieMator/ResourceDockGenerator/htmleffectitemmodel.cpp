#include "htmleffectitemmodel.h"
#include "maininterface.h"

#include <qmimedata.h>
#include <qdebug.h>

HtmlEffectItemModel::HtmlEffectItemModel(MainInterface *pMainInterface, QObject *pParent) :
    BaseItemModel(pParent),
    m_pMainInterface(pMainInterface)
{

}

QMimeData *HtmlEffectItemModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug()<<"-mimeData---start";
    QModelIndex index                   = indexes.first();
    QAbstractItemModel *pItemModel      = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *pStandardItemModel   = static_cast<BaseItemModel *>(pItemModel);
    QStandardItem *pStandardItem        = pStandardItemModel->itemFromIndex(index);

    QVariant userDataVariant    = pStandardItem->data(Qt::UserRole);
    QByteArray userByteArray    = userDataVariant.value<QByteArray>();
    HtmlEffectUserData *pHtmlEffectUserData = reinterpret_cast<HtmlEffectUserData *>(userByteArray.data());

    qDebug()<<"-xmlFilePath = "<<pHtmlEffectUserData->strXmlFilePath;

    FILE_HANDLE fileHandel = m_pMainInterface->openFile(pHtmlEffectUserData->strXmlFilePath);
    QMimeData *pMimeData = new QMimeData;
    if (fileHandel) {
         pMimeData->setData(m_pMainInterface->getXMLMimeTypeForDragDrop(),
                           m_pMainInterface->getXmlForDragDrop(fileHandel).toUtf8());
         pMimeData->setText(QString::number(m_pMainInterface->getPlayTime(fileHandel)));


        m_pMainInterface->destroyFileHandle(fileHandel);
    }

    return pMimeData;
}

