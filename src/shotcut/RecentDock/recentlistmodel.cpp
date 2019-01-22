#include "recentlistmodel.h"

#include <settings.h>
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include <QMimeData>
#include <util.h>

RecentListModel::RecentListModel(MainInterface *main, QObject *parent) :
    QAbstractItemModel(parent),
    m_mainWindow(main)
{
    m_recentList = new QList<FILE_HANDLE>;
}

RecentListModel::~RecentListModel()
{
    qDeleteAll(*m_recentList);
    m_recentList->clear();
    delete m_recentList;
    m_recentList = nullptr;
}

int RecentListModel::rowCount(const QModelIndex&) const
{
    return m_recentList->count();
}

int RecentListModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant RecentListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.row() < m_recentList->count());

    FILE_HANDLE fileHandle = m_recentList->at(index.row());
    switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            QString result = Util::baseName(m_mainWindow->getFileName(fileHandle));
            return result;
        }
        case Qt::DecorationRole: {
            int width = THUMBNAIL_WIDTH;
            int height = THUMBNAIL_HEIGHT;
            QString setting = Settings.playlistThumbnails();
            QImage image;

            image = QImage(width, height, QImage::Format_ARGB32);

            QImage thumb = m_mainWindow->getThumbnail(fileHandle);
            if (!thumb.isNull()) {
                QPainter painter(&image);
                image.fill(QApplication::palette().base().color().rgb());

                // draw the in thumbnail
                QRect rect = thumb.rect();
                if (setting != "large") {
                    rect.setWidth(width);
                    rect.setHeight(height);
                }
                painter.drawImage(rect, thumb);

                painter.end();
            }
            else {
                image.fill(QApplication::palette().base().color().rgb());
            }
            return image;
        }
        default:
            break;
    }
    return QVariant();
}

QMimeData *RecentListModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(indexes.first().row() < m_recentList->count());

    QMimeData *mimeData = new QMimeData;

    FILE_HANDLE fileHandle = m_recentList->at(indexes.first().row());

    mimeData->setData(m_mainWindow->getXMLMimeTypeForDragDrop(), m_mainWindow->getXmlForDragDrop(fileHandle).toUtf8());
    mimeData->setText(QString::number(m_mainWindow->getPlayTime(fileHandle)));

    return mimeData;
}

QModelIndex RecentListModel::index(int row, int column, const QModelIndex&) const
{
    return createIndex(row, column);
}

QModelIndex RecentListModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

void RecentListModel::append(FILE_HANDLE fileHandle)
{
    int count = m_recentList->count();
    beginInsertRows(QModelIndex(), count, count);
    m_recentList->append(fileHandle);
    endInsertRows();
}

void RecentListModel::insert(FILE_HANDLE fileHandle, int row)
{
    beginInsertRows(QModelIndex(), row, row);
    m_recentList->prepend(fileHandle);
    endInsertRows();
}

void RecentListModel::remove(int row)
{
    Q_ASSERT(row>=0 && row<m_recentList->count());
    beginRemoveRows(QModelIndex(), row, row);
    m_recentList->removeAt(row);
    endRemoveRows();
}

void RecentListModel::clear()
{
    if (rowCount()) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        m_recentList->clear();
        endRemoveRows();
    }
}

FILE_HANDLE RecentListModel::fileAt(int row) const
{
    Q_ASSERT(row >= 0 && row < m_recentList->count());
    return m_recentList->at(row);
}

QString RecentListModel::fileName(int row) const
{
    Q_ASSERT(row>=0 && row<m_recentList->count());
    return m_mainWindow->getFileName(m_recentList->at(row));
}

QImage RecentListModel::thumbnail(int row) const
{
    Q_ASSERT(row>=0 && row<m_recentList->count());
    return m_mainWindow->getThumbnail(m_recentList->at(row));
}
