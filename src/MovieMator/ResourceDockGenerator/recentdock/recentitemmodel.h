#ifndef RECENTITEMMODEL_H
#define RECENTITEMMODEL_H

#include "baseitemmodel.h"
#include "maininterface.h"

struct FileUserData
{
    QString strFilePath;
};

class RecentItemModel : public BaseItemModel
{
    Q_OBJECT

public:
    explicit RecentItemModel(MainInterface *pMainInterface = nullptr, QObject *pParent = nullptr);

    QMimeData *mimeData(const QModelIndexList &indexes) const;

private:
    MainInterface *m_pMainInterface;
};

#endif // RECENTITEMMODEL_H
