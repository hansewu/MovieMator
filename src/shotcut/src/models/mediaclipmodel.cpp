#include "mediaclipmodel.h"

#include "util.h"
#include "shotcut_mlt_properties.h"
#include <QScopedPointer>

#include "settings.h"
#include "database.h"
#include "mainwindow.h"

MediaClipModel::MediaClipModel()
{

}

MediaClipModel::~MediaClipModel()
{

//    delete m_audiolist;
//    m_audiolist = 0;
//    delete m_imagelist;
//    m_imagelist = 0;
}

void MediaClipModel::setPlaylistModel(PlaylistModel *model)
{
    m_playlistModel = model;

}

QVariant MediaClipModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return QVariant();


    int clipCount = m_videolist.count();
    Q_ASSERT(index.row() < clipCount);

    media_clipInfo* meta = m_videolist.at(index.row());

    Q_ASSERT(meta);

    if(meta)
    {
    switch(role) {
        case Qt::DisplayRole:
            return m_playlistModel->data(index,Qt::DecorationRole);

                return "a";//meta->filename;
//            if (info->producer && info->producer->is_valid())
//                 result = info->producer->get(kShotcutCaptionProperty);
//            if (result.isNull())
//                 result = Util::baseName(QString::fromUtf8(info->resource));
//            if (result == "<producer>" && info->producer && info->producer->is_valid())
//                 result = QString::fromUtf8(info->producer->get("mlt_service"));
//            return result;

     //   case ThumbnailRole:
        case Qt::DecorationRole:
               return QImage("/Users/chinpinghsu/Desktop/1.png");//meta->thumbnail;

//             if (setting == "wide")
//                 image = QImage(width * 2, THUMBNAIL_HEIGHT, QImage::Format_ARGB32_Premultiplied);
//             else if (setting == "tall")
//                 image = QImage(width, THUMBNAIL_HEIGHT * 2, QImage::Format_ARGB32_Premultiplied);
//             else if (setting == "large")
//                 image = QImage(width * 2, THUMBNAIL_HEIGHT * 2, QImage::Format_ARGB32_Premultiplied);
//             else
//                 image = QImage(width, THUMBNAIL_HEIGHT, QImage::Format_ARGB32_Premultiplied);

//             return image;

      //   case DurationRole:

//             if (info->producer && info->producer->is_valid()) {
//                 return info->producer->frames_to_time(info->frame_count);
//             }
//             else
              //   return "";

//         case SizeRole:
//             return "2.0M";
    default :
        return QVariant();

        }
    }
    else
        return QVariant();



}

//bool MediaClipModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    if(!index.isValid()) return false;
//    switch(role) {
//        case FavoriteRole:
//        {
//            QmlMetadata* meta = m_list.at(index.row());
//            meta->setIsFavorite(value.value<bool>());
//            emit(dataChanged(index, index));
//            break;
//        }
//    }
//    return true;
//}


void MediaClipModel::setCurrentMediaType(MediaType type)
{

//    m_currentMediaType = type;
//    switch(type)
//    {
//        case VideoType:
//            m_currentlist = m_videolist;
//            break;
//        case AudioType:
//            m_currentlist = m_audiolist;
//            break;
//        case ImageType:
//            m_currentlist = m_imagelist;
//            break;
//    }

}

int MediaClipModel::rowCount(const QModelIndex&) const
{
    return m_videolist.size();
}

void MediaClipModel::add(media_clipInfo *clip)
{
    Q_ASSERT(clip);

    int i = m_videolist.size();
//    for( i = 0; i < m_videolist.size(); i++ ) {
//        if (m_videolist[i]->name().toLower() > data->name().toLower() ) {
//            break;
//        }
//    }

  //  beginInsertRows(QModelIndex(), i, i);
    m_videolist.insert(i, clip);
 //   endInsertRows();
 //   m_videolist.append(clip);
}
