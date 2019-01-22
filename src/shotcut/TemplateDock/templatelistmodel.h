#ifndef TEMPLATELISTMODEL_H
#define TEMPLATELISTMODEL_H

#include <QImage>
#include <QAbstractItemModel>
#include "maininterface.h"

class TemplateListModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    static const int THUMBNAIL_WIDTH = 80;
    static const int THUMBNAIL_HEIGHT = 45;

    explicit TemplateListModel(MainInterface *main=nullptr, QObject *parent=nullptr);
    ~TemplateListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    void append(FILE_HANDLE fileHandle);
    void clear();

    FILE_HANDLE getTemplateFile(int row) const;
    QImage thumbnail(int row) const;

private:
    QList<FILE_HANDLE> *m_templateList;
    MainInterface *m_mainWindow;
};

#endif // TEMPLATELISTMODEL_H
