#ifndef TEXTITEMMODEL_H
#define TEXTITEMMODEL_H

#include "baseitemmodel.h"

struct TextUserData {
    QString xmlFilePath;
};

class MainInterface;

class TextItemModel : public BaseItemModel {
    Q_OBJECT

public:
    explicit TextItemModel(MainInterface *main = nullptr, QObject *parent = nullptr);

    QMimeData *mimeData(const QModelIndexList &indexes) const;

private:
    MainInterface *m_mainInterface;
};

#endif // TEXTITEMMODEL_H
