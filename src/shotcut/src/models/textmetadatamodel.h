#ifndef TEXTMETADATAMODEL_H
#define TEXTMETADATAMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "qmltypes/qmlmetadata.h"
class QmlTextMetaData;


class TextMetadataModel : public QAbstractListModel
{
     Q_OBJECT
public:
    TextMetadataModel();


    enum ModelRoles {
        NameRole = Qt::UserRole + 1,

        ThumbnailRole
    };




    // Implement QAbstractListModel
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
//    Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QHash<int, QByteArray> roleNames() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    // Direct access to QmlMetadata
    void addTextMetadata(QmlMetadata* data);
    Q_INVOKABLE QmlMetadata* get(int index) const;
//    MetadataFilter filter() const { return m_filter; }
 //   void setFilter(MetadataFilter);
//    Q_INVOKABLE bool isVisible(int row) const;
  //  void setIsClipProducer(bool isClipProducer);

//signals:
//    void filterChanged();

private:
    typedef QList<QmlMetadata*> TextMetadataList;
    TextMetadataList m_list;
//    MetadataFilter m_filter;
//    bool m_isClipProducer;
//    QScopedPointer<Mlt::Producer> m_producer;
//    typedef QList<int> IndexMap;
//    IndexMap m_mltIndexMap;
};

#endif // TEXTMETADATAMODEL_H

