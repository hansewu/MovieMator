#ifndef STICKERITEMMODEL_H
#define STICKERITEMMODEL_H

#include "baseitemmodel.h"

struct StickerUserData {
    QString imageFilePath;
};

class MainInterface;

class StickerItemModel : public BaseItemModel
{
    Q_OBJECT

public:
    explicit StickerItemModel(MainInterface *main = nullptr, QObject *parent = nullptr);

    QMimeData *mimeData(const QModelIndexList &indexes) const;

public slots:
    void updatCurrentSelectedAnimationFilePath(QString &animationPath);

private:
    MainInterface *m_mainInterface;
    QString m_currentSelectedAnimationFilePath;
};

#endif // STICKERITEMMODEL_H
