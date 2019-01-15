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

#ifndef MEDIACLIPMODEL_H
#define MEDIACLIPMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "mltcontroller.h"
#include "MltPlaylist.h"
#include "models/playlistmodel.h"

class MediaClipModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(MediaType)
public:


    enum MediaType {
        VideoType,
        AudioType,
        ImageType
    };

    struct media_clipInfo
    {

         QImage thumbnail;
         QString filename;
    };

    static const int THUMBNAIL_WIDTH = 80;
    static const int THUMBNAIL_HEIGHT = 45;

    explicit MediaClipModel();
    ~MediaClipModel();

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
//    Q_INVOKABLE int columnCount(const QModelIndex& /*parent*/) const;
    QVariant data(const QModelIndex &index, int role) const;
//    bool setData(const QModelIndex &index, const QVariant &value, int role);
//    QHash<int, QByteArray> roleNames() const;
    void setPlaylistModel(PlaylistModel *model);



public slots:
    void setCurrentMediaType(MediaType type);
    void add(media_clipInfo *clip);

private:
    typedef QList<media_clipInfo *> MediaClipList;
    MediaClipList m_videolist;
    PlaylistModel *m_playlistModel;
//    Mlt::Playlist* m_videolist;
//    Mlt::Playlist* m_audiolist;
//    Mlt::Playlist* m_imagelist;
//    Mlt::Playlist* m_currentlist;

//    MediaType m_currentMediaType;

};

#endif // MEDIACLIPMODEL_H
