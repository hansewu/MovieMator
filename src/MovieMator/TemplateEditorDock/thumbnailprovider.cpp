#include "thumbnailprovider.h"

#include <Logger.h>

ThumbnailProvider::ThumbnailProvider(MainInterface *main)
    : QQuickImageProvider(QQuickImageProvider::Image)
    , m_mainWindow(main)
{

}

QImage ThumbnailProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);
    int width = 100;
    int height = 56;

    if (size)
       *size = QSize(width, height);
    // id用来传递文件路径
    // 文件保存：image://thumbnail/文件路径
    QString decodedId = QUrl::fromPercentEncoding(id.toUtf8());
    return m_mainWindow->getThumbnail(m_mainWindow->openFile(decodedId)).scaled(width, height);
}

