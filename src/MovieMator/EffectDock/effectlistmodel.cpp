/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: WanYuanCN <ebthon@hotmail.com>
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

#include "effectlistmodel.h"

//#include <maininterface.h>
#include <settings.h>
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include <QMimeData>
#include <util.h>

EffectListModel::EffectListModel(MainInterface *main, QObject *parent) :
    QAbstractItemModel(parent),
    m_mainWindow(main)
{
    m_effectList = new QList<FILE_HANDLE>;
}

EffectListModel::~EffectListModel()
{
    qDeleteAll(*m_effectList);
    m_effectList->clear();
    delete m_effectList;
    m_effectList = nullptr;
}

int EffectListModel::rowCount(const QModelIndex&) const
{
    return m_effectList->count();
}

int EffectListModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant EffectListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.row() < m_effectList->count());

    FILE_HANDLE fileHandle = m_effectList->at(index.row());
    switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            QString result = Util::baseName(m_mainWindow->getFileName(fileHandle));
            return result.split(".")[0];
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
        case Qt::TextAlignmentRole: {   // 设置标题文本对齐方式
            return Qt::AlignLeft;
        }
        default:
            break;
    }
    return QVariant();
}

QMimeData *EffectListModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(indexes.first().row() < m_effectList->count());

    QMimeData *mimeData = new QMimeData;

    FILE_HANDLE fileHandle = m_effectList->at(indexes.first().row());

    mimeData->setData(m_mainWindow->getXMLMimeTypeForDragDrop(), m_mainWindow->getXmlForDragDrop(fileHandle).toUtf8());
    mimeData->setText(QString::number(m_mainWindow->getPlayTime(fileHandle)));

    return mimeData;
}

QModelIndex EffectListModel::index(int row, int column, const QModelIndex&) const
{
    return createIndex(row, column);
}

QModelIndex EffectListModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

void EffectListModel::append(FILE_HANDLE fileHandle)
{
    int count = m_effectList->count();
    beginInsertRows(QModelIndex(), count, count);
    m_effectList->append(fileHandle);
    endInsertRows();
}

FILE_HANDLE EffectListModel::fileAt(int row) const
{
    Q_ASSERT(row >= 0 && row < m_effectList->count());
    return m_effectList->at(row);
}

QImage EffectListModel::thumbnail(int row) const
{
    return m_mainWindow->getThumbnail(m_effectList->at(row));
}
