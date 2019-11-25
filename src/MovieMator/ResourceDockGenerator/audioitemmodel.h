#ifndef AUDIOITEMMODEL_H
#define AUDIOITEMMODEL_H

#include "baseitemmodel.h"

struct AudioUserData
{
    QString strXmlFilePath;
};

class MainInterface;

class AudioItemModel : public BaseItemModel
{
    Q_OBJECT

public:
    explicit AudioItemModel(MainInterface *pMainInterface = nullptr, QObject *pParent = nullptr);

    QMimeData *mimeData(const QModelIndexList &indexes) const;

private:
    MainInterface *m_pMainInterface;
};

#endif // AUDIOITEMMODEL_H
