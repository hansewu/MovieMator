#ifndef RECENTTABLEMODEL_H
#define RECENTTABLEMODEL_H

#include <QAbstractTableModel>
#include <maininterface.h>
class RecentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns {
        COLUMN_INDEX = 0,
        COLUMN_THUMBNAIL,
        COLUMN_RESOURCE,
        COLUMN_DURATION,
        COLUMN_COUNT
    };

    static const int THUMBNAIL_WIDTH = 80;
    static const int THUMBNAIL_HEIGHT = 45;

    explicit RecentTableModel(MainInterface *main = 0, QObject *parent = 0);
    ~RecentTableModel();
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::DropActions supportedDropActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QMimeData *mimeData(const int idx) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    QModelIndex createIndex(int row, int column) const;
    void showThumbnail(int row);
    QImage thumbnail(int row);
    QString fileName(int row) const;

    FILE_HANDLE fileAt(int row) const;

public slots:
    void append(FILE_HANDLE fileHandle);
    void insert(FILE_HANDLE fileHandle, int row);
    void remove(int row);
    void clear();

private:
    QList<FILE_HANDLE> m_recentlist;
    MainInterface *m_mainApp;
};

#endif // RECENTTABLEMODEL_H
