/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: Dragon-S <15919917852@163.com>
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
    Q_ASSERT(m_recentList);
    if(!m_recentList)
    {
        return 0;
    }
    return m_recentList->count();
}

int RecentListModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant RecentListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(m_recentList);
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.row() >= 0);
    Q_ASSERT(index.row() < m_recentList->count());
    if(!m_recentList || !index.isValid() || (index.row()<0) || (index.row()>=m_recentList->count()))
    {
        return QVariant();
    }

    FILE_HANDLE fileHandle = m_recentList->at(index.row());
    Q_ASSERT(fileHandle);
    if(!fileHandle)
    {
        return QVariant();
    }
    switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            Q_ASSERT(m_mainWindow);
            if(!m_mainWindow)
            {
                return QString();
            }
            QString result = Util::baseName(m_mainWindow->getFileName(fileHandle));
            return result;
        }
        case Qt::DecorationRole: {
            int width = THUMBNAIL_WIDTH;
            int height = THUMBNAIL_HEIGHT;
            QString setting = Settings.playlistThumbnails();
            QImage image;

            image = QImage(width, height, QImage::Format_ARGB32);

            Q_ASSERT(m_mainWindow);
            if(!m_mainWindow)
            {
                return image;
            }

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

                // 如果是音频文件就显示一个音乐的图片
                if(m_mainWindow->getFileType(fileHandle)==FILE_TYPE_AUDIO)
                {
                    painter.drawImage(rect, QImage(":/icons/filters/Audio.png"));
                }
                else
                {
                    painter.drawImage(rect, thumb);
                }

                painter.end();
            }
            else {
                image.fill(QApplication::palette().base().color().rgb());
            }
            return image;
        }
        case Qt::TextAlignmentRole: {
            return Qt::AlignLeft;
        }
        default:
            break;
    }
    return QVariant();
}

QMimeData *RecentListModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(m_recentList);
    Q_ASSERT(indexes.first().isValid());
    Q_ASSERT(indexes.first().row()>=0);
    Q_ASSERT(indexes.first().row()<m_recentList->count());
    if(!m_recentList || !indexes.first().isValid() || (indexes.first().row()<0) || (indexes.first().row()>=m_recentList->count()))
    {
        return nullptr;
    }
    FILE_HANDLE fileHandle = m_recentList->at(indexes.first().row());
    Q_ASSERT(fileHandle);
    Q_ASSERT(m_mainWindow);
    if(!fileHandle || !m_mainWindow)
    {
        return nullptr;
    }

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(m_mainWindow->getXMLMimeTypeForDragDrop(), m_mainWindow->getXmlForDragDrop(fileHandle).toUtf8());
    mimeData->setText(QString::number(m_mainWindow->getPlayTime(fileHandle)));

    return mimeData;
}

QMimeData *RecentListModel::mimeData(const int index) const
{
    Q_ASSERT(m_recentList);
    Q_ASSERT(index>=0);
    Q_ASSERT(index<m_recentList->count());
    if(!m_recentList || (index<0) || (index>=m_recentList->count()))
    {
        return nullptr;
    }
    FILE_HANDLE fileHandle = m_recentList->at(index);
    Q_ASSERT(fileHandle);
    Q_ASSERT(m_mainWindow);
    if(!fileHandle || !m_mainWindow)
    {
        return nullptr;
    }

    QMimeData *mimeData = new QMimeData;
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
    Q_ASSERT(fileHandle);
    Q_ASSERT(m_recentList);
    if(!fileHandle || !m_recentList)
    {
        return;
    }
    int count = m_recentList->count();
    beginInsertRows(QModelIndex(), count, count);
    m_recentList->append(fileHandle);
    endInsertRows();
}

void RecentListModel::insert(FILE_HANDLE fileHandle, int row)
{
    Q_ASSERT(fileHandle);
    Q_ASSERT(m_recentList);
    Q_ASSERT(row>=0);
    Q_ASSERT(row<=m_recentList->count());
    if(!fileHandle || !m_recentList || row<0 || row>m_recentList->count())
    {
        return;
    }
    beginInsertRows(QModelIndex(), row, row);
    m_recentList->prepend(fileHandle);
    endInsertRows();
}

void RecentListModel::remove(int row)
{
    Q_ASSERT(m_recentList);
    Q_ASSERT(row>=0);
    Q_ASSERT(row<m_recentList->count());
    if(!m_recentList || row<0 || row>=m_recentList->count())
    {
        return;
    }
    beginRemoveRows(QModelIndex(), row, row);
    m_recentList->removeAt(row);
    endRemoveRows();
}

void RecentListModel::clear()
{
    Q_ASSERT(m_recentList);
//    Q_ASSERT(rowCount());   // 不需要加，元素个数rowCount()允许为0
    if (m_recentList && rowCount()) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        m_recentList->clear();
        endRemoveRows();
    }
}

FILE_HANDLE RecentListModel::fileAt(int row) const
{
    Q_ASSERT(m_recentList);
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < m_recentList->count());
    if(!m_recentList || row<0 || row>=m_recentList->count())
    {
        return nullptr;
    }
    return m_recentList->at(row);
}

QString RecentListModel::fileName(int row) const
{
    Q_ASSERT(m_recentList);
    Q_ASSERT(row>=0);
    Q_ASSERT(row<m_recentList->count());
    Q_ASSERT(m_mainWindow);
    if(!m_recentList || row<0 || row>=m_recentList->count() || !m_mainWindow)
    {
        return QString();
    }
    return m_mainWindow->getFileName(m_recentList->at(row));
}

QImage RecentListModel::thumbnail(int row) const
{
    Q_ASSERT(m_recentList);
    Q_ASSERT(row>=0);
    Q_ASSERT(row<m_recentList->count());
    Q_ASSERT(m_mainWindow);
    if(!m_recentList || row<0 || row>=m_recentList->count() || !m_mainWindow)
    {
        return QImage();
    }
    return m_mainWindow->getThumbnail(m_recentList->at(row));
}
