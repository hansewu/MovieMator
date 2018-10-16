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
