/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
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

#include "multitrackmodel.h"
#include "mltcontroller.h"
#include "mainwindow.h"
#include "settings.h"
#include <playlistdock.h>
#include "util.h"
#include "audiolevelstask.h"
#include "shotcut_mlt_properties.h"
#include <QScopedPointer>
#include <QApplication>
#include <qmath.h>
#include <QTimer>

#include <Logger.h>
#include <QMessageBox>
#include <QtDebug>
#include "../maincontroller.h"

#include "../controllers/filtercontroller.h"

#include "../docks/timelinedock.h"


static const quintptr NO_PARENT_ID = quintptr(-1);
static const char* kShotcutDefaultTransition = "lumaMix";


bool ProducerIsTimewarp( Mlt::Producer* producer )
{
    return QString::fromUtf8(producer->get("mlt_service")) == "timewarp";
}

char* GetFilenameFromProducer( Mlt::Producer* producer )
{
    char* resource = NULL;
    if (ProducerIsTimewarp(producer))
    {
        resource = producer->get("warp_resource");
    }
    else
    {
        resource = producer->get("resource");
    }
    return resource;
}

double GetSpeedFromProducer( Mlt::Producer* producer )
{
    double speed = 1.0;
    if (ProducerIsTimewarp(producer) )
    {
        speed = fabs(producer->get_double("warp_speed"));
    }
    return speed;
}


MultitrackModel::MultitrackModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_tractor(0)
    , m_isMakingTransition(false)
{
    connect(this, SIGNAL(modified()), SLOT(adjustBackgroundDuration()));
    connect(this, SIGNAL(reloadRequested()), SLOT(reload()), Qt::QueuedConnection);
}

MultitrackModel::~MultitrackModel()
{
    delete m_tractor;
    m_tractor = 0;
}

int MultitrackModel::rowCount(const QModelIndex &parent) const
{
    if (!m_tractor)
        return 0;
    if (parent.isValid()) {
        if (parent.internalId() != NO_PARENT_ID)
            return 0;
        int i = m_trackList.at(parent.row()).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));

        Q_ASSERT(track);

        if (track) {
            Mlt::Playlist playlist(*track);
            int n = playlist.count();
//            LOG_DEBUG() << __FUNCTION__ << parent << i << n;
            return n;
        } else {
            return 0;
        }
    }
    return m_trackList.count();
}

int MultitrackModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant MultitrackModel::data(const QModelIndex &index, int role) const
{
    if (!m_tractor || !index.isValid())
        return QVariant();
    if (index.parent().isValid()) {
        // Get data for a clip.
        int count = m_trackList.count();
        Q_ASSERT(index.internalId() < count);

        int i = m_trackList.at(index.internalId()).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
//            LOG_DEBUG() << __FUNCTION__ << index.row();
            QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(index.row()));
            if (info)
            switch (role) {
            case NameRole: {
                QString result;
                if (info->producer && info->producer->is_valid())
                    result = info->producer->get(kShotcutCaptionProperty);
                if (result.isNull())
                    result = Util::baseName(QString::fromUtf8(info->resource));
                if (result == "<producer>" && info->producer && info->producer->is_valid())
                    result = QString::fromUtf8(info->producer->get("mlt_service"));
                return result;
            }
            case ResourceRole:
            case Qt::DisplayRole: {
                QString result = QString::fromUtf8(info->resource);
                if (result == "<producer>" && info->producer
                        && info->producer->is_valid() && info->producer->get("mlt_service"))
                    result = QString::fromUtf8(info->producer->get("mlt_service"));
                return result;
            }
            case ServiceRole:
                if (info->producer && info->producer->is_valid())
                    return QString::fromUtf8(info->producer->get("mlt_service"));
                break;
            case IsBlankRole:
                return playlist.is_blank(index.row());
            case StartRole:
                return info->start;
            case DurationRole:
                return info->frame_count;
            case InPointRole:
                return info->frame_in;
            case OutPointRole:
                return info->frame_out;
            case FramerateRole:
                return info->fps;
            case IsAudioRole:
                return m_trackList[index.internalId()].type == AudioTrackType;
            case IsVideoRole:
                return m_trackList[index.internalId()].type == VideoTrackType;
            case IsFilterRole:
                return m_trackList[index.internalId()].type == FilterTrackType;
            case IsTextRole:
                return m_trackList[index.internalId()].type == TextTrackType;
            case TrackTypeRole:
                return m_trackList[index.internalId()].type;
            case IsDefaultTrackRole:
                return m_trackList[index.internalId()].number == 0;
            case AudioLevelsRole:
                if (info->producer->get_data(kAudioLevelsProperty))
                    return QVariant::fromValue(*((QVariantList*) info->producer->get_data(kAudioLevelsProperty)));
                else
                    return QVariant();
            case FadeInRole: {
                //Q_ASSERT(getFilter("fadeInVolume", info->producer));

                QScopedPointer<Mlt::Filter> filter(getFilter("fadeInVolume", info->producer));
                if (!filter || !filter->is_valid())
                    filter.reset(getFilter("fadeInBrightness", info->producer));
                if (!filter || !filter->is_valid())
                    filter.reset(getFilter("fadeInMovit", info->producer));
                return (filter && filter->is_valid())? filter->get_length() : 0;
            }
            case FadeOutRole: {
             //   Q_ASSERT(getFilter("fadeOutVolume", info->producer));
                QScopedPointer<Mlt::Filter> filter(getFilter("fadeOutVolume", info->producer));
                if (!filter || !filter->is_valid())
                    filter.reset(getFilter("fadeOutBrightness", info->producer));
                if (!filter || !filter->is_valid())
                    filter.reset(getFilter("fadeOutMovit", info->producer));
                return (filter && filter->is_valid())? filter->get_length() : 0;
            }
            case IsTransitionRole:
                return isTransition(playlist, index.row());
            case FileHashRole:
                return MLT.getHash(*info->producer);
            case SpeedRole: {
                double speed = 1.0;
                if (info->producer && info->producer->is_valid()) {
                    if (!qstrcmp("timewarp", info->producer->get("mlt_service")))
                        speed = info->producer->get_double("warp_speed");
                }
                return speed;
            }
            case ThumbnailRole:
                return QString("..") + QString(info->producer->get("thumbnail"));
            case HasFilterRole:
                return hasFilterApplied(info->producer);
            default:
                break;
            }
        }
    }
    else {
        // Get data for a track.
        int i = m_trackList.at(index.row()).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
            switch (role) {
            case NameRole:
            case Qt::DisplayRole:
                return track->get(kTrackNameProperty);
            case DurationRole:
                return playlist.get_playtime();
            case IsMuteRole:
                return playlist.get_int("hide") & 2;
            case IsHiddenRole:
                return playlist.get_int("hide") & 1;
            case IsAudioRole:
                return m_trackList[index.row()].type == AudioTrackType;
            case IsVideoRole:
                return m_trackList[index.row()].type == VideoTrackType;
            case IsFilterRole:
                return m_trackList[index.row()].type == FilterTrackType;
            case IsTextRole:
                return m_trackList[index.row()].type == TextTrackType;
            case TrackTypeRole:
                return m_trackList[index.row()].type;
            case IsDefaultTrackRole:
                return m_trackList[index.row()].number == 0;
            case IsLockedRole:
                return track->get_int(kTrackLockProperty);
            case IsCompositeRole: {
                QScopedPointer<Mlt::Transition> transition(getTransition("frei0r.cairoblend", i));
                if (!transition)
                    transition.reset(getTransition("movit.overlay", i));
                if (transition && transition->is_valid()) {
                    if (!transition->get_int("disable"))
                        return Qt::Checked;
                }
                return Qt::Unchecked;
            }

            default:
                break;
            }
        }
    }
    return QVariant();
}

QModelIndex MultitrackModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0)
        return QModelIndex();
//    LOG_DEBUG() << __FUNCTION__ << row << column << parent;
    QModelIndex result;
    if (parent.isValid()) {
        int trackCount = m_trackList.count();
        Q_ASSERT(parent.row() < trackCount);

        int i = m_trackList.at(parent.row()).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        if (track) {
            Mlt::Playlist playlist((mlt_playlist) track->get_producer());
            if (row < playlist.count())
                result = createIndex(row, column, parent.row());
        }
    } else if (row < m_trackList.count()) {
        result = createIndex(row, column, NO_PARENT_ID);
    }
    return result;
}

QModelIndex MultitrackModel::parent(const QModelIndex &index) const
{
//    LOG_DEBUG() << __FUNCTION__ << index;
    if (!index.isValid() || index.internalId() == NO_PARENT_ID)
        return QModelIndex();
    else
        return createIndex(index.internalId(), 0, NO_PARENT_ID);
}

QHash<int, QByteArray> MultitrackModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ResourceRole] = "resource";
    roles[ServiceRole] = "mlt_service";
    roles[IsBlankRole] = "blank";
    roles[StartRole] = "start";
    roles[DurationRole] = "duration";
    roles[InPointRole] = "in";
    roles[OutPointRole] = "out";
    roles[FramerateRole] = "fps";
    roles[IsMuteRole] = "mute";
    roles[IsHiddenRole] = "hidden";
    roles[IsAudioRole] = "audio";
    roles[AudioLevelsRole] = "audioLevels";
    roles[IsCompositeRole] = "composite";
    roles[IsLockedRole] = "locked";
    roles[FadeInRole] = "fadeIn";
    roles[FadeOutRole] = "fadeOut";
    roles[IsTransitionRole] = "isTransition";
    roles[FileHashRole] = "hash";
    roles[SpeedRole] = "speed";
    roles[IsVideoRole] = "video";
    roles[IsFilterRole] = "filter";
    roles[IsTextRole] = "text";
    roles[IsDefaultTrackRole] = "defaultTrack";
    roles[TrackTypeRole] = "trackType";
    roles[ThumbnailRole] = "thumbnail";
    roles[HasFilterRole] = "hasFilter";
    return roles;
}

void MultitrackModel::setTrackName(int row, const QString &value)
{
    Q_ASSERT(row < m_trackList.size());

    if (row < m_trackList.size()) {
        int i = m_trackList.at(row).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        if (track) {
            track->set(kTrackNameProperty, value.toUtf8().constData());

            QModelIndex modelIndex = index(row, 0);
            QVector<int> roles;
            roles << NameRole;
            emit dataChanged(modelIndex, modelIndex, roles);
            emit modified();
        }
    }
}

void MultitrackModel::setTrackMute(int row, bool mute)
{
    Q_ASSERT(row < m_trackList.size());

    if (row < m_trackList.size()) {
        int i = m_trackList.at(row).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        if (track) {
            int hide = track->get_int("hide");
            if (mute)
                hide |= 2;
            else
                hide ^= 2;
            track->set("hide", hide);

            QModelIndex modelIndex = index(row, 0);
            QVector<int> roles;
            roles << IsMuteRole;
            emit dataChanged(modelIndex, modelIndex, roles);
            emit modified();
        }
    }
}

void MultitrackModel::setTrackHidden(int row, bool hidden)
{
    Q_ASSERT(row < m_trackList.size());

    if (row < m_trackList.size()) {
        int i = m_trackList.at(row).mlt_index;

        Q_ASSERT(m_tractor->track(i));

        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        if (track) {
            int hide = track->get_int("hide");
            if (hidden)
                hide |= 1;
            else
                hide ^= 1;
            track->set("hide", hide);
            MLT.refreshConsumer();

            QModelIndex modelIndex = index(row, 0);
            QVector<int> roles;
            roles << IsHiddenRole;
            emit dataChanged(modelIndex, modelIndex, roles);
            emit modified();
        }
    }
}

void MultitrackModel::setTrackComposite(int row, Qt::CheckState composite)
{
    Q_ASSERT(row < m_trackList.size());

    if (row < m_trackList.size()) {
        int i = m_trackList.at(row).mlt_index;

        Q_ASSERT(getTransition("frei0r.cairoblend", i));

        QScopedPointer<Mlt::Transition> transition(getTransition("frei0r.cairoblend", i));
        if (transition) {
            transition->set("disable", (composite == Qt::Unchecked));
        } else {
            transition.reset(getTransition("movit.overlay", i));
            if (transition)
                transition->set("disable", (composite == Qt::Unchecked));
        }
        MLT.refreshConsumer();

        QModelIndex modelIndex = index(row, 0);
        QVector<int> roles;
        roles << IsCompositeRole;
        emit dataChanged(modelIndex, modelIndex, roles);
        emit modified();
    }
}

void MultitrackModel::setTrackLock(int row, bool lock)
{
    Q_ASSERT(row < m_trackList.size());

    if (row < m_trackList.size()) {
        int i = m_trackList.at(row).mlt_index;

        Q_ASSERT(m_tractor->track(i));

        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        track->set(kTrackLockProperty, lock);

        QModelIndex modelIndex = index(row, 0);
        QVector<int> roles;
        roles << IsLockedRole;
        emit dataChanged(modelIndex, modelIndex, roles);
        emit modified();
    }
}

bool MultitrackModel::trimClipInValid(int trackIndex, int clipIndex, int delta, bool ripple)
{
    Q_ASSERT(trackIndex < m_trackList.size());

    bool result = true;
    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);

        Q_ASSERT(clipIndex < playlist.count());

        QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));

        if (!info || (info->frame_in + delta) < 0 || (info->frame_in + delta) > info->frame_out)
            result = false;
        else if (delta < 0 && clipIndex <= 0)
            result = false;
        else if (!ripple && delta < 0 && clipIndex > 0 && !playlist.is_blank(clipIndex - 1))
            result = false;
        else if (delta > 0 && clipIndex > 0 && isTransition(playlist, clipIndex - 1))
            result = false;
    }
    return result;
}

int MultitrackModel::trimClipIn(int trackIndex, int clipIndex, int delta, bool ripple)
{
 //   clock_t begin, duration;

 //   begin = clock();


    int result = clipIndex;
    QList<int> tracksToRemoveRegionFrom;
    int whereToRemoveRegion = -1;

    for (int i = 0; i < m_trackList.count(); ++i) {
        int mltIndex = m_trackList.at(i).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_tractor->track(mltIndex));
        if (!track)
            continue;

        //when not rippling, never touch the other tracks
        if (trackIndex != i && (!ripple || !Settings.timelineRippleAllTracks()))
            continue;

        if (Settings.timelineRippleAllTracks()) {
            if (track->get_int(kTrackLockProperty))
                continue;

            if (trackIndex != i && ripple) {
                tracksToRemoveRegionFrom << i;
                continue;
            }
        }

        Mlt::Playlist playlist(*track);

        Q_ASSERT(playlist.clip_info(clipIndex));

        QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));

        Q_ASSERT(whereToRemoveRegion == -1);

        whereToRemoveRegion = info->start + delta;

        if (info->frame_in + delta < 0)
            delta = -info->frame_in; // clamp

        int in = info->frame_in + delta;
        int out = info->frame_out;
        playlist.resize_clip(clipIndex, in, out);

        // Adjust all filters that have an explicit duration.
        int n = info->producer->filter_count();
        for (int j = 0; j < n; j++) {
            Mlt::Filter* filter = info->producer->filter(j);
            if (filter && filter->is_valid() && filter->get_length() > 0) {
                if (QString(filter->get(kShotcutFilterProperty)).startsWith("fadeIn")
                        || QString(filter->get("mlt_service")) == "webvfx") {
                    filter->set_in_and_out(in, in + filter->get_length() - 1);
                }
            }
            delete filter;
        }

        QModelIndex modelIndex = createIndex(clipIndex, 0, i);
        QVector<int> roles;
        roles << DurationRole;
        roles << InPointRole;
        emit dataChanged(modelIndex, modelIndex, roles);

//        AudioLevelsTask::start(*info->producer, this, modelIndex);


//        if (!ripple)
        {
            // Adjust left of the clip.
            if (clipIndex > 0 && playlist.is_blank(clipIndex - 1)) {
                int out = playlist.clip_length(clipIndex - 1) + delta - 1;
                if (out < 0) {
    //                LOG_DEBUG() << "remove blank at left";
                    beginRemoveRows(index(i), clipIndex - 1, clipIndex - 1);
                    playlist.remove(clipIndex - 1);
                    endRemoveRows();
                    --result;
                } else {
    //                LOG_DEBUG() << "adjust blank on left to" << out;
                    playlist.resize_clip(clipIndex - 1, 0, out);
        
                    QModelIndex index = createIndex(clipIndex - 1, 0, i);
                    QVector<int> roles;
                    roles << DurationRole;
                    emit dataChanged(index, index, roles);
                }
            } else if (delta > 0) {
    //            LOG_DEBUG() << "add blank on left duration" << delta - 1;
                beginInsertRows(index(i), clipIndex, clipIndex);
                playlist.insert_blank(clipIndex, delta - 1);
                endInsertRows();
                ++result;
            }
        }
        emit modified();
    }
    foreach (int idx, tracksToRemoveRegionFrom) {
        Q_ASSERT(whereToRemoveRegion != -1);
        removeRegion(idx, whereToRemoveRegion - delta, delta);
    }

  //  duration = clock() - begin;
//    printf( "函数f()的运行时间大约为：%dms\n", duration*1000/CLOCKS_PER_SEC );

    //qDebug() << "函数的运行时间大约为：" << duration*1000/CLOCKS_PER_SEC << " ms";

    return result;
}

void MultitrackModel::notifyClipIn(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    if (trackIndex >= 0 && trackIndex < m_trackList.size() && clipIndex >= 0) {
        QModelIndex index = createIndex(clipIndex, 0, trackIndex);
        QVector<int> roles;
        roles << AudioLevelsRole;
        // emit dataChanged(index, index, roles);
        MLT.refreshConsumer();
//        emit dataChanged(index, index, roles);
        // int mltIndex = m_trackList.at(trackIndex).mlt_index;
        // QScopedPointer<Mlt::Producer> track(m_tractor->track(mltIndex));
        // if (!track)
        //     return;
        // Mlt::Playlist playlist(*track);
        // QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));
        // AudioLevelsTask::start(*info->producer, this, index);
    }
    m_isMakingTransition = false;
}

bool MultitrackModel::trimClipOutValid(int trackIndex, int clipIndex, int delta, bool ripple)
{
    bool result = true;

    Q_ASSERT(trackIndex < m_trackList.size());

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));

    if (track) {
        Mlt::Playlist playlist(*track);

        Q_ASSERT(clipIndex < playlist.count());

        QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));
        if (!info || (info->frame_out - delta) >= info->length || (info->frame_out - delta) < info->frame_in)
            result = false;
        else if (!ripple && delta < 0 && (clipIndex + 1) < playlist.count() && !playlist.is_blank(clipIndex + 1))
            result = false;
        else if (delta > 0 && (clipIndex + 1) < playlist.count() && isTransition(playlist, clipIndex + 1))
            return false;
    }
    return result;
}

int MultitrackModel::trackHeight() const
{
    int result = m_tractor? m_tractor->get_int(kTrackHeightProperty) : 30;
    return result? result : 30;
}

void MultitrackModel::setTrackHeight(int height)
{
    if (m_tractor) {
        m_tractor->set(kTrackHeightProperty, height);
        emit trackHeightChanged();
    }
}

double MultitrackModel::scaleFactor() const
{
    double result = m_tractor? m_tractor->get_double(kTimelineScaleProperty) : 0;
    return (result > 0)? result : (qPow(1.0, 3.0) + 0.01);
}

void MultitrackModel::setScaleFactor(double scale)
{
    if (m_tractor) {
        m_tractor->set(kTimelineScaleProperty, scale);
        emit scaleFactorChanged();
    }
}

int MultitrackModel::trimClipOut(int trackIndex, int clipIndex, int delta, bool ripple)
{
    QList<int> tracksToRemoveRegionFrom;
    int result = clipIndex;
    int whereToRemoveRegion = -1;

    for (int i = 0; i < m_trackList.count(); ++i) {
        int mltIndex = m_trackList.at(i).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_tractor->track(mltIndex));
        if (!track)
            continue;

        Mlt::Playlist playlist(*track);
        QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));

        //when not rippling, never touch the other tracks
        if (trackIndex != i && (!ripple || !Settings.timelineRippleAllTracks()))
            continue;

        if (Settings.timelineRippleAllTracks()) {
            if (track->get_int(kTrackLockProperty))
                continue;

            if (trackIndex != i && ripple) {
                tracksToRemoveRegionFrom << i;
                continue;
            }
        }

        Q_ASSERT(whereToRemoveRegion == -1);
        whereToRemoveRegion = info->start + info->frame_count - delta;

        if ((info->frame_out - delta) >= info->length)
            delta = info->frame_out - info->length + 1; // clamp

        if (!ripple) {
            // Adjust right of the clip.
            if (clipIndex >= 0 && (clipIndex + 1) < playlist.count() && playlist.is_blank(clipIndex + 1)) {
                int out = playlist.clip_length(clipIndex + 1) + delta - 1;
                if (out < 0) {
    //                LOG_DEBUG() << "remove blank at right";
                    beginRemoveRows(index(i), clipIndex + 1, clipIndex + 1);
                    playlist.remove(clipIndex + 1);
                    endRemoveRows();
                } else {
    //                LOG_DEBUG() << "adjust blank on right to" << out;
                    playlist.resize_clip(clipIndex + 1, 0, out);
        
                    QModelIndex index = createIndex(clipIndex + 1, 0, i);
                    QVector<int> roles;
                    roles << DurationRole;
                    emit dataChanged(index, index, roles);
                }
            } else if (delta > 0 && (clipIndex + 1) < playlist.count())  {
                // Add blank to right.
    //            LOG_DEBUG() << "add blank on right duration" << (delta - 1);
                int newIndex = clipIndex + 1;
                beginInsertRows(index(i), newIndex, newIndex);
                playlist.insert_blank(newIndex, delta - 1);
                endInsertRows();
            }
        }
        int in = info->frame_in;
        int out = info->frame_out - delta;
        playlist.resize_clip(clipIndex, in, out);

        // Adjust all filters that have an explicit duration.
        int n = info->producer->filter_count();
        for (int j = 0; j < n; j++) {
            Mlt::Filter* filter = info->producer->filter(j);
            if (filter && filter->is_valid() && filter->get_length() > 0) {
                if (QString(filter->get(kShotcutFilterProperty)).startsWith("fadeOut")
                        || QString(filter->get("mlt_service")) == "webvfx") {
                    filter->set_in_and_out(out - filter->get_length() + 1, out);
                }
            }
            delete filter;
        }

        QModelIndex index = createIndex(clipIndex, 0, i);
        QVector<int> roles;
        roles << DurationRole;
        roles << OutPointRole;
        emit dataChanged(index, index, roles);

//        AudioLevelsTask::start(*info->producer, this, index);
        emit modified();
    }
    foreach (int idx, tracksToRemoveRegionFrom) {
        Q_ASSERT(whereToRemoveRegion != -1);
        removeRegion(idx, whereToRemoveRegion, delta);
    }
    return result;
}

void MultitrackModel::notifyClipOut(int trackIndex, int clipIndex)
{
    if (trackIndex >= 0 && trackIndex < m_trackList.size() && clipIndex >= 0) {
        QModelIndex index = createIndex(clipIndex, 0, trackIndex);
        QVector<int> roles;
        roles << AudioLevelsRole;
        // emit dataChanged(index, index, roles);
        MLT.refreshConsumer();
    }
    m_isMakingTransition = false;
}

bool MultitrackModel::moveClipValid(int fromTrack, int toTrack, int clipIndex, int position)
{
    // XXX This is very redundant with moveClip().
    bool result = false;

    Q_ASSERT(toTrack < m_trackList.size());
    Q_ASSERT(fromTrack < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(toTrack).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        int targetIndex = playlist.get_clip_index_at(position);

        if (fromTrack != toTrack) {
            Mlt::Producer* trackFrom = m_tractor->track(m_trackList.at(fromTrack).mlt_index);
            Mlt::Playlist playlistFrom(*trackFrom);
            delete trackFrom;
            if (clipIndex < 0 || clipIndex >= playlistFrom.count())
                return false;
            QScopedPointer<Mlt::Producer> clip(playlistFrom.get_clip(clipIndex));
            if (position >= playlist.get_playtime())
                result = true;
            else if (playlist.is_blank_at(0) && playlist.count() == 1)
                // blank track
                result = true;
            else if (playlist.is_blank_at(position) && playlist.is_blank_at(position + clip->get_playtime() - 1)
                    && playlist.get_clip_index_at(position) == playlist.get_clip_index_at(position + clip->get_playtime() - 1))
                result = true;
            if (!result) {
                QModelIndex parentIndex = index(fromTrack);
                // Remove blank on fromTrack.
                beginRemoveRows(parentIndex, clipIndex, clipIndex);
                playlistFrom.remove(clipIndex);
                endRemoveRows();
        
                // Insert clip on fromTrack.
                beginInsertRows(parentIndex, clipIndex, clipIndex);
                playlistFrom.insert(*clip, clipIndex, clip->get_in(), clip->get_out());
                endInsertRows();
            }
        }
        else if ((clipIndex + 1) < playlist.count() && position >= playlist.get_playtime()) {
            result = true;
        }
        else if ((targetIndex < (clipIndex - 1) || targetIndex > (clipIndex + 1))
            && playlist.is_blank_at(position) && playlist.clip_length(clipIndex) <= playlist.clip_length(targetIndex)) {
            result = true;
        }
        else if (targetIndex >= (clipIndex - 1) && targetIndex <= (clipIndex + 1)) {
            int length = playlist.clip_length(clipIndex);
            int targetIndexEnd = playlist.get_clip_index_at(position + length - 1);

            if ((playlist.is_blank_at(position) || targetIndex == clipIndex)
                && (playlist.is_blank_at(position + length - 1) || targetIndexEnd == clipIndex)) {

                result = true;
            }
        }
    }
    return result;
}

bool MultitrackModel::moveClip(int fromTrack, int toTrack, int clipIndex, int position)
{
    LOG_DEBUG() << __FUNCTION__ << clipIndex << "fromTrack" << fromTrack << "toTrack" << toTrack;
    bool result = false;
    Q_ASSERT(fromTrack < m_trackList.size());
    Q_ASSERT(toTrack < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(toTrack).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        int targetIndex = playlist.get_clip_index_at(position);

        if (fromTrack != toTrack) {
//            qDebug()<<"1";
            result = moveClipToTrack(fromTrack, toTrack, clipIndex, position);
        }
        else if ((clipIndex + 1) < playlist.count() && position >= playlist.get_playtime()) {
            // Clip relocated to end of playlist.
//            qDebug()<<"2";
            moveClipToEnd(playlist, toTrack, clipIndex, position);
            result = true;
        }
        else if ((targetIndex < (clipIndex - 1) || targetIndex > (clipIndex + 1))
            && playlist.is_blank_at(position) && playlist.clip_length(clipIndex) <= playlist.clip_length(targetIndex)) {
            // Relocate clip.
//            qDebug()<<"3";
            relocateClip(playlist, toTrack, clipIndex, position);
            result = true;
        }
        else if (targetIndex >= (clipIndex - 1) && targetIndex <= (clipIndex + 1)) {
            int length = playlist.clip_length(clipIndex);
            int targetIndexEnd = playlist.get_clip_index_at(position + length - 1);

            if ((playlist.is_blank_at(position) || targetIndex == clipIndex)
                && (playlist.is_blank_at(position + length - 1) || targetIndexEnd == clipIndex)) {

                if (position < 0) {
                    // Special case: dragged left of timeline origin.
//                    qDebug()<<"4";
                    Mlt::ClipInfo* info = playlist.clip_info(clipIndex);

                    Q_ASSERT(info);

                    playlist.resize_clip(clipIndex, info->frame_in - position, info->frame_out);
                    delete info;
                    QModelIndex idx = createIndex(clipIndex, 0, toTrack);
                    QVector<int> roles;
                    roles << DurationRole;
                    roles << InPointRole;
                    emit dataChanged(idx, idx, roles);
                    if (clipIndex > 0) {
                        QModelIndex parentIndex = index(toTrack);
                        beginMoveRows(parentIndex, clipIndex, clipIndex, parentIndex, 0);
                        playlist.move(clipIndex, 0);
                        endMoveRows();
                        consolidateBlanks(playlist, toTrack);
                        clipIndex = 0;
                    }
                }
                // Reposition the clip within its current blank spot.
//                qDebug()<<"5";
                moveClipInBlank(playlist, toTrack, clipIndex, position);
                result = true;
            }
        }
    }
    if (result) {
//        qDebug()<<"6";
        emit modified();
        MLT.refreshConsumer();
    }
    return result;
}

int MultitrackModel::overwriteClip(int trackIndex, Mlt::Producer& clip, int position, bool seek)
{
    if (checkClip(clip) == false)
        return -1;
    createIfNeeded();
    int result = -1;

    Q_ASSERT(trackIndex < m_trackList.size());

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        if (position >= playlist.get_playtime() - 1) {
//            LOG_DEBUG() << __FUNCTION__ << "appending";
            removeBlankPlaceholder(playlist, trackIndex);
            int n = playlist.count();
            int length = position - playlist.clip_start(n - 1) - playlist.clip_length(n - 1);
    
            // Add blank to end if needed.
            if (length > 0) {
                beginInsertRows(index(trackIndex), n, n);
                playlist.blank(length - 1);
                endInsertRows();
                ++n;
            }
    
            // Append clip.
            int in = clip.get_in();
            int out = clip.get_out();
            clip.set_in_and_out(0, clip.get_length() - 1);
            beginInsertRows(index(trackIndex), n, n);
            playlist.append(clip.parent(), in, out);
            endInsertRows();
            AudioLevelsTask::start(clip.parent(), this, createIndex(n, 0, trackIndex));
            result = playlist.count() - 1;
        } else if (position + clip.get_playtime() > playlist.get_playtime()
            // Handle straddling - new clip larger than another with blanks on both sides.
            || playlist.get_clip_index_at(position) == playlist.get_clip_index_at(position + clip.get_playtime() - 1)) {
//            LOG_DEBUG() << __FUNCTION__ << "overwriting blank space" << clip.get_playtime();
            int targetIndex = playlist.get_clip_index_at(position);

            if (position > playlist.clip_start(targetIndex)) {
                splitClip(trackIndex, targetIndex, position);
            
                // Notify item on left was adjusted.
                QModelIndex modelIndex = createIndex(targetIndex, 0, trackIndex);
                QVector<int> roles;
                roles << DurationRole;
                emit dataChanged(modelIndex, modelIndex, roles);
                AudioLevelsTask::start(clip.parent(), this, modelIndex);
                ++targetIndex;
            } else if (position < 0) {
                clip.set_in_and_out(-position, clip.get_out());
                QModelIndex modelIndex = createIndex(targetIndex, 0, trackIndex);
                // Notify clip on right was adjusted.
                QVector<int> roles;
                roles << InPointRole;
                roles << DurationRole;
                emit dataChanged(modelIndex, modelIndex, roles);
            }
        
            // Adjust clip on right.
            int duration = playlist.clip_length(targetIndex) - clip.get_playtime();
            if (duration > 0) {
//                LOG_DEBUG() << "adjust item on right" << (targetIndex) << " to" << duration;
                playlist.resize_clip(targetIndex, 0, duration - 1);
                QModelIndex modelIndex = createIndex(targetIndex, 0, trackIndex);
                // Notify clip on right was adjusted.
                QVector<int> roles;
                roles << DurationRole;
                emit dataChanged(modelIndex, modelIndex, roles);
                AudioLevelsTask::start(clip.parent(), this, modelIndex);
            } else {
//                LOG_DEBUG() << "remove item on right";
                beginRemoveRows(index(trackIndex), targetIndex, targetIndex);
                playlist.remove(targetIndex);
                endRemoveRows();
            }
            // Insert clip between subclips.
            int in = clip.get_in();
            int out = clip.get_out();
            clip.set_in_and_out(0, clip.get_length() - 1);
            beginInsertRows(index(trackIndex), targetIndex, targetIndex);
            playlist.insert(clip.parent(), targetIndex, in, out);
            endInsertRows();
            result = targetIndex;
        }
        if (result >= 0) {
            QModelIndex index = createIndex(result, 0, trackIndex);
            AudioLevelsTask::start(clip.parent(), this, index);
            emit modified();
            if (seek)
            {
//                qDebug()<<"20";
                emit seeked(playlist.clip_start(result) /*+ playlist.clip_length(result)*/);
            }
        }
    }
    return result;
}

QString MultitrackModel::overwrite(int trackIndex, Mlt::Producer& clip, int position, bool seek)
{
    Q_ASSERT(checkClip(clip));
    Q_ASSERT(trackIndex < m_trackList.size());

    if (checkClip(clip) == false)
        return QString();



    createIfNeeded();
    Mlt::Playlist result;
    int i = m_trackList.at(trackIndex).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        removeBlankPlaceholder(playlist, trackIndex);
        int targetIndex = playlist.get_clip_index_at(position);
        if (position >= playlist.get_playtime() - 1) {
//            LOG_DEBUG() << __FUNCTION__ << "appending";
            int n = playlist.count();
            int length = position - playlist.clip_start(n - 1) - playlist.clip_length(n - 1);
    
            // Add blank to end if needed.
            if (length > 0) {
                beginInsertRows(index(trackIndex), n, n);
                playlist.blank(length - 1);
                endInsertRows();
                ++n;
            }
    
            // Append clip.
            int in = clip.get_in();
            int out = clip.get_out();
            clip.set_in_and_out(0, clip.get_length() - 1);
            beginInsertRows(index(trackIndex), n, n);
            playlist.append(clip.parent(), in, out);
            endInsertRows();
            targetIndex = playlist.count() - 1;
        } else {
            int lastIndex = playlist.get_clip_index_at(position + clip.get_playtime());
//            LOG_DEBUG() << __FUNCTION__ << "overwriting with duration" << clip.get_playtime()
//                << "from" << targetIndex << "to" << lastIndex;

            // Add affected clips to result playlist.
            int i = targetIndex;
            if (position == playlist.clip_start(targetIndex))
                --i;
            for (; i <= lastIndex; i++) {
                Mlt::Producer* producer = playlist.get_clip(i);

                Q_ASSERT(producer);

                if (producer)
                    result.append(*producer);
                delete producer;
            }

            if (position > playlist.clip_start(targetIndex)) {
//                LOG_DEBUG() << "split starting item" <<  targetIndex;
                splitClip(trackIndex, targetIndex, position);
                ++targetIndex;
            } else if (position < 0) {
                clip.set_in_and_out(-position, clip.get_out());
                QModelIndex modelIndex = createIndex(targetIndex, 0, trackIndex);
                // Notify clip was adjusted.
                QVector<int> roles;
                roles << InPointRole;
                roles << DurationRole;
                emit dataChanged(modelIndex, modelIndex, roles);
            }

            int length = clip.get_playtime();
            while (length > 0 && targetIndex < playlist.count()) {
                if (playlist.clip_length(targetIndex) > length) {
//                    LOG_DEBUG() << "split last item" << targetIndex;
                    splitClip(trackIndex, targetIndex, position + length);
                }
//                LOG_DEBUG() << "length" << length << "item length" << playlist.clip_length(targetIndex);
                length -= playlist.clip_length(targetIndex);
//                LOG_DEBUG() << "delete item" << targetIndex;
                beginRemoveRows(index(trackIndex), targetIndex, targetIndex);
                playlist.remove(targetIndex);
                endRemoveRows();
            }

            // Insert clip between subclips.
            int in = clip.get_in();
            int out = clip.get_out();
            clip.set_in_and_out(0, clip.get_length() - 1);
            beginInsertRows(index(trackIndex), targetIndex, targetIndex);
            playlist.insert(clip.parent(), targetIndex, in, out);
            endInsertRows();
        }
        QModelIndex index = createIndex(targetIndex, 0, trackIndex);
        AudioLevelsTask::start(clip.parent(), this, index);
        emit modified();
        if (seek)
            emit seeked(playlist.clip_start(targetIndex));
    }
    return MLT.XML(&result);
}

int MultitrackModel::insertClip(int trackIndex, Mlt::Producer &clip, int position)
{
    Q_ASSERT(checkClip(clip));
    Q_ASSERT(trackIndex < m_trackList.size());

    if (checkClip(clip) == false)
        return -1;

    createIfNeeded();
    int result = -1;
    int i = m_trackList.at(trackIndex).mlt_index;
    int clipPlaytime = clip.get_playtime();

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        if (position < 0 || position >= playlist.get_playtime() - 1) {
            LOG_DEBUG() << __FUNCTION__ << "appending";
            removeBlankPlaceholder(playlist, trackIndex);
            int n = playlist.count();
            int length = position - playlist.clip_start(n - 1) - playlist.clip_length(n - 1);
    
            // Add blank to end if needed.
            if (length > 0) {
                beginInsertRows(index(trackIndex), n, n);
                playlist.blank(length - 1);
                endInsertRows();
                ++n;
            }
    
            // Append clip.
            int in = clip.get_in();
            int out = clip.get_out();
            clip.set_in_and_out(0, clip.get_length() - 1);
            beginInsertRows(index(trackIndex), n, n);
            playlist.append(clip.parent(), in, out);
            endInsertRows();
            result = playlist.count() - 1;
        } else {
//            LOG_DEBUG() << __FUNCTION__ << "inserting" << position << MLT.XML(&clip);
          //  LOG_DEBUG() << __FUNCTION__ << "inserting" << position << MLT.XML(&clip); xjp delete,
            int targetIndex = playlist.get_clip_index_at(position);
        
            if (position > playlist.clip_start(targetIndex)) {
                splitClip(trackIndex, targetIndex, position);
        
                // Notify item on left was adjusted.
                QModelIndex modelIndex = createIndex(targetIndex, 0, trackIndex);
                QVector<int> roles;
                roles << DurationRole;
                emit dataChanged(modelIndex, modelIndex, roles);
                AudioLevelsTask::start(clip.parent(), this, modelIndex);
                ++targetIndex;

                // Notify item on right was adjusted.
                modelIndex = createIndex(targetIndex, 0, trackIndex);
                emit dataChanged(modelIndex, modelIndex, roles);
                AudioLevelsTask::start(clip.parent(), this, modelIndex);
            }

            // Insert clip between split blanks.
            beginInsertRows(index(trackIndex), targetIndex, targetIndex);
            if (qstrcmp("blank", clip.get("mlt_service"))) {
                int in = clip.get_in();
                int out = clip.get_out();
                clip.set_in_and_out(0, clip.get_length() - 1);
                playlist.insert(clip.parent(), targetIndex, in, out);
            } else {
                playlist.insert_blank(targetIndex, clipPlaytime - 1);
            }
            endInsertRows();
            result = targetIndex;
        }
        if (result >= 0) {
            if (Settings.timelineRippleAllTracks()) {
                //fill in/expand blanks in all the other tracks
                QList<int> tracksToInsertBlankInto;
                for (int j = 0; j < m_trackList.count(); ++j) {
                    if (j == trackIndex)
                        continue;
                    int mltIndex = m_trackList.at(j).mlt_index;
                    QScopedPointer<Mlt::Producer> otherTrack(m_tractor->track(mltIndex));
                    if (otherTrack->get_int(kTrackLockProperty))
                        continue;

                    tracksToInsertBlankInto << j;
                }
                if (!tracksToInsertBlankInto.isEmpty())
                    insertOrAdjustBlankAt(tracksToInsertBlankInto, position, clipPlaytime);
            }

            QModelIndex index = createIndex(result, 0, trackIndex);
            AudioLevelsTask::start(clip.parent(), this, index);
            emit modified();
            emit seeked(playlist.clip_start(result));
        }
    }
    return result;
}

int MultitrackModel::appendClip(int trackIndex, Mlt::Producer &clip)
{
    Q_ASSERT(trackIndex < m_trackList.size());


    if (checkClip(clip) == false)
        return -1;
    if (!createIfNeeded()) {
        return -1;
    }
    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        removeBlankPlaceholder(playlist, trackIndex);
        i = playlist.count();
        int in = clip.get_in();
        int out = clip.get_out();
        clip.set_in_and_out(0, clip.get_length() - 1);
        beginInsertRows(index(trackIndex), i, i);
        playlist.append(clip.parent(), in, out);
        endInsertRows();
        QModelIndex index = createIndex(i, 0, trackIndex);
        AudioLevelsTask::start(clip.parent(), this, index);
        emit modified();
        emit seeked(playlist.clip_start(i));
        return i;
    }
    return -1;
}

void MultitrackModel::removeClip(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex < m_trackList.size());


    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    int clipPlaytime = -1;
    int clipStart = -1;

    if (track) {
        Mlt::Playlist playlist(*track);
        if (clipIndex < playlist.count()) {
            // Shotcut does not like the behavior of remove() on a
            // transition (MLT mix clip). So, we null mlt_mix to prevent it.
            clearMixReferences(trackIndex, clipIndex);

            Q_ASSERT(playlist.get_clip(clipIndex));
            Q_ASSERT(clipIndex < playlist.count());

            QScopedPointer<Mlt::Producer> producer(playlist.get_clip(clipIndex));
            if (producer) {
                clipPlaytime = producer->get_playtime();
                clipStart = playlist.clip_start(clipIndex);
            }

//#ifdef Q_OS_MAC
//            // XXX Remove this when Qt is upgraded to > 5.2.
//            // Workaround a crash bug choosing Remove from CLip/blank context menu.
//            playlist.remove(clipIndex);
//            // Consolidate blanks
//            for (int i = 1; i < playlist.count(); i++) {
//                if (playlist.is_blank(i - 1) && playlist.is_blank(i)) {
//                    int out = playlist.clip_length(i - 1) + playlist.clip_length(i) - 1;
//                    playlist.resize_clip(i - 1, 0, out);
//                    QModelIndex idx = createIndex(i - 1, 0, trackIndex);
//                    QVector<int> roles;
//                    roles << DurationRole;
//                    emit dataChanged(idx, idx, roles);
//                    playlist.remove(i--);
//                }
//                if (playlist.count() > 0) {
//                    int i = playlist.count() - 1;
//                    if (playlist.is_blank(i)) {
//                        playlist.remove(i);
//                    }
//                }
//            }
//            if (playlist.count() == 0) {
//                playlist.blank(0);
//            }
//            QTimer::singleShot(10, this, SLOT(reload()));
//#else
            beginRemoveRows(index(trackIndex), clipIndex, clipIndex);
            playlist.remove(clipIndex);
            endRemoveRows();
            consolidateBlanks(playlist, trackIndex);
//#endif
            // Ripple all unlocked tracks.
            if (clipPlaytime > 0 && Settings.timelineRippleAllTracks())
            for (int j = 0; j < m_trackList.count(); ++j) {
                if (j == trackIndex)
                    continue;

                int mltIndex = m_trackList.at(j).mlt_index;
                QScopedPointer<Mlt::Producer> otherTrack(m_tractor->track(mltIndex));
                if (otherTrack) {
                    if (otherTrack->get_int(kTrackLockProperty))
                        continue;

                    removeRegion(j, clipStart, clipPlaytime);
                }
            }
            emit modified();
        }
    }
}

void MultitrackModel::liftClip(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex < m_trackList.size());

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        if (clipIndex < playlist.count()) {
            // Shotcut does not like the behavior of replace_with_blank() on a
            // transition (MLT mix clip). So, we null mlt_mix to prevent it.


//            Mlt::Producer *transClip = playlist.get_clip(clipIndex);
//            mlt_properties_debug(transClip->parent().get_properties(), "123456------------", stderr);

            clearMixReferences(trackIndex, clipIndex);

            playlist.replace_with_blank(clipIndex);

            QModelIndex index = createIndex(clipIndex, 0, trackIndex);
            QVector<int> roles;
            roles << ResourceRole;
            roles << ServiceRole;
            roles << IsBlankRole;
            roles << IsTransitionRole;
            emit dataChanged(index, index, roles);

            consolidateBlanks(playlist, trackIndex);

            emit modified();
        }
    }
}

void MultitrackModel::splitClip(int trackIndex, int clipIndex, int position)
{
    Q_ASSERT(trackIndex < m_trackList.size());

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        QScopedPointer<Mlt::Producer> clip(playlist.get_clip(clipIndex));

        // Make copy of clip.
        Mlt::Producer producer(MLT.profile(), "xml-string",
            MLT.XML(&clip->parent()).toUtf8().constData());
        int in = clip->get_in();
        int out = clip->get_out();
        int duration = position - playlist.clip_start(clipIndex);

//        // clip时清除后一个clip的所有滤镜
//        int nFilterCount = producer.filter_count();

//        for (int i = nFilterCount -1; i >=0 ; i--)
//        {
//             Mlt::Filter* filter = producer.filter(i);
//             if (filter && filter->is_valid() && !filter->get_int("_loader"))
//             {
//                   producer.detach(*filter);
//             }

//        }
//        //end

//        nFilterCount = clip->parent().filter_count();

//               for (int i = nFilterCount -1; i >=0 ; i--)
//               {
//                    Mlt::Filter* filter = clip->parent().filter(i);
//                    if (filter && filter->is_valid() && !filter->get_int("_loader"))
//                    {
//                          clip->parent().detach(*filter);
//                    }

//               }
        // Remove fades that are usually not desired after split.
       // Q_ASSERT(getFilter("fadeOutVolume", &clip->parent()));

//        QScopedPointer<Mlt::Filter> filter(getFilter("fadeOutVolume", &clip->parent()));
//        if (filter && filter->is_valid())
//            clip->parent().detach(*filter);
//        filter.reset(getFilter("fadeOutBrightness", &clip->parent()));
//        if (filter && filter->is_valid())
//            clip->parent().detach(*filter);
//        filter.reset(getFilter("fadeOutMovit", &clip->parent()));
//        if (filter && filter->is_valid())
//            clip->parent().detach(*filter);
//        filter.reset(getFilter("fadeInVolume", &producer));
//        if (filter && filter->is_valid())
//            producer.detach(*filter);
//        filter.reset(getFilter("fadeInBrightness", &producer));
//        if (filter && filter->is_valid())
//            producer.detach(*filter);
//        filter.reset(getFilter("fadeInMovit", &producer));
//        if (filter && filter->is_valid())
//            producer.detach(*filter);


        playlist.resize_clip(clipIndex, in, in + duration - 1);
        QModelIndex modelIndex = createIndex(clipIndex, 0, trackIndex);
        QVector<int> roles;
        roles << DurationRole;
        roles << OutPointRole;
        roles << FadeOutRole;
        emit dataChanged(modelIndex, modelIndex, roles);
        AudioLevelsTask::start(clip->parent(), this, modelIndex);

        beginInsertRows(index(trackIndex), clipIndex + 1, clipIndex + 1);
        if (clip->is_blank()) {
            playlist.insert_blank(clipIndex + 1, out - in - duration);
            endInsertRows();
        } else {
            playlist.insert(producer, clipIndex + 1, in + duration, out);
            endInsertRows();
            modelIndex = createIndex(clipIndex + 1, 0, trackIndex);
            AudioLevelsTask::start(producer.parent(), this, modelIndex);
        }
        emit modified();
    }
}

void MultitrackModel::joinClips(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    if (clipIndex < 0) return;
    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        if (clipIndex >= playlist.count() - 1) return;
        QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));
        int in = info->frame_in;
        int duration = info->frame_count + playlist.clip_length(clipIndex + 1);

        // Move a fade out on the right clip onto the left clip.
        Q_ASSERT(clipIndex < playlist.count());
        Q_ASSERT(clipIndex+1 < playlist.count());

        QScopedPointer<Mlt::Producer> clip(playlist.get_clip(clipIndex));
        info.reset(playlist.clip_info(clipIndex + 1));
        QScopedPointer<Mlt::Filter> filter(getFilter("fadeOutVolume", info->producer));
        if (filter && filter->is_valid())
            clip->parent().attach(*filter);
        filter.reset(getFilter("fadeOutBrightness", info->producer));
        if (filter && filter->is_valid())
            clip->parent().attach(*filter);
        filter.reset(getFilter("fadeOutMovit", info->producer));
        if (filter && filter->is_valid())
            clip->parent().attach(*filter);

        playlist.resize_clip(clipIndex, in, in + duration - 1);
        QModelIndex modelIndex = createIndex(clipIndex, 0, trackIndex);
        QVector<int> roles;
        roles << DurationRole;
        roles << OutPointRole;
        roles << FadeOutRole;
        emit dataChanged(modelIndex, modelIndex, roles);
        AudioLevelsTask::start(clip->parent(), this, modelIndex);

        beginRemoveRows(index(trackIndex), clipIndex + 1, clipIndex + 1);
        playlist.remove(clipIndex + 1);
        endRemoveRows();

        emit modified();
    }
}

void MultitrackModel::appendFromPlaylist(Mlt::Playlist *from, int trackIndex)
{
    Q_ASSERT(trackIndex < m_trackList.size());

    int nPlaylistTime;
    createIfNeeded();
    int i = m_trackList.at(trackIndex).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        removeBlankPlaceholder(playlist, trackIndex);
        nPlaylistTime = playlist.get_playtime();
        i = playlist.count();
        beginInsertRows(index(trackIndex), i, i + from->count() - 1);
        for (int j = 0; j < from->count(); j++) {
            QScopedPointer<Mlt::Producer> clip(from->get_clip(j));
            if (!clip->is_blank()) {
                int in = clip->get_in();
                int out = clip->get_out();
                clip->set_in_and_out(0, clip->get_length() - 1);
                playlist.append(clip->parent(), in, out);
                QModelIndex modelIndex = createIndex(j, 0, trackIndex);
                AudioLevelsTask::start(clip->parent(), this, modelIndex);
            } else {
                playlist.blank(clip->get_out());
            }
        }
        endInsertRows();
        emit modified();
        emit seeked(nPlaylistTime);
    }
}

void MultitrackModel::overwriteFromPlaylist(Mlt::Playlist& from, int trackIndex, int position)
{
    Q_ASSERT(trackIndex < m_trackList.size());

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        int targetIndex = playlist.get_clip_index_at(position);
        if (targetIndex > 0) {
            --targetIndex;
            beginRemoveRows(index(trackIndex), targetIndex, targetIndex);
            playlist.remove(targetIndex);
            endRemoveRows();
        }
        if (targetIndex < playlist.count()) {
            beginRemoveRows(index(trackIndex), targetIndex, targetIndex);
            playlist.remove(targetIndex);
            endRemoveRows();
        }
        if (targetIndex < playlist.count()) {
            beginRemoveRows(index(trackIndex), targetIndex, targetIndex);
            playlist.remove(targetIndex);
            endRemoveRows();
        }
        if (from.count() > 0) {
            beginInsertRows(index(trackIndex), targetIndex, targetIndex + from.count() - 1);
            for (int i = 0; i < from.count(); i++) {
                QScopedPointer<Mlt::Producer> clip(from.get_clip(i));
                if (clip->is_blank()) {
                    playlist.insert_blank(targetIndex, clip->get_out());
                } else {
                    playlist.insert(*clip, targetIndex);
                    QModelIndex modelIndex = createIndex(targetIndex, 0, trackIndex);
                    AudioLevelsTask::start(clip->parent(), this, modelIndex);
                }
                ++targetIndex;
            }
            endInsertRows();
        }
        consolidateBlanks(playlist, trackIndex);
        emit modified();
        emit seeked(position);
    }
    
}

void MultitrackModel::fadeIn(int trackIndex, int clipIndex, int duration)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));
        if (info && info->producer && info->producer->is_valid()) {
            QScopedPointer<Mlt::Filter> filter;
            duration = qBound(0, duration, info->frame_count);

            if (m_trackList[trackIndex].type == VideoTrackType) {
                // Get video filter.
                if (Settings.playerGPU())
                    filter.reset(getFilter("fadeInMovit", info->producer));
                else
                    filter.reset(getFilter("fadeInBrightness", info->producer));
    
                // Add video filter if needed.
                if (!filter) {
                    if (Settings.playerGPU()) {
                        Mlt::Filter f(MLT.profile(), "movit.opacity");
                        f.set(kShotcutFilterProperty, "fadeInMovit");
                        QString opacity = QString("0~=0; %1=1").arg(duration - 1);
                        f.set("opacity", opacity.toLatin1().constData());
                        f.set("alpha", 1);
                        info->producer->attach(f);
                        filter.reset(new Mlt::Filter(f));
                    } else {
                        Mlt::Filter f(MLT.profile(), "brightness");
                        f.set(kShotcutFilterProperty, "fadeInBrightness");
                        QString level = QString("0=0; %1=1").arg(duration - 1);
                        f.set("level", level.toLatin1().constData());
                        f.set("alpha", 1);
                        info->producer->attach(f);
                        filter.reset(new Mlt::Filter(f));
                    }
                } else if (Settings.playerGPU()) {
                    // Special handling for animation keyframes on movit.opacity.
                    QString opacity = QString("0~=0; %1=1").arg(duration - 1);
                    filter->set("opacity", opacity.toLatin1().constData());
                } else {
                    // Special handling for animation keyframes on brightness.
                    QString level = QString("0=0; %1=1").arg(duration - 1);
                    filter->set("level", level.toLatin1().constData());
                }
                // Adjust video filter.
                filter->set_in_and_out(info->frame_in, info->frame_in + duration - 1);
            }

            // Get audio filter.
            filter.reset(getFilter("fadeInVolume", info->producer));

            // Add audio filter if needed.
            if (!filter) {
                Mlt::Filter f(MLT.profile(), "volume");
                f.set(kShotcutFilterProperty, "fadeInVolume");
                f.set("gain", 0);
                f.set("end", 1);
                info->producer->attach(f);
                filter.reset(new Mlt::Filter(f));
            }
            // Adjust audio filter.
            filter->set_in_and_out(info->frame_in, info->frame_in + duration - 1);

             // Signal change.
            QModelIndex modelIndex = createIndex(clipIndex, 0, trackIndex);
            QVector<int> roles;
            roles << FadeInRole;
            emit dataChanged(modelIndex, modelIndex, roles);
            emit modified();
        }
    }
}

void MultitrackModel::fadeOut(int trackIndex, int clipIndex, int duration)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(trackIndex).mlt_index;
    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);

        Q_ASSERT(playlist.clip_info(clipIndex));

        QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));
        if (info && info->producer && info->producer->is_valid()) {
            QScopedPointer<Mlt::Filter> filter;
            duration = qBound(0, duration, info->frame_count);

            if (m_trackList[trackIndex].type == VideoTrackType) {
                // Get video filter.
                if (Settings.playerGPU())
                {
                    Q_ASSERT(getFilter("fadeOutMovit", info->producer));
                    filter.reset(getFilter("fadeOutMovit", info->producer));
                }
                else
                {
                    Q_ASSERT(getFilter("fadeOutBrightness", info->producer));
                    filter.reset(getFilter("fadeOutBrightness", info->producer));
                }

    
                // Add video filter if needed.
                if (!filter) {
                    if (Settings.playerGPU()) {
                        Mlt::Filter f(MLT.profile(), "movit.opacity");
                        f.set(kShotcutFilterProperty, "fadeOutMovit");
                        QString opacity = QString("0~=1; %1=1").arg(duration - 1);
                        f.set("opacity", opacity.toLatin1().constData());
                        f.set("alpha", 1);
                        info->producer->attach(f);
                        filter.reset(new Mlt::Filter(f));
                    } else {
                        Mlt::Filter f(MLT.profile(), "brightness");
                        f.set(kShotcutFilterProperty, "fadeOutBrightness");
                        QString level = QString("0=1; %1=1").arg(duration - 1);
                        f.set("level", level.toLatin1().constData());
                        f.set("alpha", 1);
                        info->producer->attach(f);
                        filter.reset(new Mlt::Filter(f));
                    }
                } else if (Settings.playerGPU()) {
                    // Special handling for animation keyframes on movit.opacity.
                    QString opacity = QString("0~=1; %1=0").arg(duration - 1);
                    filter->set("opacity", opacity.toLatin1().constData());
                } else {
                    // Special handling for animation keyframes on brightness.
                    QString level = QString("0=1; %1=0").arg(duration - 1);
                    filter->set("level", level.toLatin1().constData());
                }
                // Adjust video filter.
                filter->set_in_and_out(info->frame_out - duration + 1, info->frame_out);
            }

            // Get audio filter.
            Q_ASSERT(getFilter("fadeOutVolume", info->producer));
            filter.reset(getFilter("fadeOutVolume", info->producer));

            // Add audio filter if needed.
            if (!filter) {
                Mlt::Filter f(MLT.profile(), "volume");
                f.set(kShotcutFilterProperty, "fadeOutVolume");
                f.set("gain", 1);
                f.set("end", 0);
                info->producer->attach(f);
                filter.reset(new Mlt::Filter(f));
            }
            // Adjust audio filter.
            filter->set_in_and_out(info->frame_out - duration + 1, info->frame_out);

             // Signal change.
            QModelIndex modelIndex = createIndex(clipIndex, 0, trackIndex);
            QVector<int> roles;
            roles << FadeOutRole;
            emit dataChanged(modelIndex, modelIndex, roles);
            emit modified();
        }
    }
}

bool MultitrackModel::addTransitionValid(int fromTrack, int toTrack, int clipIndex, int position)
{

    Q_ASSERT(toTrack < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    bool result = false;
    int i = m_trackList.at(toTrack).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        int targetIndex = playlist.get_clip_index_at(position);
        int endOfPreviousClip = playlist.clip_start(clipIndex - 1) + playlist.clip_length(clipIndex - 1);
//        if (playlist.is_blank(clipIndex - 1))
//            endOfPreviousClip = playlist.clip_start(clipIndex - 2) + playlist.clip_length(clipIndex - 2);
        int endOfCurrentClip = position + playlist.clip_length(clipIndex);
        int startOfNextClip = playlist.clip_start(clipIndex + 1);
//        if (playlist.is_blank(clipIndex + 1))
//            startOfNextClip = playlist.clip_start(clipIndex + 2);

        int startOfPreviousClip = playlist.clip_start(clipIndex - 1);
//        if (playlist.is_blank(clipIndex - 1))
//            startOfPreviousClip = playlist.clip_start(clipIndex - 2);

        int endOfNextClip = startOfNextClip + playlist.clip_length(clipIndex + 1);
//        if (playlist.is_blank(clipIndex + 1))
//            endOfNextClip = startOfNextClip + playlist.clip_length(clipIndex + 2);

//        if (fromTrack == toTrack)//同一轨道
//        if (!playlist.is_blank_at(position))//1 左边
//        if (!playlist.is_blank(clipIndex + 1) || targetIndex < clipIndex)
//        if ((targetIndex == (clipIndex - 1) && endOfCurrentClip > endOfPreviousClip && !isTransition(playlist, clipIndex - 1)) ||
//            (targetIndex == clipIndex && position < startOfNextClip && !isTransition(playlist, clipIndex + 1))) {
//            result = true;
//        }

        if (playlist.count() > 1)
        if (fromTrack == toTrack)//同一轨道
        if (( targetIndex == (clipIndex - 1) && !playlist.is_blank(clipIndex - 1) && endOfCurrentClip > endOfPreviousClip && !isTransition(playlist, clipIndex - 1) && (position > startOfPreviousClip) ) ||
//            ( playlist.is_blank(clipIndex - 1) && targetIndex == (clipIndex - 2) && endOfCurrentClip > endOfPreviousClip && !isTransition(playlist, clipIndex - 2) && (position > startOfPreviousClip) ) ||
            ( targetIndex == clipIndex && !playlist.is_blank(clipIndex + 1) && position < startOfNextClip && !isTransition(playlist, clipIndex + 1) && (endOfNextClip > endOfCurrentClip) ) //||
//            ( targetIndex == clipIndex && playlist.is_blank(clipIndex + 1) && position < startOfNextClip && !isTransition(playlist, clipIndex + 2) && (endOfNextClip > endOfCurrentClip) ) ||
//            ( targetIndex == (clipIndex + 1) && playlist.is_blank(clipIndex + 1) && position < startOfNextClip && !isTransition(playlist, clipIndex + 2) && (endOfNextClip > endOfCurrentClip) )
        ){
            result = true;
        }




    }
    return result;
}

int MultitrackModel::addTransition(int trackIndex, int clipIndex, int position)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        int endOfPreviousClip = playlist.clip_start(clipIndex - 1) + playlist.clip_length(clipIndex - 1);
        if (playlist.is_blank(clipIndex - 1))
            endOfPreviousClip = playlist.clip_start(clipIndex - 2) + playlist.clip_length(clipIndex - 2);
        int endOfCurrentClip = position + playlist.clip_length(clipIndex);
        int startOfNextClip = playlist.clip_start(clipIndex + 1);
        if (playlist.is_blank(clipIndex + 1))
            startOfNextClip = playlist.clip_start(clipIndex + 2);
        int targetIndex = playlist.get_clip_index_at(position);

        if (!playlist.is_blank_at(position))
        if ((targetIndex == (clipIndex - 1) && !playlist.is_blank(clipIndex - 1) && endOfCurrentClip > endOfPreviousClip) ||
            ( targetIndex == (clipIndex - 2) && playlist.is_blank(clipIndex - 1) && endOfCurrentClip > endOfPreviousClip) ||// dragged left
            (targetIndex == clipIndex && position < startOfNextClip) ||
            ( targetIndex == (clipIndex + 1) && playlist.is_blank(clipIndex + 1) && position < startOfNextClip )
                ) { // dragged right
            int duration = qAbs(position - playlist.clip_start(clipIndex));
            if (targetIndex < clipIndex)
                duration = qAbs(endOfPreviousClip - position);
            else
                duration = qAbs(endOfCurrentClip - startOfNextClip);


//                        if (targetIndex == (clipIndex - 2))
//                            playlist.remove(clipIndex - 1);
//                        if ((targetIndex == clipIndex) || targetIndex == (clipIndex + 1))
//                            if (playlist.is_blank(clipIndex + 1))
//                                playlist.remove(clipIndex + 1);

            // Adjust/insert blanks
            moveClipInBlank(playlist, trackIndex, clipIndex, position);



            targetIndex = playlist.get_clip_index_at(position);

            // Create mix
            beginInsertRows(index(trackIndex), targetIndex + 1, targetIndex + 1);
            playlist.mix(targetIndex, duration);
            QScopedPointer<Mlt::Producer> producer(playlist.get_clip(targetIndex + 1));
            producer->parent().set(kShotcutTransitionProperty, kShotcutDefaultTransition);
            endInsertRows();

            // Add transitions
            Mlt::Transition dissolve(MLT.profile(), Settings.playerGPU()? "movit.luma_mix" : "luma");
            Mlt::Transition crossFade(MLT.profile(), "mix:-1");
            playlist.mix_add(targetIndex + 1, &dissolve);
            playlist.mix_add(targetIndex + 1, &crossFade);

            // Notify ins and outs changed
            QModelIndex modelIndex = createIndex(targetIndex, 0, trackIndex);
            QVector<int> roles;
            roles << StartRole;
            roles << OutPointRole;
            roles << DurationRole;
            emit dataChanged(modelIndex, modelIndex, roles);
            modelIndex = createIndex(targetIndex + 2, 0, trackIndex);
            roles.clear();
            roles << StartRole;
            roles << InPointRole;
            roles << DurationRole;
            emit dataChanged(modelIndex, modelIndex, roles);
            emit modified();
            emit seeked(playlist.clip_start(targetIndex + 1));
            return targetIndex + 1;
        }
    }
    return -1;
}

void MultitrackModel::clearMixReferences(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(trackIndex).mlt_index;

    int duration;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        Q_ASSERT(clipIndex - 1 < playlist.count());

        QScopedPointer<Mlt::Producer> producer(playlist.get_clip(clipIndex - 1));
        if (producer && producer->is_valid()) {
            // Clear these since they are no longer valid.
            producer->set("mix_in", NULL, 0);
            producer->set("mix_out", NULL, 0);
            producer.reset(playlist.get_clip(clipIndex));
            if (producer && producer->is_valid()) {
                producer->parent().set("mlt_mix", NULL, 0);
                producer->set("mix_in", NULL, 0);
                producer->set("mix_out", NULL, 0);
            }
            producer.reset(playlist.get_clip(clipIndex + 1));
            if (producer && producer->is_valid()) {
                producer->set("mix_in", NULL, 0);
                producer->set("mix_out", NULL, 0);
            }
        }
    }
}

void MultitrackModel::initMixReferences()
{
    int i = 0;
    int j = 0;
    for(j = 0; j < m_trackList.count(); j++)
    {
        Mlt::Producer *track = m_tractor->track(m_trackList[j].mlt_index);
        if (track)
        {
            Mlt::Playlist playlist(*track);
            for (i = 0; i< playlist.count(); i++)
            {
                if (isTransition(playlist, i))
                {
                    addMixReferences(j, i);
                }
            }

        }
    }
}

void MultitrackModel::addMixReferences(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0 );

       Mlt::Producer *track = m_tractor->track(m_trackList[trackIndex].mlt_index);
        if (track)
        {
            Mlt::Playlist playlist(*track);

                if (isTransition(playlist, clipIndex))
                {
                    Mlt::Producer *clip  = playlist.get_clip(clipIndex);
                    Mlt::Producer *preClip = playlist.get_clip(clipIndex - 1);
                    Mlt::Producer *nextClip = playlist.get_clip(clipIndex + 1);
                    clip->parent().set("mlt_mix", clip->parent().get_producer(), 0);
                    preClip->set("mix_out", clip->parent().get_producer(), 0);
                    clip->parent().set("mix_in", preClip->get_producer(), 0);
                    nextClip->set("mix_in", clip->parent().get_producer(), 0);
                    clip->parent().set("mix_out", nextClip->get_producer(), 0);
                }
        }

}

void MultitrackModel::removeTransition(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
//        clearMixReferences(trackIndex, clipIndex);
        if(!isTransition(playlist, clipIndex))
            return;

        Mlt::Producer *transClip = playlist.get_clip(clipIndex);
//        mlt_properties_debug(transClip->parent().get_properties(), "123456------------", stderr);

        beginRemoveRows(index(trackIndex), clipIndex, clipIndex);
        playlist.remove(clipIndex);
        endRemoveRows();

        --clipIndex;


        QModelIndex modelIndex = createIndex(clipIndex, 0, trackIndex);
        QVector<int> roles;
        roles << OutPointRole;
        roles << DurationRole;
        emit dataChanged(modelIndex, modelIndex, roles);
        modelIndex = createIndex(clipIndex + 1, 0, trackIndex);
        roles << InPointRole;
        roles << DurationRole;
        emit dataChanged(modelIndex, modelIndex, roles);
        emit modified();
    }
}

void MultitrackModel::removeTransitionByTrimIn(int trackIndex, int clipIndex, int delta)
{
    QModelIndex modelIndex = index(clipIndex, 0, index(trackIndex));
    clearMixReferences(trackIndex, clipIndex);
    delta = -data(modelIndex, MultitrackModel::DurationRole).toInt();
    liftClip(trackIndex, clipIndex);
    trimClipOut(trackIndex, clipIndex - 1, delta, false);
    notifyClipOut(trackIndex, clipIndex - 1);
}

void MultitrackModel::removeTransitionByTrimOut(int trackIndex, int clipIndex, int delta)
{
    QModelIndex modelIndex = index(clipIndex + 1, 0, index(trackIndex));
    clearMixReferences(trackIndex, clipIndex);
    delta = -data(modelIndex, MultitrackModel::DurationRole).toInt();
    liftClip(trackIndex, clipIndex + 1);
    trimClipIn(trackIndex, clipIndex + 2, delta, false);
    notifyClipIn(trackIndex, clipIndex + 1);
}

bool MultitrackModel::trimTransitionInValid(int trackIndex, int clipIndex, int delta)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    bool result = false;
    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        if (clipIndex + 2 < playlist.count()) {
            Mlt::ClipInfo info;
            // Check if there is already a transition and its new length valid.
            if (isTransition(playlist, clipIndex + 1) && playlist.clip_length(clipIndex + 1) + delta > 0) {
                // Check clip A out point.
                playlist.clip_info(clipIndex, &info);
                info.frame_out -= delta;
                if (info.frame_out > info.frame_in && info.frame_out < info.length) {
                    // Check clip B in point.
                    playlist.clip_info(clipIndex + 2, &info);
                    info.frame_in -= playlist.clip_length(clipIndex + 1) + delta;
                    if (info.frame_in >= 0 && info.frame_in <= info.frame_out)
                        result = true;
                }
            }
        }
    }
    return result;
}

void MultitrackModel::trimTransitionIn(int trackIndex, int clipIndex, int delta)
{

    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);

        // Adjust the playlist "mix" entry.
        QScopedPointer<Mlt::Producer> producer(playlist.get_clip(clipIndex + 1));
        Mlt::Tractor tractor(producer->parent());
        QScopedPointer<Mlt::Producer> track_a(tractor.track(0));
        QScopedPointer<Mlt::Producer> track_b(tractor.track(1));
        int out = playlist.clip_length(clipIndex + 1) + delta - 1;
        playlist.block();
        track_a->set_in_and_out(track_a->get_in() - delta, track_a->get_out());
        track_b->set_in_and_out(track_b->get_in() - delta, track_b->get_out());
        playlist.unblock();
        tractor.multitrack()->set_in_and_out(0, out);
        tractor.set_in_and_out(0, out);
        producer->set("length", out + 1);
        producer->set_in_and_out(0, out);

        // Adjust the transitions.
        QScopedPointer<Mlt::Service> service(tractor.producer());
        while (service && service->is_valid()) {
            if (service->type() == transition_type) {
                Mlt::Transition transition(*service);
                transition.set_in_and_out(0, out);
            }
            service.reset(service->producer());
        }

        // Adjust clip entry being trimmed.
        Mlt::ClipInfo info;
        playlist.clip_info(clipIndex, &info);
        playlist.resize_clip(clipIndex, info.frame_in, info.frame_out - delta);

        QVector<int> roles;
        roles << OutPointRole;
        roles << DurationRole;
        emit dataChanged(createIndex(clipIndex, 0, trackIndex),
                         createIndex(clipIndex + 1, 0, trackIndex), roles);
        emit modified();
    }
}

bool MultitrackModel::trimTransitionOutValid(int trackIndex, int clipIndex, int delta)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    Q_UNUSED(delta)
    bool result = false;
    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        if (clipIndex > 1) {
            Mlt::ClipInfo info;
            // Check if there is already a transition.
            if (isTransition(playlist, clipIndex - 1)) {
                // Check clip A out point.
                playlist.clip_info(clipIndex - 2, &info);
                info.frame_out += playlist.clip_length(clipIndex - 1) + delta;
                if (info.frame_out > info.frame_in && info.frame_out < info.length) {
                    // Check clip B in point.
                    playlist.clip_info(clipIndex, &info);
                    info.frame_in += delta;
                    if (info.frame_in >= 0 && info.frame_in <= info.frame_out)
                        result = true;
                }
            }
        }
    }
    return result;
}

void MultitrackModel::trimTransitionOut(int trackIndex, int clipIndex, int delta)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);

        // Adjust the playlist "mix" entry.
        Q_ASSERT(playlist.get_clip(clipIndex - 1));

        QScopedPointer<Mlt::Producer> producer(playlist.get_clip(clipIndex - 1));
        Mlt::Tractor tractor(producer->parent());
        QScopedPointer<Mlt::Producer> track_a(tractor.track(0));
        QScopedPointer<Mlt::Producer> track_b(tractor.track(1));
        int out = playlist.clip_length(clipIndex - 1) + delta - 1;
        playlist.block();
        track_a->set_in_and_out(track_a->get_in(), track_a->get_out() + delta);
        track_b->set_in_and_out(track_b->get_in(), track_b->get_out() + delta);
        playlist.unblock();
        tractor.multitrack()->set_in_and_out(0, out);
        tractor.set_in_and_out(0, out);
        producer->set("length", out + 1);
        producer->set_in_and_out(0, out);

        // Adjust the transitions.
        Q_ASSERT(tractor.producer());

        QScopedPointer<Mlt::Service> service(tractor.producer());
        while (service && service->is_valid()) {
            if (service->type() == transition_type) {
                Mlt::Transition transition(*service);
                transition.set_in_and_out(0, out);
            }
            service.reset(service->producer());
        }

        // Adjust clip entry being trimmed.
        Mlt::ClipInfo info;
        playlist.clip_info(clipIndex, &info);
        playlist.resize_clip(clipIndex, info.frame_in + delta, info.frame_out);

        QVector<int> roles;
        roles << OutPointRole;
        roles << DurationRole;
        emit dataChanged(createIndex(clipIndex - 1, 0, trackIndex),
                         createIndex(clipIndex - 1, 0, trackIndex), roles);
        roles.clear();
        roles << InPointRole;
        roles << DurationRole;
        emit dataChanged(createIndex(clipIndex, 0, trackIndex),
                         createIndex(clipIndex, 0, trackIndex), roles);
        emit modified();
    }
}

bool MultitrackModel::addTransitionByTrimInValid(int trackIndex, int clipIndex, int delta)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    Q_UNUSED(delta)
    bool result = false;
    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        if (clipIndex > 0) {
            // Check if preceeding clip is not blank, not already a transition,
            // and there is enough frames before in point of current clip.
            if (delta < 0 && !playlist.is_blank(clipIndex - 1) && !isTransition(playlist, clipIndex - 1)) {
                Mlt::ClipInfo info;
                playlist.clip_info(clipIndex, &info);
                if (info.frame_in >= -delta)
                    result = true;
            } else {
                result = m_isMakingTransition;
            }
        }
    }
    return result;
}

void MultitrackModel::addTransitionByTrimIn(int trackIndex, int clipIndex, int delta)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);

        // Create transition if it does not yet exist.
        if (!isTransition(playlist, clipIndex - 1)) {
            beginInsertRows(index(trackIndex), clipIndex, clipIndex);
            playlist.mix_out(clipIndex - 1, -delta);
            QScopedPointer<Mlt::Producer> producer(playlist.get_clip(clipIndex));
            producer->parent().set(kShotcutTransitionProperty, kShotcutDefaultTransition);
            endInsertRows();

            // Add transitions.
            Mlt::Transition dissolve(MLT.profile(), Settings.playerGPU()? "movit.luma_mix" : "luma");
            Mlt::Transition crossFade(MLT.profile(), "mix:-1");
            playlist.mix_add(clipIndex, &dissolve);
            playlist.mix_add(clipIndex, &crossFade);

            // Notify clip A changed.
            QModelIndex modelIndex = createIndex(clipIndex - 1, 0, trackIndex);
            QVector<int> roles;
            roles << OutPointRole;
            roles << DurationRole;
            emit dataChanged(modelIndex, modelIndex, roles);
            emit modified();
            m_isMakingTransition = true;
        } else if (m_isMakingTransition) {
            // Adjust a transition addition already in progress.
            // m_isMakingTransition will be set false when mouse button released via notifyClipOut().
            trimTransitionIn(trackIndex, clipIndex - 2, -delta);
        }
    }
}

bool MultitrackModel::addTransitionByTrimOutValid(int trackIndex, int clipIndex, int delta)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    Q_UNUSED(delta)
    bool result = false;
    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        if (clipIndex + 1 < playlist.count()) {
            // Check if following clip is not blank, not already a transition,
            // and there is enough frames after out point of current clip.
            if (delta < 0 && !playlist.is_blank(clipIndex + 1) && !isTransition(playlist, clipIndex +  1)) {
                Mlt::ClipInfo info;
                playlist.clip_info(clipIndex, &info);
                if ((info.length - info.frame_out) >= -delta)
                    result = true;
            } else {
                result = m_isMakingTransition;
            }
        }
    }
    return result;
}

void MultitrackModel::addTransitionByTrimOut(int trackIndex, int clipIndex, int delta)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int i = m_trackList.at(trackIndex).mlt_index;
    Q_ASSERT(m_tractor->track(i));
    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);

        // Create transition if it does not yet exist.
        if (!isTransition(playlist, clipIndex + 1)) {
            beginInsertRows(index(trackIndex), clipIndex + 1, clipIndex + 1);
            playlist.mix_in(clipIndex, -delta);

            Q_ASSERT(playlist.get_clip(clipIndex + 1));

            QScopedPointer<Mlt::Producer> producer(playlist.get_clip(clipIndex + 1));
            producer->parent().set(kShotcutTransitionProperty, kShotcutDefaultTransition);
            endInsertRows();

            // Add transitions.
            Mlt::Transition dissolve(MLT.profile(), Settings.playerGPU()? "movit.luma_mix" : "luma");
            Mlt::Transition crossFade(MLT.profile(), "mix:-1");
            playlist.mix_add(clipIndex + 1, &dissolve);
            playlist.mix_add(clipIndex + 1, &crossFade);

            // Notify clip B changed.
            QModelIndex modelIndex = createIndex(clipIndex + 2, 0, trackIndex);
            QVector<int> roles;
            roles << InPointRole;
            roles << DurationRole;
            emit dataChanged(modelIndex, modelIndex, roles);
            emit modified();
            m_isMakingTransition = true;
        } else if (m_isMakingTransition) {
            // Adjust a transition addition already in progress.
            // m_isMakingTransition will be set false when mouse button released via notifyClipIn().
            delta = playlist.clip_start(clipIndex + 1) - (playlist.clip_start(clipIndex) + playlist.clip_length(clipIndex) + delta);
            trimTransitionOut(trackIndex, clipIndex + 2, delta);
        }
    }
}

bool MultitrackModel::moveClipToTrack(int fromTrack, int toTrack, int clipIndex, int position)
{
    Q_ASSERT(fromTrack < m_trackList.size());
    Q_ASSERT(toTrack < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    bool result;
    Mlt::Producer* trackFrom = m_tractor->track(m_trackList.at(fromTrack).mlt_index);
    Mlt::Playlist playlistFrom(*trackFrom);
    delete trackFrom;
    QScopedPointer<Mlt::Producer> clip(playlistFrom.get_clip(clipIndex));
    QModelIndex parentIndex = index(fromTrack);

    // Replace clip on fromTrack with blank.
    beginRemoveRows(parentIndex, clipIndex, clipIndex);
    endRemoveRows();
    beginInsertRows(parentIndex, clipIndex, clipIndex);
    playlistFrom.replace_with_blank(clipIndex);
    endInsertRows();

    result = overwriteClip(toTrack, *clip, position, true) >= 0;

    // If there was an error, rollback the cross-track changes.
    if (!result) {
        // Remove blank on fromTrack.
        beginRemoveRows(parentIndex, clipIndex, clipIndex);
        playlistFrom.remove(clipIndex);
        endRemoveRows();

        // Insert clip on fromTrack.
        beginInsertRows(parentIndex, clipIndex, clipIndex);
        playlistFrom.insert(*clip, clipIndex, clip->get_in(), clip->get_out());
        endInsertRows();
    }
    consolidateBlanks(playlistFrom, fromTrack);

    return result;
}

void MultitrackModel::moveClipToEnd(Mlt::Playlist& playlist, int trackIndex, int clipIndex, int position)
{
    Q_ASSERT(trackIndex < m_trackList.size());

    int n = playlist.count();
    int length = position - playlist.clip_start(n - 1) - playlist.clip_length(n - 1);

    if (clipIndex > 0 && playlist.is_blank(clipIndex - 1)) {
        // If there was a blank on the left adjust it.
        int duration = playlist.clip_length(clipIndex - 1) + playlist.clip_length(clipIndex);
//        LOG_DEBUG() << "adjust blank on left to" << duration;
        playlist.resize_clip(clipIndex - 1, 0, duration - 1);

        QModelIndex index = createIndex(clipIndex - 1, 0, trackIndex);
        QVector<int> roles;
        roles << DurationRole;
        emit dataChanged(index, index, roles);
    } else if ((clipIndex + 1) < n && playlist.is_blank(clipIndex + 1)) {
        // If there was a blank on the right adjust it.
        int duration = playlist.clip_length(clipIndex + 1) + playlist.clip_length(clipIndex);
//        LOG_DEBUG() << "adjust blank on right to" << duration;
        playlist.resize_clip(clipIndex + 1, 0, duration - 1);

        QModelIndex index = createIndex(clipIndex + 1, 0, trackIndex);
        QVector<int> roles;
        roles << DurationRole;
        emit dataChanged(index, index, roles);
    } else {
        // Add new blank
        beginInsertRows(index(trackIndex), clipIndex, clipIndex);
        playlist.insert_blank(clipIndex, playlist.clip_length(clipIndex) - 1);
        endInsertRows();
        ++clipIndex;
        ++n;
    }
    // Add blank to end if needed.
    if (length > 0) {
        beginInsertRows(index(trackIndex), n, n);
        playlist.blank(length - 1);
        endInsertRows();
    }
    // Finally, move clip into place.
    QModelIndex parentIndex = index(trackIndex);
    beginMoveRows(parentIndex, clipIndex, clipIndex, parentIndex, playlist.count());
    playlist.move(clipIndex, playlist.count());
    endMoveRows();
    consolidateBlanks(playlist, trackIndex);
}

void MultitrackModel::relocateClip(Mlt::Playlist& playlist, int trackIndex, int clipIndex, int position)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int targetIndex = playlist.get_clip_index_at(position);

    if (position > playlist.clip_start(targetIndex)) {
//        LOG_DEBUG() << "splitting clip at position" << position;
        // Split target blank clip.
        beginInsertRows(index(trackIndex), targetIndex, targetIndex);
        playlist.split_at(position);
        endInsertRows();
        if (clipIndex >= targetIndex)
            ++clipIndex;

        // Notify blank on left was adjusted.
        QModelIndex modelIndex = createIndex(targetIndex, 0, trackIndex);
        QVector<int> roles;
        roles << DurationRole;
        emit dataChanged(modelIndex, modelIndex, roles);
        ++targetIndex;
    }

    // Adjust blank on right.
    int duration = playlist.clip_length(targetIndex) - playlist.clip_length(clipIndex);
    if (duration > 0) {
//        LOG_DEBUG() << "adjust blank on right" << targetIndex << " to" << duration;
        playlist.resize_clip(targetIndex, 0, duration - 1);
        // Notify blank on right was adjusted.
        QModelIndex modelIndex = createIndex(targetIndex, 0, trackIndex);
        QVector<int> roles;
        roles << DurationRole;
        emit dataChanged(modelIndex, modelIndex, roles);
    } else {
//        LOG_DEBUG() << "remove blank on right";
        beginRemoveRows(index(trackIndex), targetIndex, targetIndex);
        playlist.remove(targetIndex);
        endRemoveRows();
        if (clipIndex >= targetIndex)
            --clipIndex;
    }

    // Insert clip.
    Q_ASSERT(playlist.get_clip(clipIndex));

    QScopedPointer<Mlt::Producer> clip(playlist.get_clip(clipIndex));
    QModelIndex parentIndex = index(trackIndex);
    beginInsertRows(parentIndex, targetIndex, targetIndex);
    playlist.insert(*clip, targetIndex, clip->get_in(), clip->get_out());
    endInsertRows();
    AudioLevelsTask::start(clip->parent(), this, createIndex(targetIndex, 0, trackIndex));
    if (clipIndex >= targetIndex)
        ++clipIndex;

    // Replace clip with blank.
    beginRemoveRows(parentIndex, clipIndex, clipIndex);
    endRemoveRows();
    beginInsertRows(parentIndex, clipIndex, clipIndex);
    playlist.replace_with_blank(clipIndex);
    endInsertRows();

    consolidateBlanks(playlist, trackIndex);
}

void MultitrackModel::moveClipInBlank(Mlt::Playlist& playlist, int trackIndex, int clipIndex, int position)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    int delta = position - playlist.clip_start(clipIndex);

    if (clipIndex > 0 && playlist.is_blank(clipIndex - 1)) {
        // Adjust blank on left.
        int duration = playlist.clip_length(clipIndex - 1) + delta;
        if (duration > 0) {
//            LOG_DEBUG() << "adjust blank on left" << (clipIndex - 1) << "to" << duration;
            playlist.resize_clip(clipIndex - 1, 0, duration - 1);

            QModelIndex index = createIndex(clipIndex - 1, 0, trackIndex);
            QVector<int> roles;
            roles << DurationRole;
            emit dataChanged(index, index, roles);
        } else {
//            LOG_DEBUG() << "remove blank on left";
            int i = clipIndex - 1;
            beginRemoveRows(index(trackIndex), i, i);
            playlist.remove(i);
            endRemoveRows();
            consolidateBlanks(playlist, trackIndex);
            --clipIndex;
        }
    } else if (delta > 0) {
//        LOG_DEBUG() << "add blank on left with duration" << delta;
        // Add blank to left.
        int i = qMax(clipIndex, 0);
        beginInsertRows(index(trackIndex), i, i);
        playlist.insert_blank(i, delta - 1);
        endInsertRows();
        ++clipIndex;
    }

    if ((clipIndex + 1) < playlist.count() && playlist.is_blank(clipIndex + 1)) {
        // Adjust blank to right.
        int duration = playlist.clip_length(clipIndex + 1) - delta;
        if (duration > 0) {
//            LOG_DEBUG() << "adjust blank on right" << (clipIndex + 1) << "to" << duration;
            playlist.resize_clip(clipIndex + 1, 0, duration - 1);

            QModelIndex index = createIndex(clipIndex + 1, 0, trackIndex);
            QVector<int> roles;
            roles << DurationRole;
            emit dataChanged(index, index, roles);
        } else {
//            LOG_DEBUG() << "remove blank on right";
            int i = clipIndex + 1;
            beginRemoveRows(index(trackIndex), i, i);
            playlist.remove(i);
            endRemoveRows();
            consolidateBlanks(playlist, trackIndex);
        }
    } else if (delta < 0 && (clipIndex + 1) < playlist.count()) {
        // Add blank to right.
//        LOG_DEBUG() << "add blank on right with duration" << -delta;
        beginInsertRows(index(trackIndex), clipIndex + 1, clipIndex + 1);
        playlist.insert_blank(clipIndex + 1, (-delta - 1));
        endInsertRows();
    }
}

void MultitrackModel::consolidateBlanks(Mlt::Playlist &playlist, int trackIndex)
{
    Q_ASSERT(trackIndex < m_trackList.size());

    for (int i = 1; i < playlist.count(); i++) {
        if (playlist.is_blank(i - 1) && playlist.is_blank(i)) {
            int out = playlist.clip_length(i - 1) + playlist.clip_length(i) - 1;
            playlist.resize_clip(i - 1, 0, out);
            QModelIndex idx = createIndex(i - 1, 0, trackIndex);
            QVector<int> roles;
            roles << DurationRole;
            emit dataChanged(idx, idx, roles);
            beginRemoveRows(index(trackIndex), i, i);
            playlist.remove(i--);
            endRemoveRows();
        }
    }

    if (playlist.count() > 0) {
        int i = playlist.count() - 1;
        if (playlist.is_blank(i)) {
            beginRemoveRows(index(trackIndex), i, i);
            playlist.remove(i);
            endRemoveRows();
        }
    }


    if (playlist.count() == 0  ) {
        beginInsertRows(index(trackIndex), 0, 0);
        playlist.blank(0);
        endInsertRows();
    }
}

void MultitrackModel::consolidateBlanksAllTracks()
{
    if (!m_tractor) return;
    int i = 0;
    foreach (Track t, m_trackList) {
        Mlt::Producer* track = m_tractor->track(t.mlt_index);
        if (track) {
            Mlt::Playlist playlist(*track);
            consolidateBlanks(playlist, i);
        }
        ++i;
    }
}

void MultitrackModel::audioLevelsReady(const QModelIndex& index)
{
    QVector<int> roles;
    roles << AudioLevelsRole;
    emit dataChanged(index, index, roles);
}

bool MultitrackModel::createIfNeeded()
{
    if (!m_tractor) {
        m_tractor = new Mlt::Tractor(MLT.profile());
        MLT.profile().set_explicit(true);
        m_tractor->set("moviemator", 1);
        retainPlaylist();
        addBackgroundTrack();
        addVideoTrack();
        emit created();
    } else if (!m_trackList.count()) {
        addVideoTrack();
    }
    return true;
}

void MultitrackModel::addBackgroundTrack()
{
    Mlt::Playlist playlist(MLT.profile());
    playlist.set("id", kBackgroundTrackId);
    Mlt::Producer producer(MLT.profile(), "color:black");
    producer.set("length", 1);
    producer.set("id", "black");
    // Allow mixing against frames produced by this producer.
    producer.set("set.test_audio", 0);
    playlist.append(producer);
    m_tractor->set_track(playlist, m_tractor->count());
}

void MultitrackModel::adjustBackgroundDuration()
{
    Q_ASSERT(m_tractor);

    if (!m_tractor) return;
    int n = 0;
    foreach (Track t, m_trackList) {
        Mlt::Producer* track = m_tractor->track(t.mlt_index);
        if (track)
            n = qMax(n, track->get_length());
        delete track;
    }
    Mlt::Producer* track = m_tractor->track(0);
    if (track) {
        Mlt::Playlist playlist(*track);
        Mlt::Producer* clip = playlist.get_clip(0);
        if (clip) {
            if (n != clip->parent().get_length()) {
                clip->parent().set("length", n);
                clip->parent().set_in_and_out(0, n - 1);
                clip->set("length", n);
                clip->set_in_and_out(0, n - 1);
                playlist.resize_clip(0, 0, n - 1);
                emit durationChanged();
            }
            delete clip;
        }
        delete track;
    }
}

int MultitrackModel::addAudioTrack()
{
    if (!m_tractor) {
        m_tractor = new Mlt::Tractor(MLT.profile());
        MLT.profile().set_explicit(true);
        m_tractor->set("moviemator", 1);
        retainPlaylist();
        addBackgroundTrack();
        addAudioTrack();
        emit created();
        emit modified();
        return 0;
    }

    // Get the new track index.
    int i = m_tractor->count();

    // Create the MLT track.
    Mlt::Playlist playlist(MLT.profile());
    playlist.set(kAudioTrackProperty, 1);
    playlist.set("hide", 1);
    playlist.blank(0);
    m_tractor->set_track(playlist, i);
    MLT.updateAvformatCaching(m_tractor->count());

    // Add the mix transition.
    Mlt::Transition mix(MLT.profile(), "mix");
    mix.set("always_active", 1);
    mix.set("combine", 1);
    m_tractor->plant_transition(mix, 0, i);

    // Get the new, logical audio-only index.
    int a = 0;
    foreach (Track t, m_trackList) {
        if (t.type == AudioTrackType)
            ++a;
    }

    // Add the shotcut logical audio track.
    Track t;
    t.mlt_index = i;
    t.type = AudioTrackType;
    t.number = a++;
    QString trackName = QString("A%1").arg(a);
    playlist.set(kTrackNameProperty, trackName.toUtf8().constData());
    beginInsertRows(QModelIndex(), m_trackList.count(), m_trackList.count());
    m_trackList.append(t);
    endInsertRows();
    emit modified();
    MAIN.setCurrentTrack(m_trackList.count() - 1);
    return m_trackList.count() - 1;
}

int MultitrackModel::addVideoTrack()
{
    if (!m_tractor) {
        createIfNeeded();
        return 0;
    }

    // Get the new track index.
    int i = m_tractor->count();

    // Create the MLT track.
    Mlt::Playlist playlist(MLT.profile());
    playlist.set(kVideoTrackProperty, 1);
    playlist.blank(0);
    m_tractor->set_track(playlist, i);
    MLT.updateAvformatCaching(m_tractor->count());

    // Add the mix transition.
    Mlt::Transition mix(MLT.profile(), "mix");
    mix.set("always_active", 1);
    mix.set("combine", 1);
    m_tractor->plant_transition(mix, 0, i);

    // Add the composite transition.
    Mlt::Transition composite(MLT.profile(), Settings.playerGPU()? "movit.overlay" : "frei0r.cairoblend");
    if (!composite.is_valid())
    qDebug() << "composite is invalid!";
    composite.set("disable", 1);
    foreach (Track t, m_trackList) {
        if (t.type == VideoTrackType) {
            composite.set("disable", 0);
            break;
        }
    }

    // Get the new, logical video-only index.
    int v = 0;
    int last_mlt_index = 0;
    foreach (Track t, m_trackList) {
        if (t.type == VideoTrackType) {
            ++v;
            last_mlt_index = t.mlt_index;
        }
    }
    m_tractor->plant_transition(composite, last_mlt_index, i);

    int videoTrackIndex = 0;
    if (isExistsTextTrack()) {
        videoTrackIndex = getTextTrackIndex() + 1;
    }

    // Add the shotcut logical video track.
    Track t;
    t.mlt_index = i;
    t.type = VideoTrackType;
    t.number = v++;
    QString trackName = QString("V%1").arg(v);
    playlist.set(kTrackNameProperty, trackName.toUtf8().constData());
    beginInsertRows(QModelIndex(), videoTrackIndex, videoTrackIndex);
//    m_trackList.prepend(t);
    m_trackList.insert(videoTrackIndex, t);


    emit modified();
    MAIN.setCurrentTrack(videoTrackIndex);
    endInsertRows();

    return videoTrackIndex;
}

int MultitrackModel::addFilterTrack()
{
    if (!m_tractor) {
        m_tractor = new Mlt::Tractor(MLT.profile());
        MLT.profile().set_explicit(true);
        m_tractor->set("moviemator", 1);
        retainPlaylist();
        addBackgroundTrack();
        addFilterTrack();
        emit created();
        emit modified();
        return 0;
    }

    // Get the new track index.
    int i = m_tractor->count();

    // Create the MLT track.
    Mlt::Playlist playlist(MLT.profile());
    playlist.set(kFilterTrackProperty, 1);
    playlist.set("hide", 0);
    playlist.blank(0);
    m_tractor->set_track(playlist, i);
//    MLT.updateAvformatCaching(m_tractor->count());

    // Add the mix transition.
//    Mlt::Transition mix(MLT.profile(), "mix");
//    mix.set("always_active", 1);
//    mix.set("combine", 1);
//    m_tractor->plant_transition(mix, 0, i);

    // Get the new, logical audio-only index.
    int a = 0;
    foreach (Track t, m_trackList) {
        if (t.type == FilterTrackType)
            ++a;
    }

    // Add the shotcut logical audio track.
    Track t;
    t.mlt_index = i;
    t.type = FilterTrackType;
    t.number = a++;
    QString trackName = QString("Fx%1").arg(a);
    playlist.set(kTrackNameProperty, trackName.toUtf8().constData());
    beginInsertRows(QModelIndex(), m_trackList.count(), m_trackList.count());
    m_trackList.append(t);
    endInsertRows();
    emit modified();
    return m_trackList.count() - 1;
}

bool MultitrackModel::isExistsTextTrack() {
    foreach (Track t, m_trackList) {
        if (t.type == TextTrackType) {
            return true;
        }
    }
    return false;
}

int MultitrackModel::getTextTrackIndex() {
    for (int i = 0; i < m_trackList.count(); i++) {
        Track track = m_trackList.at(i);
        if (track.type == TextTrackType) {
            return i;
        }
    }

    return -1;
}

int MultitrackModel::addTextTrack()
{
//    if (!m_tractor) {
//        m_tractor = new Mlt::Tractor(MLT.profile());
//        MLT.profile().set_explicit(true);
//        m_tractor->set("moviemator", 1);
//        retainPlaylist();
//        addBackgroundTrack();
//        addTextTrack();
//        emit created();
//        emit modified();
//        return 0;
//    }

//    // Get the new track index.
//    int i = m_tractor->count();

//    // Create the MLT track.
//    Mlt::Playlist playlist(MLT.profile());
//    playlist.set(kFilterTrackProperty, 1);
//    playlist.set("hide", 0);
//    playlist.blank(0);
//    m_tractor->set_track(playlist, i);
////    MLT.updateAvformatCaching(m_tractor->count());

//    // Add the mix transition.
////    Mlt::Transition mix(MLT.profile(), "mix");
////    mix.set("always_active", 1);
////    mix.set("combine", 1);
////    m_tractor->plant_transition(mix, 0, i);

//    // Get the new, logical audio-only index.
//    int a = 0;
//    foreach (Track t, m_trackList) {
//        if (t.type == TextTrackType)
//            ++a;
//    }

//    // Add the shotcut logical audio track.
//    Track t;
//    t.mlt_index = i;
//    t.type = TextTrackType;
//    t.number = a++;
//    QString trackName = QString("T%1").arg(a);
//    playlist.set(kTrackNameProperty, trackName.toUtf8().constData());
//    beginInsertRows(QModelIndex(), m_trackList.count(), m_trackList.count());
//    m_trackList.append(t);
//    endInsertRows();
//    emit modified();
//    return m_trackList.count() - 1;

    if (!m_tractor) {
        m_tractor = new Mlt::Tractor(MLT.profile());
        MLT.profile().set_explicit(true);
        m_tractor->set("moviemator", 1);
        retainPlaylist();
        addBackgroundTrack();
        addTextTrack();
        emit created();
        emit modified();
        return 0;
    }

    // Get the new track index.
    int i = m_tractor->count();

    // Create the MLT track.
    Mlt::Playlist playlist(MLT.profile());
    playlist.set(kTextTrackProperty, 1);
    playlist.blank(0);
    m_tractor->set_track(playlist, i);
    MLT.updateAvformatCaching(m_tractor->count());

    // Add the mix transition.
    Mlt::Transition mix(MLT.profile(), "mix");
    mix.set("always_active", 1);
    mix.set("combine", 1);
    m_tractor->plant_transition(mix, 0, i);

    // Add the composite transition.
    Mlt::Transition composite(MLT.profile(), Settings.playerGPU()? "movit.overlay" : "frei0r.cairoblend");
    if (!composite.is_valid())
    qDebug() << "composite is invalid!";
    composite.set("disable", 1);
    foreach (Track t, m_trackList) {
        if (t.type == VideoTrackType) {
            composite.set("disable", 0);
            break;
        }
    }

    // Get the new, logical video-only index.
    int v = 0;
    int last_mlt_index = 0;
    foreach (Track t, m_trackList) {
        if (t.type == VideoTrackType) {
            ++v;
            last_mlt_index = t.mlt_index;
        }
    }
    m_tractor->plant_transition(composite, last_mlt_index, i);


    // Add the shotcut logical video track.
    Track t;
    t.mlt_index = i;
    t.type = TextTrackType;
    t.number = v++;
    QString trackName = QString("T%1").arg(v);
    playlist.set(kTrackNameProperty, trackName.toUtf8().constData());
    beginInsertRows(QModelIndex(), 0, 0);
    m_trackList.prepend(t);


    emit modified();
    MAIN.setCurrentTrack(0);
    endInsertRows();

    return 0;
}


void MultitrackModel::removeTrack(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < m_trackList.size()) {
        const Track& track = m_trackList.value(trackIndex);
        QScopedPointer<Mlt::Transition> transition(getTransition("frei0r.cairoblend", track.mlt_index));

        // Remove transitions.
        if (!transition)
            transition.reset(getTransition("movit.overlay", track.mlt_index));
        if (transition)
            m_tractor->field()->disconnect_service(*transition);
        transition.reset(getTransition("mix", track.mlt_index));
        if (transition)
            m_tractor->field()->disconnect_service(*transition);

//        foreach (Track t, m_trackList) LOG_DEBUG() << (t.type == VideoTrackType?"Video":"Audio") << "track number" << t.number << "mlt_index" << t.mlt_index;
//        LOG_DEBUG() << trackIndex << "mlt_index" << track.mlt_index;

        // Remove track.
        beginRemoveRows(QModelIndex(), trackIndex, trackIndex);
        m_tractor->remove_track(track.mlt_index);
        m_trackList.removeAt(trackIndex);
        endRemoveRows();

//        foreach (Track t, m_trackList) LOG_DEBUG() << (t.type == VideoTrackType?"Video":"Audio") << "track number" << t.number << "mlt_index" << t.mlt_index;

        // Renumber other tracks.
        int row = 0;
        foreach (Track t, m_trackList) {
            if (t.mlt_index > track.mlt_index)
                --m_trackList[row].mlt_index;
            if (t.type == track.type && t.number > track.number) {
                --m_trackList[row].number;

                // Rename default track names.
                QScopedPointer<Mlt::Producer> mltTrack(m_tractor->track(m_trackList[row].mlt_index));
                QString trackNameTemplate = (t.type == VideoTrackType)? QString("V%1") : QString("A%1");
                QString trackName = trackNameTemplate.arg(t.number + 1);
                if (mltTrack && mltTrack->get(kTrackNameProperty) == trackName) {
                    trackName = trackNameTemplate.arg(m_trackList[row].number + 1);
                    mltTrack->set(kTrackNameProperty, trackName.toUtf8().constData());
                    QModelIndex modelIndex = index(row, 0);
                    QVector<int> roles;
                    roles << NameRole;
                    emit dataChanged(modelIndex, modelIndex, roles);
                }
            }
            ++row;
        }
//        foreach (Track t, m_trackList) LOG_DEBUG() << (t.type == VideoTrackType?"Video":"Audio") << "track number" << t.number << "mlt_index" << t.mlt_index;
    }
    emit modified();
}

void MultitrackModel::retainPlaylist()
{
//    if (!MAIN.playlist())
//        MAIN.playlistDock()->model()->createIfNeeded();
//    Mlt::Playlist playlist(*MAIN.playlist());
//    playlist.set("id", kPlaylistTrackId);
//    QString retain = QString("xml_retain %1").arg(kPlaylistTrackId);
//    m_tractor->set(retain.toUtf8().constData(), playlist.get_service(), 0);
}

void MultitrackModel::loadPlaylist()
{
//    Mlt::Properties retainList((mlt_properties) m_tractor->get_data("xml_retain"));
//    if (retainList.is_valid() && retainList.get_data(kPlaylistTrackId)) {
//        Mlt::Playlist playlist((mlt_playlist) retainList.get_data(kPlaylistTrackId));
//        if (playlist.is_valid() && playlist.type() == playlist_type)
//            MAIN.playlistDock()->model()->setPlaylist(playlist);
//    }
//    retainPlaylist();
}

void MultitrackModel::removeRegion(int trackIndex, int position, int length)
{
    Q_ASSERT(trackIndex < m_trackList.size());

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        int clipIndex = playlist.get_clip_index_at(position);

        if (clipIndex >= 0 && clipIndex < playlist.count())
        {
            int clipStart = playlist.clip_start(clipIndex);
            int playtime = playlist.get_playtime();
            playlist.block(playlist.get_playlist());

            if (position + length > playtime)
                length -= (position + length - playtime);

            if (clipStart < position) {
                beginInsertRows(index(trackIndex), clipIndex + 1, clipIndex + 1);
                playlist.split_at(position);
                endInsertRows();
                QModelIndex modelIndex = createIndex(clipIndex, 0, trackIndex);
                QVector<int> roles;
                roles << DurationRole;
                roles << OutPointRole;
                emit dataChanged(modelIndex, modelIndex, roles);
                ++clipIndex;
            }

            while (length > 0) {
                if (playlist.clip_length(clipIndex) > length) {
                    beginInsertRows(index(trackIndex), clipIndex + 1, clipIndex + 1);
                    playlist.split_at(position + length);
                    endInsertRows();
                    QModelIndex modelIndex = createIndex(clipIndex, 0, trackIndex);
                    QVector<int> roles;
                    roles << DurationRole;
                    roles << OutPointRole;
                    emit dataChanged(modelIndex, modelIndex, roles);
                }
                length -= playlist.clip_length(clipIndex);
                if (clipIndex < playlist.count()) {
                    // Shotcut does not like the behavior of remove() on a
                    // transition (MLT mix clip). So, we null mlt_mix to prevent it.
                    QScopedPointer<Mlt::Producer> producer(playlist.get_clip(clipIndex));
                    if (producer)
                        producer->parent().set("mlt_mix", NULL, 0);
                    beginRemoveRows(index(trackIndex), clipIndex, clipIndex);
                    playlist.remove(clipIndex);
                    endRemoveRows();
                }
            }
            playlist.unblock(playlist.get_playlist());
            consolidateBlanks(playlist, trackIndex);
        }
    }
}

bool MultitrackModel::isTransition(Mlt::Playlist &playlist, int clipIndex) const
{
    Q_ASSERT(clipIndex >= 0);
    Q_ASSERT(clipIndex < playlist.count());

    QScopedPointer<Mlt::Producer> producer(playlist.get_clip(clipIndex));
    if (producer && producer->parent().get(kShotcutTransitionProperty))
        return true;
    return false;
}

void MultitrackModel::insertTrack(int trackIndex, TrackType type)
{
    if (!m_tractor || trackIndex <= 0) {
        addVideoTrack();
        return;
    }

    // Get the new track index.
    Track& track = m_trackList[qMax(0, qMin(trackIndex, m_trackList.count() - 1))];
    int i = track.mlt_index;
    if (type == VideoTrackType)
        ++i;

    if (trackIndex >= m_trackList.count()) {
        if (type == AudioTrackType) {
            addAudioTrack();
            return;
        } else if (type == VideoTrackType) {
            i = track.mlt_index;
        }
    }

//    foreach (Track t, m_trackList) LOG_DEBUG() << (t.type == VideoTrackType?"Video":"Audio") << "track number" << t.number << "mlt_index" << t.mlt_index;
//    LOG_DEBUG() << "trackIndex" << trackIndex << "mlt_index" << i;

    // Get the new, logical video-only index.
    int videoTrackCount = 0;
    int last_mlt_index = 0;
    int row = 0;
    foreach (Track t, m_trackList) {
        if (t.type == track.type) {
            if ((t.type == VideoTrackType && t.number > track.number) ||
                (t.type == AudioTrackType && t.number >= track.number)) {
                // Rename default track names.
                QScopedPointer<Mlt::Producer> mltTrack(m_tractor->track(t.mlt_index));
                QString trackNameTemplate = (t.type == VideoTrackType)? QString("V%1") : QString("A%1");
                QString trackName = trackNameTemplate.arg(++t.number);
                if (mltTrack && mltTrack->get(kTrackNameProperty) == trackName) {
                    trackName = trackNameTemplate.arg(t.number + 1);
                    mltTrack->set(kTrackNameProperty, trackName.toUtf8().constData());
                    QModelIndex modelIndex = index(row, 0);
                    QVector<int> roles;
                    roles << NameRole;
                    emit dataChanged(modelIndex, modelIndex, roles);
                }
                ++m_trackList[row].number;
            }
        }
        if (t.mlt_index >= i)
            ++m_trackList[row].mlt_index;
        if (t.type == VideoTrackType) {
            ++videoTrackCount;
            last_mlt_index = t.mlt_index;
        }
        ++row;
    }

//    foreach (Track t, m_trackList) LOG_DEBUG() << (t.type == VideoTrackType?"Video":"Audio") << "track number" << t.number << "mlt_index" << t.mlt_index;

    // Create the MLT track.
    Mlt::Playlist playlist(MLT.profile());
    if (track.type == VideoTrackType) {
        playlist.set(kVideoTrackProperty, 1);
    } else if (track.type == AudioTrackType) {
        playlist.set(kAudioTrackProperty, 1);
        playlist.set("hide", 1);
    }
    playlist.blank(0);
    m_tractor->insert_track(playlist, i);
    MLT.updateAvformatCaching(m_tractor->count());

    // Add the mix transition.
    Mlt::Transition mix(MLT.profile(), "mix");
    mix.set("always_active", 1);
    mix.set("combine", 1);
    m_tractor->plant_transition(mix, 0, i);

    if (type == VideoTrackType) {
        // Add the composite transition.
        Mlt::Transition composite(MLT.profile(), Settings.playerGPU()? "movit.overlay" : "frei0r.cairoblend");
        m_tractor->plant_transition(composite, last_mlt_index, i);
    }

    // Add the shotcut logical video track.
    Track t;
    t.mlt_index = i;
    t.type = type;
    QString trackName;
    if (t.type == VideoTrackType) {
        t.number = videoTrackCount - trackIndex;
        trackName = QString("V%1");
    } else if (t.type == AudioTrackType) {
        t.number = trackIndex - videoTrackCount;
        trackName = QString("A%1");
    }
    trackName = trackName.arg(t.number + 1);
    playlist.set(kTrackNameProperty, trackName.toUtf8().constData());
    beginInsertRows(QModelIndex(), trackIndex, trackIndex);
    m_trackList.insert(trackIndex, t);
    endInsertRows();
    emit modified();

    MAIN.setCurrentTrack(trackIndex);
//    foreach (Track t, m_trackList) LOG_DEBUG() << (t.type == VideoTrackType?"Video":"Audio") << "track number" << t.number << "mlt_index" << t.mlt_index;
}

void MultitrackModel::insertOrAdjustBlankAt(QList<int> tracks, int position, int length)
{
    foreach (int trackIndex, tracks) {
        int mltIndex = m_trackList.at(trackIndex).mlt_index;
        Q_ASSERT(m_tractor->track(mltIndex));
        QScopedPointer<Mlt::Producer> otherTrack(m_tractor->track(mltIndex));

        if (otherTrack) {
            Mlt::Playlist trackPlaylist(*otherTrack);
            int idx = trackPlaylist.get_clip_index_at(position);

            if (trackPlaylist.is_blank(idx)) {

                trackPlaylist.resize_clip(idx, 0, trackPlaylist.clip_length(idx) + length);
                QModelIndex modelIndex = createIndex(idx, 0, trackIndex);
                emit dataChanged(modelIndex, modelIndex, QVector<int>() << DurationRole);
            } else if (length > 0) {
                splitClip(trackIndex, idx, position);
                beginInsertRows(index(trackIndex), idx + 1, idx + 1);
                trackPlaylist.insert_blank(idx + 1, length);
                endInsertRows();
            } else {
                Q_ASSERT(!"unsupported");
            }
        }
    }
}

void MultitrackModel::load()
{
    if (m_tractor) {
        beginResetModel();
        delete m_tractor;
        m_tractor = 0;
        m_trackList.clear();
        endResetModel();
    }
    // In some versions of MLT, the resource property is the XML filename,
    // but the Mlt::Tractor(Service&) constructor will fail unless it detects
    // the type as tractor, and mlt_service_identify() needs the resource
    // property to say "<tractor>" to identify it as playlist type.
    MLT.producer()->set("mlt_type", "mlt_producer");
    MLT.producer()->set("resource", "<tractor>");
    MLT.profile().set_explicit(true);
    m_tractor = new Mlt::Tractor(*MLT.producer());
    if (!m_tractor->is_valid()) {
        delete m_tractor;
        m_tractor = 0;
        return;
    }

    loadPlaylist();
    addBlackTrackIfNeeded();
    MLT.updateAvformatCaching(m_tractor->count());
    refreshTrackList();
//    convertOldDoc();
    consolidateBlanksAllTracks();
    adjustBackgroundDuration();
    initMixReferences();
    if (m_trackList.count() > 0) {
        beginInsertRows(QModelIndex(), 0, m_trackList.count() - 1);
        endInsertRows();
        getAudioLevels();
    }
    emit loaded();
}

void MultitrackModel::reload(bool asynchronous)
{
    if (m_tractor) {
        if (asynchronous) {
            emit reloadRequested();
        } else {
            beginResetModel();
            endResetModel();
            getAudioLevels();
        }
    }
}

void MultitrackModel::close()
{
    if (!m_tractor) return;
    if (m_trackList.count() > 0) {
        beginRemoveRows(QModelIndex(), 0, m_trackList.count() - 1);
        m_trackList.clear();
        endRemoveRows();
    }
    delete m_tractor;
    m_tractor = 0;
    emit closed();
}

int MultitrackModel::clipIndex(int trackIndex, int position)
{
    Q_ASSERT(trackIndex < m_trackList.size());

    int i = m_trackList.at(trackIndex).mlt_index;

    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        return playlist.get_clip_index_at(position);
    }
    return -1; // error
}

void MultitrackModel::refreshTrackList()
{
    int n = m_tractor->count();
    int a = 0;
    int v = 0;
    bool isKdenlive = false;

    // Add video tracks in reverse order.
    for (int i = 0; i < n; ++i) {
        Q_ASSERT(m_tractor->track(i));

        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        if (!track)
            continue;
        QString trackId = track->get("id");
        if (trackId == "black_track")
            isKdenlive = true;
        else if (trackId == kBackgroundTrackId)
            continue;
        else if (!track->get(kShotcutPlaylistProperty) && !track->get(kAudioTrackProperty) && !track->get(kTextTrackProperty)) {
            int hide = track->get_int("hide");
             // hide: 0 = a/v, 2 = muted video track
            if (track->get(kVideoTrackProperty) || hide == 0 || hide == 2) {
                Track t;
                t.mlt_index = i;
                t.type = VideoTrackType;
                t.number = v++;
                QString trackName = track->get(kTrackNameProperty);
                if (trackName.isEmpty())
                    trackName = QString("V%1").arg(v);
                track->set(kTrackNameProperty, trackName.toUtf8().constData());
                m_trackList.prepend(t);
            }
        }
    }

    // Add audio tracks.
    for (int i = 0; i < n; ++i) {
        Q_ASSERT(m_tractor->track(i));

        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        if (!track)
            continue;
        QString trackId = track->get("id");
        if (trackId == "black_track")
            isKdenlive = true;
        else if (isKdenlive && trackId == "playlist1")
            // In Kdenlive, playlist1 is a special audio mixdown track.
            continue;
        else if (trackId == kPlaylistTrackId)
            continue;
        else if (!track->get(kShotcutPlaylistProperty) && !track->get(kVideoTrackProperty) && !track->get(kTextTrackProperty)) {
            int hide = track->get_int("hide");
            // hide: 1 = audio only track, 3 = muted audio-only track
            if (track->get(kAudioTrackProperty) || hide == 1 || hide == 3) {
                Track t;
                t.mlt_index = i;
                t.type = AudioTrackType;
                t.number = a++;
                QString trackName = track->get(kTrackNameProperty);
                if (trackName.isEmpty())
                    trackName = QString("A%1").arg(a);
                track->set(kTrackNameProperty, trackName.toUtf8().constData());
                m_trackList.append(t);
//                LOG_DEBUG() << __FUNCTION__ << QString(track->get("id")) << i;
            }
        }
    }

    // Add text tracks in reverse order.
    for (int i = 0; i < n; ++i) {
        Q_ASSERT(m_tractor->track(i));

        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        if (!track)
            continue;
        QString trackId = track->get("id");
        if (trackId == "black_track")
            isKdenlive = true;
        else if (trackId == kBackgroundTrackId)
            continue;
        else if (!track->get(kShotcutPlaylistProperty) && !track->get(kAudioTrackProperty) && !track->get(kVideoTrackProperty)) {
            if (track->get(kTextTrackProperty)) {
                Track t;
                t.mlt_index = i;
                t.type = TextTrackType;
                t.number = v++;
                QString trackName = track->get(kTrackNameProperty);
                if (trackName.isEmpty())
                    trackName = QString("T%1").arg(v);
                track->set(kTrackNameProperty, trackName.toUtf8().constData());
                m_trackList.prepend(t);
            }
        }
    }
}

void MultitrackModel::getAudioLevels()
{
    for (int trackIx = 0; trackIx < m_trackList.size(); trackIx++) {
        int i = m_trackList.at(trackIx).mlt_index;
        Q_ASSERT(m_tractor->track(i));
        QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
        Mlt::Playlist playlist(*track);
        for (int clipIx = 0; clipIx < playlist.count(); clipIx++) {
            QScopedPointer<Mlt::Producer> clip(playlist.get_clip(clipIx));
            if (clip && clip->is_valid() && !clip->is_blank() && clip->get_int("audio_index") > -1) {
                QModelIndex index = createIndex(clipIx, 0, trackIx);
                AudioLevelsTask::start(clip->parent(), this, index);
            }
        }
    }
}

void MultitrackModel::addBlackTrackIfNeeded()
{
    return;
    // Make sure the first track is black silence and add it if needed
    bool found = false;
    int n = m_tractor->count();
    Q_ASSERT(m_tractor->track(0));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(0));
    if (track) {
        Mlt::Playlist playlist(*track);
        Q_ASSERT(playlist.get_clip(0));

        QScopedPointer<Mlt::Producer> clip(playlist.get_clip(0));
        if (clip && QString(clip->get("resource")) == "black")
            found = true;
    }
    if (!found) {
        // Move all existing tracks down by 1.
        for (int i = n; i > 0; i++) {
            Mlt::Producer* producer = m_tractor->track(n - 1);
            if (producer)
                m_tractor->set_track(*producer, n);
            delete producer;
        }
        Mlt::Producer producer(MLT.profile(), "color:black");
        m_tractor->set_track(producer, 0);
    }
}

void MultitrackModel::convertOldDoc()
{
    // Convert composite to frei0r.cairoblend.
    int n = m_tractor->count();
    for (int i = 1; i < n; ++i) {
        Q_ASSERT(getTransition("composite", i));

        QScopedPointer<Mlt::Transition> transition(getTransition("composite", i));
        if (transition) {
            Mlt::Transition composite(MLT.profile(), "frei0r.cairoblend");
            composite.set("disable", transition->get_int("disable"));
            m_tractor->field()->disconnect_service(*transition);
            m_tractor->plant_transition(composite, transition->get_int("a_track"), i);
        }
    }

    // Remove movit.rect filters.
    QScopedPointer<Mlt::Service> service(m_tractor->producer());
    while (service && service->is_valid()) {
        if (service->type() == filter_type) {
            Mlt::Filter f((mlt_filter) service->get_service());
            if (QString::fromLatin1(f.get("mlt_service")) == "movit.rect") {
                m_tractor->field()->disconnect_service(f);
            }
        }
        service.reset(service->producer());
    }

    // Change a_track of composite transitions to bottom video track.
    int a_track = 0;
    foreach (Track t, m_trackList) {
        if (t.type == VideoTrackType)
            a_track = t.mlt_index;
    }
    QString name = Settings.playerGPU()? "movit.overlay" : "frei0r.cairoblend";
    foreach (Track t, m_trackList) {
        if (t.type == VideoTrackType) {
            Q_ASSERT(getTransition(name, t.mlt_index));

            QScopedPointer<Mlt::Transition> transition(getTransition(name, t.mlt_index));
            if (transition && transition->get_a_track() != 0)
                transition->set("a_track", a_track);
        }
    }

    // Ensure the background track clears the test_audio flag on frames.
    QScopedPointer<Mlt::Producer> track(m_tractor->track(0));
    if (track) {
        Mlt::Playlist playlist(*track);
        QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(0));
        if (info && info->producer->is_valid() && QString(info->producer->get("id")) == "black")
            info->producer->set("set.test_audio", 0);
    }
}

Mlt::Transition *MultitrackModel::getTransition(const QString &name, int trackIndex) const
{
    QScopedPointer<Mlt::Service> service(m_tractor->producer());
    while (service && service->is_valid()) {
        if (service->type() == transition_type) {
            Mlt::Transition t((mlt_transition) service->get_service());
            if (name == t.get("mlt_service") && t.get_b_track() == trackIndex)
                return new Mlt::Transition(t);
        }
        service.reset(service->producer());
    }
    return 0;
}

Mlt::Filter *MultitrackModel::getFilter(const QString &name, int trackIndex) const
{
    QScopedPointer<Mlt::Service> service(m_tractor->producer());
    while (service && service->is_valid()) {
        if (service->type() == filter_type) {
            Mlt::Filter f((mlt_filter) service->get_service());
            if (name == f.get("mlt_service") && f.get_track() == trackIndex)
                return new Mlt::Filter(f);
        }
        service.reset(service->producer());
    }
    return 0;
}

Mlt::Filter *MultitrackModel::getFilter(const QString &name, Mlt::Service* service) const
{
    for (int i = 0; i < service->filter_count(); i++) {
        Mlt::Filter* filter = service->filter(i);
        if (filter) {
            if (name == filter->get(kShotcutFilterProperty))
                return filter;
            delete filter;
        }
    }
    return 0;
}

void MultitrackModel::removeBlankPlaceholder(Mlt::Playlist& playlist, int trackIndex)
{
    if (playlist.count() == 1 && playlist.is_blank(0)) {
        beginRemoveRows(index(trackIndex), 0, 0);
        playlist.remove(0);
        endRemoveRows();
    }
}

//test:add filter to timeline
void MultitrackModel::addFilter(int trackIndex, Mlt::Producer *filter)
{
    Q_ASSERT(trackIndex < m_trackList.size());

    //如果trackIndex代表的轨道不是滤镜轨道，查找第一个滤镜轨道的索引
    TrackType trackType = m_trackList.at(trackIndex).type;
    if (trackType != TextTrackType)
    {
        int count = m_trackList.count();
        if (count <= 0)
            return;
        trackIndex = -1;
        int j = 0;
        for (j = 0; j < count; j++)
        {
            trackType = m_trackList.at(j).type;
            if (trackType == FilterTrackType)
            {
                trackIndex = j;
                break;
            }
        }
    }

    if (trackIndex == -1)
        return;

    int i = m_trackList.at(trackIndex).mlt_index;
    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
//        removeBlankPlaceholder(playlist, trackIndex);
        i = playlist.count();
//        int in = clip.get_in();
//        int out = clip.get_out();
//        clip.set_in_and_out(0, clip.get_length() - 1);
        beginInsertRows(index(trackIndex), i, i);
        playlist.append(*filter, 0, 100);
        endInsertRows();
//        QModelIndex index = createIndex(i, 0, trackIndex);
//        AudioLevelsTask::start(clip.parent(), this, index);
        emit modified();
        emit seeked(playlist.clip_start(i));
//        return i;
    }
//    return -1;
}

//test:add text to timeline
void MultitrackModel::addText(int trackIndex, Mlt::Producer *filter)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(filter);
    //如果trackIndex代表的轨道不是滤镜轨道，查找第一个滤镜轨道的索引
//    TrackType trackType = m_trackList.at(trackIndex).type;
//    if (trackType != TextTrackType)
//    {
//        int count = m_trackList.count();
//        if (count <= 0)
//            return;
//        trackIndex = -1;
//        int j = 0;
//        for (j = 0; j < count; j++)
//        {
//            trackType = m_trackList.at(j).type;
//            if (trackType == TextTrackType)
//            {
//                trackIndex = j;
//                break;
//            }
//        }
//    }

    Mlt::Producer *producer = new Mlt::Producer(MLT.profile(), "color:#00000000");
//    producer->set("argument", "blue");
    producer->set_in_and_out(0, 200);

//    Mlt::Producer *producer = new Mlt::Producer(MLT.profile(),"loader", "qtext");
//    producer->set_in_and_out(0, 200);

//           producer->set("text", "text");
//           producer->set("fgcolour", "#ffffffff");
//           producer->set("bgcolour", "#20000000");
//           producer->set("olcolour", "#ff000000");
//           producer->set("weight", 600);
//           producer->set("valign", "center");
//           producer->set("halign", "center");
//           producer->set("size", 120);

//    Mlt::Filter* textFilter = new Mlt::Filter(MLT.profile(), "dynamictext");
//       textFilter->set("argument", "text");
//       textFilter->set("fgcolour", "#ffffffff");
//       textFilter->set("bgcolour", "#20000000");
//       textFilter->set("olcolour", "#ff000000");
//       textFilter->set("weight", 500);

//       textFilter->set("geometry",   "0/50%:50%x50%");
//       textFilter->set("valign", "center");
//       textFilter->set("halign", "center");
//    //   filter->savePreset(preset.parameters, qsTr('Bottom Left'));



//       textFilter->set("size", 50);
//producer->attach(*textFilter);


    if (trackIndex == -1)
        return;

    int i = m_trackList.at(trackIndex).mlt_index;
    Q_ASSERT(m_tractor->track(i));
    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
//        removeBlankPlaceholder(playlist, trackIndex);
        i = playlist.count();
//        int in = clip.get_in();
//        int out = clip.get_out();
//        clip.set_in_and_out(0, clip.get_length() - 1);
        beginInsertRows(index(trackIndex), i, i);
        playlist.append(*producer);
        endInsertRows();
//        QModelIndex index = createIndex(i, 0, trackIndex);
//        AudioLevelsTask::start(clip.parent(), this, index);
        emit modified();
        emit seeked(playlist.clip_start(i));
//        return i;
    }
//    return -1;
}


bool MultitrackModel::checkClip(Mlt::Producer &clip)
{
    int len = clip.get_length();
    if (len <= 0)
    {
        QMessageBox dialog(QMessageBox::Critical, qApp->applicationName(), tr("Failed to Add Clip\nInvalid Time Code!"), QMessageBox::Ok, &(MAIN));
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
        dialog.setWindowModality(Qt::WindowModal);
        dialog.setDefaultButton(QMessageBox::Ok);
//        dialog.setEscapeButton(QMessageBox::No);
        int r = dialog.exec();
        return false;
    }
    return true;
}


int  MultitrackModel::getTrackType(int trackIndex)
{

    Track t = m_trackList.at(trackIndex);
    return t.type;
}

void MultitrackModel::setTransitionDuration(int trackIndex, int clipIndex, int duration)
{
    Q_ASSERT(trackIndex < m_trackList.size());
    Q_ASSERT(clipIndex >= 0);

    const Track& track = m_trackList.value(trackIndex);
    int i = m_trackList.at(trackIndex).mlt_index;
    Q_ASSERT(m_tractor->track(i));

    QScopedPointer<Mlt::Producer> trackProducer(m_tractor->track(i));
    if (trackProducer) {
        Mlt::Playlist playlist(*trackProducer);
        if (isTransition(playlist, clipIndex))
        {

            playlist.resize_clip(clipIndex, 0, duration);

            Q_ASSERT(playlist.get_clip(clipIndex));
            QScopedPointer<Mlt::Producer> producer(playlist.get_clip(clipIndex));
            Mlt::Tractor tractor(producer->parent());

            QScopedPointer<Mlt::Service> service(tractor.producer());
            while (service && service->is_valid()) {
                if (service->type() == transition_type) {
                    Mlt::Transition transition(*service);
                    transition.set_in_and_out(0, duration);
                }
                service.reset(service->producer());
            }

            // Notify ins and outs changed
            QModelIndex modelIndex = createIndex(clipIndex - 1, 0, trackIndex);
            QVector<int> roles;
            roles << StartRole;
            roles << OutPointRole;
            roles << DurationRole;
            emit dataChanged(modelIndex, modelIndex, roles);

            modelIndex = createIndex(clipIndex, 0, trackIndex);
                        roles.clear();
                        roles << StartRole;
                        roles << OutPointRole;
                        roles << DurationRole;
                        emit dataChanged(modelIndex, modelIndex, roles);
            modelIndex = createIndex(clipIndex + 1, 0, trackIndex);
            roles.clear();
            roles << StartRole;
            roles << InPointRole;
            roles << DurationRole;
            emit dataChanged(modelIndex, modelIndex, roles);
            emit modified();
        }
    }
}

bool MultitrackModel::hasFilterApplied(Mlt::Producer *producer) const
{
    bool result = false;
    if (producer && producer->is_valid())
    {
        int count = producer->filter_count();
        for (int i = 0; i < count; i++) {
            Mlt::Filter* filter = producer->filter(i);
            if (filter && filter->is_valid() && !filter->get_int("_loader") && !filter->get_int("disable")) {
                result = true;
                break;
            }
            delete filter;
        }
    }

    return result;
}

int MultitrackModel::moveInsertClipValid(int fromTrack, int toTrack, int clipIndex, int position)
{
    Q_ASSERT(toTrack < m_trackList.size());
    int result  = -1;

    int toClipIndex;
    //自身位置
    int destinationClipIndex = this->clipIndex(toTrack, position);
    if ((fromTrack == toTrack) && (destinationClipIndex == clipIndex))
        return result;


    int i = m_trackList.at(toTrack).mlt_index;
    Q_ASSERT(m_tractor->track(i));
    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track)
    {
        Mlt::Playlist playlist(*track);
        //转场位置返回false
        if (isTransition(playlist, destinationClipIndex))
            return result;

        int startOfClip = playlist.clip_start(destinationClipIndex);
        int endOfClip = startOfClip + playlist.clip_length(destinationClipIndex);
        if (abs(position - startOfClip) < abs(endOfClip - position))
        {
            toClipIndex = destinationClipIndex;
            if ( (fromTrack == toTrack) && (destinationClipIndex - 1 == clipIndex) )
                return result;
            if (destinationClipIndex > 0 &&  isTransition(playlist, destinationClipIndex - 1))
                return result;
        }
        else
        {
            toClipIndex = destinationClipIndex + 1;
            if ( (fromTrack == toTrack) && ((destinationClipIndex + 1) == clipIndex) )
                return result;
            if (destinationClipIndex < playlist.count() - 1 &&  isTransition(playlist, destinationClipIndex + 1))
                return result;
        }

        result = playlist.clip_start(toClipIndex);
        //同一轨道从前往后移动
        if ( (fromTrack == toTrack)  && (toClipIndex > clipIndex))
            result --;
    }

    return result;
}

int MultitrackModel::moveInsertClip(int fromTrack, int toTrack, int clipIndex, int position)
{
    Q_ASSERT(fromTrack < m_trackList.size());

    Mlt::Producer *fromTrackProducer = m_tractor->track(m_trackList.at(fromTrack).mlt_index);
    Mlt::Playlist fromPlaylist(*fromTrackProducer);
    delete fromTrackProducer;
    QScopedPointer<Mlt::Producer> clip(fromPlaylist.get_clip(clipIndex));


    //新位置插入clip
    int toClipIndex;
    int destinationClipIndex = this->clipIndex(toTrack, position);
    if ((fromTrack == toTrack) && (destinationClipIndex == clipIndex))
        return -1;

    int i = m_trackList.at(toTrack).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track)
    {
        Mlt::Playlist playlist(*track);
        if (isTransition(playlist, destinationClipIndex))
            return -1;
        int startOfClip = playlist.clip_start(destinationClipIndex);
        int endOfClip = startOfClip + playlist.clip_length(destinationClipIndex);
        if (abs(position - startOfClip) < abs(endOfClip - position))
        {
            toClipIndex = destinationClipIndex;
            if ( (fromTrack == toTrack) && (destinationClipIndex - 1 == clipIndex) )
                return -1;
            if (destinationClipIndex > 0 && isTransition(playlist, destinationClipIndex - 1))
                return -1;
            if ( (fromTrack == toTrack) && (toClipIndex <= clipIndex))
               clipIndex++;
        }
        else
        {
            toClipIndex = destinationClipIndex + 1;
            if ( (fromTrack == toTrack) && ((destinationClipIndex + 1) == clipIndex) )
                return -1;
            if (destinationClipIndex < playlist.count() - 1  && isTransition(playlist, destinationClipIndex + 1))
                return -1;
            if ( (fromTrack == toTrack) && (toClipIndex <= clipIndex))
               clipIndex++;
        }

        QModelIndex toParentIndex = index(toTrack);
        beginInsertRows(toParentIndex, toClipIndex, toClipIndex);
        playlist.insert(clip->parent(), toClipIndex, clip->get_in(), clip->get_out());
        endInsertRows();


        QModelIndex parentIndex = index(fromTrack);
        // Remove clip on fromTrack
        beginRemoveRows(parentIndex, clipIndex, clipIndex);
        fromPlaylist.remove(clipIndex);
        endRemoveRows();
//        beginInsertRows(parentIndex, clipIndex, clipIndex);
//        fromPlaylist.replace_with_blank(clipIndex);
//        endInsertRows();

    }

    consolidateBlanksAllTracks();
    emit modified();

    return 0;
}


int MultitrackModel::refreshClipFromXmlForFilter(int trackIndex, int clipIndex, QString strXml)
{
    Mlt::Producer *fromTrackProducer = m_tractor->track(m_trackList.at(trackIndex).mlt_index);
    Mlt::Playlist fromPlaylist(*fromTrackProducer);
    delete fromTrackProducer;
    QScopedPointer<Mlt::Producer> clip(fromPlaylist.get_clip(clipIndex));

    Mlt::Producer clip_xml(MLT.profile(), "xml-string", strXml.toUtf8().constData());

    qDebug() << "clip_xml FILTER COUNT=" << clip_xml.filter_count();

    return 0;
}


int MultitrackModel::getStartPositionOfClip(int trackIndex, int clipIndex)
{
    int position = -1;
    int i = m_trackList.at(trackIndex).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        position = playlist.clip_start(clipIndex);
    }

    return position;
}

void MultitrackModel::setClipSpeed(int trackIndex, int clipIndex, double speed)
{
    Q_ASSERT(speed > 0);

    connect(this, SIGNAL(producerChanged(Mlt::Producer*)), &MAIN, SLOT(onProducerChanged()));
    connect(this, SIGNAL(producerChanged(Mlt::Producer*)), MAIN.filterController(), SLOT(setProducer(Mlt::Producer*)));
    connect(this, SIGNAL(producerChanged(Mlt::Producer*)), MAIN.timelineDock(), SLOT(onProducerChanged(Mlt::Producer*)));

    int i = m_trackList.at(trackIndex).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_tractor->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
//        Mlt::Producer *clip = playlist.get_clip(clipIndex);
        Mlt::ClipInfo* i = playlist.clip_info(clipIndex);

        Mlt::Producer *clip = i->producer;

        if (GetSpeedFromProducer(clip) != speed)
        {
            Mlt::Producer *newProducer = producer(clip, MLT.profile(), speed);

//            if (newProducer->is_valid())
//            {
//                delete newProducer;
//                return;
//            }
            newProducer->pass_list(*clip, "audio_index, video_index, force_aspect_ratio,"
                         "video_delay, force_progressive, force_tff,"
                         kAspectRatioNumerator ","
                         kAspectRatioDenominator ","
                         kShotcutHashProperty);
            Mlt::Controller::copyFilters(*clip, *newProducer);


            emit producerChanged(newProducer);
            delete newProducer;
        }

    }
}

Mlt::Producer* MultitrackModel::producer(Mlt::Producer *producer, Mlt::Profile& profile, double speed)
{
    Mlt::Producer* p = NULL;
    if ( speed == 1.0 )
    {
        p = new Mlt::Producer(profile, GetFilenameFromProducer(producer));
    }
    else
    {
        char* filename = GetFilenameFromProducer(producer);
        QString s = QString("%1:%L2:%3").arg("timewarp").arg(speed).arg(filename);
        p = new Mlt::Producer(profile, s.toUtf8().constData());
        p->set(kShotcutProducerProperty, "avformat");
    }
//    if (p->is_valid())
//        p->set("video_delay", double(ui->syncSlider->value()) / 1000);
    return p;
}

void MultitrackModel::debugPrintState()
{
    qDebug("timeline state: {");
    for (int i = 0; i < trackList().count(); ++i)
    {
        int mltIndex = trackList()[i].mlt_index;
        QString trackStr = QString("   track %1 (mlt-idx %2):").arg(i).arg(mltIndex);

        Q_ASSERT(tractor()->track(mltIndex));

        QScopedPointer<Mlt::Producer> trackProducer(tractor()->track(mltIndex));
        Mlt::Playlist playlist(*trackProducer);

        for (int j = 0; j < playlist.count(); ++j) {
            QScopedPointer<Mlt::Producer> clip(playlist.get_clip(j));
            Mlt::ClipInfo info;
            playlist.clip_info(j, &info);
            trackStr += QString(" [ %5 %1 -> %2 (%3 frames) %4]").arg(info.frame_in).arg(info.frame_out).arg(info.frame_count).arg(clip->is_blank() ? "blank " : "").arg(MLT.uuid(*clip).toString());
        }
        LOG_DEBUG() << qPrintable(trackStr);
    }
    qDebug("}");
}
