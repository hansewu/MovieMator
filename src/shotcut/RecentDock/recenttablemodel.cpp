/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "recenttablemodel.h"
#include <QImage>
#include <QRunnable>
#include <maininterface.h>
#include <settings.h>
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include <QMimeData>
#include <util.h>

RecentTableModel::RecentTableModel(MainInterface *main, QObject *parent)
    : QAbstractTableModel(parent)
    , m_mainApp(main)
{

}

RecentTableModel::~RecentTableModel()
{

}

int RecentTableModel::rowCount(const QModelIndex&) const
{
    return  m_recentlist.count();
}

int RecentTableModel::columnCount(const QModelIndex&) const
{
    return COLUMN_COUNT;
}

QString RecentTableModel::fileName(int row) const
{
    Q_ASSERT(row >= 0 && row < m_recentlist.count());
    return m_mainApp->getFileName(m_recentlist[row]);
}

QVariant RecentTableModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.row() < m_recentlist.count());

    FILE_HANDLE fileHandle = m_recentlist[index.row()];
    switch (role) {
    case Qt::DisplayRole:
    case Qt::ToolTipRole: {

        switch (index.column()) {
        case COLUMN_INDEX:
            return QString::number(index.row() + 1);
        case COLUMN_RESOURCE: {
            QString result =  Util::baseName(m_mainApp->getFileName(fileHandle));
            return result;
        }
        case COLUMN_DURATION:
            return m_mainApp->getDuration(fileHandle);
        default:
            break;
        }
        break;
    }
    case Qt::DecorationRole:
        if (index.column() == COLUMN_THUMBNAIL) {
            int width = THUMBNAIL_WIDTH;
            QString setting = Settings.playlistThumbnails();
            QImage image;

            image = QImage(width, THUMBNAIL_HEIGHT, QImage::Format_ARGB32);

            QImage thumb = m_mainApp->getThumbnail(fileHandle);
            if (!thumb.isNull()) {
                QPainter painter(&image);
                image.fill(QApplication::palette().base().color().rgb());

                // draw the in thumbnail
                QRect rect = thumb.rect();
                if (setting != "large") {
                    rect.setWidth(width);
                    rect.setHeight(THUMBNAIL_HEIGHT);
                }
                painter.drawImage(rect, thumb);

                painter.end();
            }
            else {
                image.fill(QApplication::palette().base().color().rgb());
            }
            return image;
        }
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant RecentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section) {
        case COLUMN_INDEX:
            return tr("#");
        case COLUMN_THUMBNAIL:
            return tr("Thumbnails");
        case COLUMN_RESOURCE:
            return tr("Clip");
        case COLUMN_DURATION:
            return tr("Duration");
        default:
            break;
        }
    }
    return QVariant();
}

Qt::DropActions RecentTableModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

QStringList RecentTableModel::mimeTypes() const
{
    QStringList ls = QAbstractTableModel::mimeTypes();
    ls.append(m_mainApp->getXMLMimeTypeForDragDrop());
    ls.append("text/uri-list");
    return ls;
}

QMimeData *RecentTableModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(indexes.first().row() < m_recentlist.count());

    QMimeData *mimeData = new QMimeData;

    FILE_HANDLE fileHandle = m_recentlist[indexes.first().row()];

    mimeData->setData(m_mainApp->getXMLMimeTypeForDragDrop(), m_mainApp->getXmlForDragDrop(fileHandle).toUtf8());
    mimeData->setText(m_mainApp->getDuration(fileHandle));

    return mimeData;
}

QMimeData *RecentTableModel::mimeData(const int idx) const
{
    Q_ASSERT(idx < m_recentlist.count());

    QMimeData *mimeData = new QMimeData;

    FILE_HANDLE fileHandle = m_recentlist[idx];

    mimeData->setData(m_mainApp->getXMLMimeTypeForDragDrop(), m_mainApp->getXmlForDragDrop(fileHandle).toUtf8());
    mimeData->setText(QString::number(m_mainApp->getPlayTime(fileHandle)));

    return mimeData;
}

bool RecentTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    return false;
}


Qt::ItemFlags RecentTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaults = QAbstractTableModel::flags(index);
    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaults;
    else
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaults;
}

QModelIndex RecentTableModel::createIndex(int row, int column) const
{
    return QAbstractTableModel::createIndex(row, column);
}

void RecentTableModel::remove(int row)
{
    Q_ASSERT(row < m_recentlist.count());
    beginRemoveRows(QModelIndex(), row, row);
    m_recentlist.removeAt(row);
    endRemoveRows();
}

void RecentTableModel::clear()
{
    if (rowCount()) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        m_recentlist.clear();
        endRemoveRows();
    }
}

void RecentTableModel::append(FILE_HANDLE fileHandle)
{
    int count = m_recentlist.count();
    beginInsertRows(QModelIndex(), count, count);
    m_recentlist.append(fileHandle);
    endInsertRows();
}

void RecentTableModel::insert(FILE_HANDLE fileHandle, int row)
{
    beginInsertRows(QModelIndex(), row, row);
    m_recentlist.prepend(fileHandle);
    endInsertRows();
}

void RecentTableModel::showThumbnail(int row)
{
    emit dataChanged(createIndex(row, COLUMN_THUMBNAIL), createIndex(row, COLUMN_THUMBNAIL));
}

QImage RecentTableModel::thumbnail(int row)
{
    Q_ASSERT(row >= 0 && row < m_recentlist.count());
    QModelIndex index = createIndex(row, COLUMN_THUMBNAIL);
    QVariant ret = data(index, Qt::DecorationRole);
    return ret.value<QImage>();
}

FILE_HANDLE RecentTableModel::fileAt(int row) const
{
    Q_ASSERT(row >= 0 && row < m_recentlist.count());
    return m_recentlist[row];
}
