#ifndef STICKERITEMMODEL_H
#define STICKERITEMMODEL_H

#include "baseitemmodel.h"

struct StickerUserData
{
    QString strImageFilePath;
};

class MainInterface;

class StickerItemModel : public BaseItemModel
{
    Q_OBJECT

public:
    explicit StickerItemModel(MainInterface *pMainInterface = nullptr, QObject *pParent = nullptr);

    QMimeData *mimeData(const QModelIndexList &indexes) const;

public slots:
    void updatCurrentSelectedAnimationFilePath(QString &strAnimationPath);

private:
    MainInterface *m_pMainInterface;
    QString       m_strCurrentSelectedAnimationFilePath;
};

#endif // STICKERITEMMODEL_H
