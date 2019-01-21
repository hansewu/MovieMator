#ifndef EFFECTLISTMODEL_H
#define EFFECTLISTMODEL_H

#include <QImage>
#include <QAbstractItemModel>
#include "maininterface.h"

class EffectListModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    static const int THUMBNAIL_WIDTH = 90;
    static const int THUMBNAIL_HEIGHT = 60;

    explicit EffectListModel(MainInterface *main=nullptr, QObject *parent=nullptr);
    ~EffectListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    void append(FILE_HANDLE fileHandle);
    void clear();

    FILE_HANDLE fileAt(int row) const;
    QImage thumbnail(int row) const;

private:
    QList<FILE_HANDLE> *m_effectList;
    MainInterface *m_mainWindow;
};

#endif // EFFECTLISTMODEL_H
