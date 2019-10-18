#ifndef HTMLEFFECTITEMMODEL_H
#define HTMLEFFECTITEMMODEL_H
#include "baseitemmodel.h"

struct HtmlEffectUserData
{
    QString strXmlFilePath;
};

class MainInterface;

class HtmlEffectItemModel : public BaseItemModel
{
    Q_OBJECT

public:
    explicit HtmlEffectItemModel(MainInterface *pMainInterface = nullptr, QObject *pParent = nullptr);

    QMimeData *mimeData(const QModelIndexList &indexes) const;

private:
    MainInterface *m_pMainInterface;
};

#endif // HTMLEFFECTITEMMODEL_H
