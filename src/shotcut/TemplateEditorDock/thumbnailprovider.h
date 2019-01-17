#ifndef THUMBNAILPROVIDER_H
#define THUMBNAILPROVIDER_H

#include <QQuickImageProvider>

#include "maininterface.h"

class ThumbnailProvider : public QQuickImageProvider
{
public:
    explicit ThumbnailProvider(MainInterface *main=nullptr);

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    MainInterface *m_mainWindow;
};

#endif // THUMBNAILPROVIDER_H
