#ifndef BASEITEMMODEL_H
#define BASEITEMMODEL_H

#include <qstandarditemmodel.h>

class BaseItemModel : public QStandardItemModel {
    Q_OBJECT

public:
    explicit BaseItemModel(QObject *parent = nullptr);

    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // BASEITEMMODEL_H
