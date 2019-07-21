#ifndef TEXTITEMMODEL_H
#define TEXTITEMMODEL_H

#include "baseitemmodel.h"

struct TextUserData
{
    QString strXmlFilePath;
};

class MainInterface;

class TextItemModel : public BaseItemModel
{
    Q_OBJECT

public:
    explicit TextItemModel(MainInterface *pMainInterface = nullptr, QObject *pParent = nullptr);

    QMimeData *mimeData(const QModelIndexList &indexes) const;

private:
    MainInterface *m_pMainInterface;
};

#endif // TEXTITEMMODEL_H
