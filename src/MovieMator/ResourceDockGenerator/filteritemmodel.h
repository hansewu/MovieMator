#ifndef FILTERITEMMODEL_H
#define FILTERITEMMODEL_H

#include "baseitemmodel.h"

struct FilterUserData
{
    int nFilterIndex;
    QString strPerviewFilePath;
};

class FilterItemModel : public BaseItemModel
{
    Q_OBJECT

public:
    explicit FilterItemModel(QObject *pParent = nullptr);

    QMimeData *mimeData(const QModelIndexList &indexes) const;
};

#endif // FILTERITEMMODEL_H
