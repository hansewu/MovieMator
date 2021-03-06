/*
 * Copyright (c) 2014-2016 Meltytech, LLC
 * Author: Brian Matherly <code@brianmatherly.com>
  *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: wyl <wyl@pylwyl.local>
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

#include "metadatamodel.h"
#include "qmlmetadata.h"
#include "settings.h"
#include <Logger.h>

MetadataModel::MetadataModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_metadataFilterType(FavoritesFilter)
    , m_isClipProducer(true)
{
}

int MetadataModel::rowCount(const QModelIndex&) const
{
    return m_list.size();
}

QVariant MetadataModel::data(const QModelIndex &index, int role) const
{
    QVariant result;

    int metaCount = m_list.count();
    Q_ASSERT(index.row() < metaCount);

    QmlMetadata* meta = m_list.at(index.row());
    Q_ASSERT(meta);
    if(meta) {
        switch(role) {
            case Qt::DisplayRole:
            case NameRole:
                result = meta->name();
                break;
            case HiddenRole:
                result = meta->isHidden();
                break;
            case FavoriteRole:
                result = meta->isFavorite();
                break;
            case ServiceRole:
                result = meta->mlt_service();
                break;
            case IsAudioRole:
                result = meta->isAudio();
                break;
            case NeedsGpuRole:
                result = meta->needsGPU();
                break;
            case VisibleRole:
                result = isVisible(index.row());
                break;
            case ThumbnailRole:
                result = meta->thumbnail();
                break;
            case NeedsProVersionRole:
                result = meta->needsProVersion();
                break;
            case FreeVersionRole:
                result = meta->freeVersion();
                break;
            case FilterTypeDisplayRole:
                result = meta->filterType();
                break;
        }
    }

    return result;
}

bool MetadataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()) return false;

    int metaCount = m_list.count();
    Q_ASSERT(index.row() < metaCount);

    switch(role) {
        case FavoriteRole:
        {
            QmlMetadata* meta = m_list.at(index.row());
            Q_ASSERT(meta);
            if(!meta) break;
            meta->setIsFavorite(value.value<bool>());
            emit(dataChanged(index, index));
            break;
        }
    }
    return true;
}

QHash<int, QByteArray> MetadataModel::roleNames() const {
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[NameRole] = "name";
    roles[HiddenRole] = "hidden";
    roles[FavoriteRole] = "favorite";
    roles[ServiceRole] = "service";
    roles[IsAudioRole] = "isAudio";
    roles[NeedsGpuRole] = "needsGpu";
    roles[VisibleRole] = "isVisible";
    roles[ThumbnailRole] = "thumbnail";
    roles[NeedsProVersionRole] = "needsProVersion";
    roles[FreeVersionRole] = "freeVersion";
    roles[FilterTypeDisplayRole] = "filterTypeDisplay";

    return roles;
}

Qt::ItemFlags MetadataModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

void MetadataModel::add(QmlMetadata* data)
{
    Q_ASSERT(data);
    if(!data) return;

    int i = 0;
//    for( i = 0; i < m_list.size(); i++ ) {
//        if (m_list[i]->name().toLower() > data->name().toLower() ) {
//            break;
//        }
//    }

    for( i = 0; i < m_list.size(); i++ ) {
        if (m_list[i]->filterType().toLower() > data->filterType().toLower() ) {
            break;
        }
        if (m_list[i]->filterType().toLower() == data->filterType().toLower() )
        {
            if (m_list[i]->name().toLower() > data->name().toLower() ) {
                break;
            }
        }
    }

    beginInsertRows(QModelIndex(), i, i);
    m_list.insert(i, data);
    endInsertRows();

    data->setParent(this);
}

QmlMetadata* MetadataModel::get(int index) const
{
    if( index < m_list.size() ) {
        return m_list[index];
    }
    return nullptr;
}

void MetadataModel::setMetadataFilterType(MetadataFilterType type)
{
    beginResetModel();
    m_metadataFilterType = type;
    emit metadataFilterTypeChanged();
    endResetModel();
}

bool MetadataModel::isVisible(int row) const
{
    QmlMetadata* meta = m_list.at(row);
    Q_ASSERT(meta);
    if(!meta) return false;

    if (meta->isHidden()) return false;
    if (meta->needsGPU() && !Settings.playerGPU()) return false;
    if (!meta->needsGPU() && Settings.playerGPU() && !meta->gpuAlt().isEmpty()) return false;
    if (m_metadataFilterType == FavoritesFilter && !meta->isFavorite()) return false;
    if (m_metadataFilterType == AudioFilter && !meta->isAudio()) return false;
    if (m_metadataFilterType == VideoFilter && meta->isAudio()) return false;
    if (meta->isClipOnly() && !m_isClipProducer) return false;
    return true;
}

void MetadataModel::setIsClipProducer(bool isClipProducer)
{
    if (m_isClipProducer == isClipProducer) return;

    beginResetModel();
    m_isClipProducer = isClipProducer;
    endResetModel();
}
