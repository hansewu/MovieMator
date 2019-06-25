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
#include "textmetadatamodel.h"

#include <QIcon>
#include "textmetadatamodel.h"
#include "qmltypes/qmltextmetadata.h"
#include "settings.h"
#include <Logger.h>
#include <QPixmap>


TextMetadataModel::TextMetadataModel()
{
}

int TextMetadataModel::rowCount(const QModelIndex&) const
{
    return m_list.size();
}

//int TextMetadataModel::columnCount(const QModelIndex&) const
//{
//    return 4;
//}

QVariant TextMetadataModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.row() < m_list.size());
    QVariant result;
    QmlMetadata* meta = m_list.at(index.row());
    Q_ASSERT(meta);
    if(meta) {
        switch(role) {
            case Qt::DisplayRole:
            case NameRole:
                result = meta->name();
                break;

        case Qt::DecorationRole:

          //  QString str = QString("../..") + meta->thumbnail();
            result = QIcon(QString("./share/moviemator/qml") + meta->thumbnail());
            break;

        case Qt::StatusTipRole:
            result =  QString::number(index.row());//meta->thumbnail();//QString("../..") +
            break;

        case ThumbnailRole:
                result =  meta->thumbnail(); //QString("../..") +
                break;
        }
    }

    return result;
}

bool TextMetadataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value)
    Q_UNUSED(role)
    if(!index.isValid()) return false;

    return true;
}

QHash<int, QByteArray> TextMetadataModel::roleNames() const {
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[NameRole] = "name";


    roles[ThumbnailRole] = "thumbnail";
    return roles;
}

Qt::ItemFlags TextMetadataModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractListModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;
}

void TextMetadataModel::addTextMetadata(QmlMetadata* data)
{
    Q_ASSERT(data);

    int i = 0;
    for( i = 0; i < m_list.size(); i++ ) {
        if (m_list[i]->name().toLower() > data->name().toLower() ) {
            break;
        }
    }

    beginInsertRows(QModelIndex(), i, i);
    m_list.insert(i, data);
    endInsertRows();

    data->setParent(this);


}

QmlMetadata* TextMetadataModel::get(int index) const
{
    if( index < m_list.size() ) {
        return m_list[index];
    }
    return nullptr;
}

