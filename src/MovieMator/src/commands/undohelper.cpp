/*
 * Copyright (c) 2015-2016 Meltytech, LLC
 * Author: Harald Hvaal <harald.hvaal@gmail.com>
  *
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

#include "undohelper.h"
#include "mltcontroller.h"
#include "models/audiolevelstask.h"
#include "shotcut_mlt_properties.h"
#include <Logger.h>
#include <QScopedPointer>
#include <QUuid>

#include <QFile>

#define UNDOHELPER_DEBUG
#ifdef UNDOHELPER_DEBUG
#define UNDOLOG LOG_DEBUG()
#else
#define UNDOLOG if (false) LOG_DEBUG()
#endif

UndoHelper::UndoHelper(MultitrackModel& model)
    : m_model(model)
    , m_hints(NoHints)
{
}

void UndoHelper::recordBeforeState()
{
#ifdef UNDOHELPER_DEBUG
    debugPrintState();
#endif
    m_state.clear();
    m_clipsAdded.clear();
    m_insertedOrder.clear();
    for (int i = 0; i < m_model.trackList().count(); ++i)
    {
        int mltIndex = m_model.trackList()[i].mlt_index;
        Q_ASSERT(m_model.tractor()->track(mltIndex));
        QScopedPointer<Mlt::Producer> trackProducer(m_model.tractor()->track(mltIndex));
        Mlt::Playlist playlist(*trackProducer);

        for (int j = 0; j < playlist.count(); ++j) {

            Q_ASSERT(playlist.get_clip(j));
            QScopedPointer<Mlt::Producer> clip(playlist.get_clip(j));
            QUuid uid = MLT.ensureHasUuid(*clip);
            m_insertedOrder << uid;
            Info& info = m_state[uid];
            if (!(m_hints & SkipXML))
                info.xml = MLT.XML(&clip->parent());
//            printf("bbb--------%s\n", info.xml.toUtf8().constData());
//            printf("-----------\n");
//            printf("%s\n", MLT.XML(clip.data()).toUtf8().constData());

            Mlt::ClipInfo clipInfo;
            playlist.clip_info(j, &clipInfo);
            info.frame_in = clipInfo.frame_in;
            info.frame_out = clipInfo.frame_out;
            info.oldTrackIndex = i;
            info.oldClipIndex = j;
            info.isBlank = playlist.is_blank(j);
        }
    }
    UNDOLOG << "recordBeforeState end";
}

void UndoHelper::recordAfterState()
{
#ifdef UNDOHELPER_DEBUG
    debugPrintState();
#endif
    QList<QUuid> clipsRemoved = m_state.keys();
    m_clipsAdded.clear();
    for (int i = 0; i < m_model.trackList().count(); ++i)
    {
        int mltIndex = m_model.trackList()[i].mlt_index;

        int trackCount = m_model.tractor()->count();
        Q_ASSERT(mltIndex < trackCount);
        Q_ASSERT(m_model.tractor()->track(mltIndex));

        QScopedPointer<Mlt::Producer> trackProducer(m_model.tractor()->track(mltIndex));
        Mlt::Playlist playlist(*trackProducer);

        for (int j = 0; j < playlist.count(); ++j) {

            Q_ASSERT(playlist.get_clip(j));

            QScopedPointer<Mlt::Producer> clip(playlist.get_clip(j));
            QUuid uid = MLT.ensureHasUuid(*clip);

            /* Clips not previously in m_state are new */
            if (!m_state.contains(uid)) {
                UNDOLOG << "New clip!" << clip;
                m_clipsAdded << uid;
            }
            else {
                Info &info = m_state[uid];
                info.changes = 0;
                info.newTrackIndex = i;
                info.newClipIndex = j;

                /* Indices have changed; these are moved */
                if (info.oldTrackIndex != info.newTrackIndex || info.oldClipIndex != info.newClipIndex) {
                    UNDOLOG << "Clip" << uid << "moved from"
                        << info.oldTrackIndex << info.oldClipIndex
                        << "to"
                        << info.newTrackIndex << info.newClipIndex;
                    info.changes |= Moved;
                }

                if (!(m_hints & SkipXML)) {

                    Q_ASSERT(&clip->parent());

                    QString newXml = MLT.XML(&clip->parent());
                    UNDOLOG << "wzq xml:" << newXml;
                    if (info.xml != newXml) {
                        UNDOLOG << "Modified xml:" << uid;
                        info.changes |= XMLModified;
                    }
                }

                Mlt::ClipInfo newInfo;
                playlist.clip_info(j, &newInfo);
                /* Only in/out point changes are handled at this time. */
                if (info.frame_in != newInfo.frame_in || info.frame_out != newInfo.frame_out) {
                    UNDOLOG << "In/out changed:" << uid;
                    info.changes |= ClipInfoModified;
                }
            }
            clipsRemoved.removeOne(uid);
        }
    }

    /* Clips that did not show up are removed from the timeline */
    foreach (QUuid uid, clipsRemoved) {
        UNDOLOG << "Clip removed:" << uid;
        m_state[uid].changes = Removed;
    }

    UNDOLOG << "recordAfterState end";
}

void UndoHelper::undoChanges()
{
#ifdef UNDOHELPER_DEBUG
    debugPrintState();
#endif

    /* We're walking through the list in the order of uids, which is the order in which the
     * clips were laid out originally. As we go through the clips we make sure the clips behind
     * the current index are as they were originally before we move on to the next one */
    foreach (QUuid uid, m_insertedOrder) {
        //Q_ASSERT()
        const Info& info = m_state[uid];
        UNDOLOG << "Handling uid" << uid << "on track" << info.oldTrackIndex << "index" << info.oldClipIndex;

        /* This is the index in the track we're currently restoring */
        int currentIndex = info.oldClipIndex;

        int mltIndex = m_model.trackList()[info.oldTrackIndex].mlt_index;
        QScopedPointer<Mlt::Producer> trackProducer(m_model.tractor()->track(mltIndex));
        Mlt::Playlist playlist(*trackProducer);

        /* Clips that were moved are simply searched for using the uid, and moved in place. We
         * do not use the indices directly because they become invalid once the playlist is
         * modified. */
        if (info.changes & Moved) {
            Q_ASSERT(info.newTrackIndex == info.oldTrackIndex && "cross-track moves are unsupported so far");
            int clipCurrentlyAt = -1;
            for (int i = 0; i < playlist.count(); ++i) {

                Q_ASSERT(playlist.get_clip(i));
                QScopedPointer<Mlt::Producer> clip(playlist.get_clip(i));
                if (MLT.uuid(*clip) == uid) {
                    clipCurrentlyAt = i;
                    break;
                }
            }
            Q_ASSERT(clipCurrentlyAt != -1 && "Moved clip could not be found");
            UNDOLOG << "Found clip with uid" << uid << "at index" << clipCurrentlyAt;

            if (clipCurrentlyAt != info.oldClipIndex) {
                UNDOLOG << "moving from" << clipCurrentlyAt << "to" << currentIndex;
                QModelIndex modelIndex = m_model.createIndex(clipCurrentlyAt, 0, info.oldTrackIndex);
                m_model.beginMoveRows(modelIndex.parent(), clipCurrentlyAt, clipCurrentlyAt, modelIndex.parent(), currentIndex);
                playlist.move(clipCurrentlyAt, currentIndex);
                m_model.endMoveRows();
            }
        }

        /* Removed clips are reinserted using their stored XML */
        if (info.changes & Removed) {
            QModelIndex modelIndex = m_model.createIndex(currentIndex, 0, info.oldTrackIndex);
            m_model.beginInsertRows(modelIndex.parent(), currentIndex, currentIndex);
            if (info.isBlank) {
                playlist.insert_blank(currentIndex, info.frame_out);
                UNDOLOG << "inserting isBlank at " << currentIndex;
            } else {
                UNDOLOG << "inserting clip at " << currentIndex;
                LOG_DEBUG() << m_hints;
                Q_ASSERT(!(m_hints & SkipXML) && "Cannot restore clip without stored XML");
                Q_ASSERT(!info.xml.isEmpty());


//                QString projectXML =  MLT.XML(m_model.tractor());
//                QFile outFile1("/Users/gdb/Desktop/file1.xml");
//                outFile1.open(QIODevice::WriteOnly);
//                QTextStream ts1(&outFile1);
//                ts1 << projectXML << endl;

                Mlt::Producer restoredClip(MLT.profile(), "xml-string", info.xml.toUtf8().constData());
//                char *sss = "xml-string";
//                Mlt::Tractor restoredClip(MLT.profile(), (char *)sss, (char *)info.xml.toUtf8().constData());

//                mlt_properties_debug(restoredClip.get_properties(), "123456------------", stderr);


//                mlt_service_type type = restoredClip.type();
//                if (type == tractor_type)
//                {
//                Mlt::Tractor tractor(restoredClip);
//                QScopedPointer<Mlt::Producer> clip1(tractor.track(0));
//                mlt_properties_debug(clip1->parent().get_properties(), "123456------------", stderr);
//                tractor.set_track(*playlist.get_clip(currentIndex - 1), 0);
//                tractor.set_track(*playlist.get_clip(currentIndex), 1);
//                m_model.addMixReferences(info.oldTrackIndex, currentIndex);
//                }

                playlist.insert(restoredClip, currentIndex, info.frame_in, info.frame_out);




                if (m_model.isTransition(playlist, currentIndex))
                {

//                    mlt_service_type type = restoredClip.type();
                    Mlt::Tractor tractor(restoredClip);
                    QScopedPointer<Mlt::Producer> clip1(tractor.track(0));
                    QScopedPointer<Mlt::Producer> clip2(tractor.track(1));
//                    mlt_properties_debug(clip1->get_properties(), "123456------------", stderr);
                    Mlt::Producer *trackA = playlist.get_clip(currentIndex - 1)->parent().cut(clip1->get_in(), clip1->get_out());
                    Mlt::Producer *trackB = playlist.get_clip(currentIndex + 1)->parent().cut(clip2->get_in(), clip2->get_out());
//                    tractor.set_track(*playlist.get_clip(currentIndex - 1), 0);
//                    tractor.set_track(*playlist.get_clip(currentIndex + 1), 1);
                    tractor.set_track(*trackA, 0);
                    tractor.set_track(*trackB, 1);
                    m_model.addMixReferences(info.oldTrackIndex, currentIndex);
                }

                QScopedPointer<Mlt::Producer> clipTest(playlist.get_clip(currentIndex));
                MLT.XML(&clipTest->parent());//不能删

//                printf("rrr---------%s\n", MLT.XML(&clipTest->parent()).toUtf8().constData());
//                projectXML =  MLT.XML(m_model.tractor());
//                QFile outFile2("/Users/gdb/Desktop/file2.xml");
//                outFile2.open(QIODevice::WriteOnly);
//                QTextStream ts2(&outFile2);
//                ts2 << projectXML << endl;
            }
            m_model.endInsertRows();

            Q_ASSERT(currentIndex < playlist.count());

            QScopedPointer<Mlt::Producer> clip(playlist.get_clip(currentIndex));

            Q_ASSERT(!clip.isNull());

            MLT.setUuid(*clip, uid);
            AudioLevelsTask::start(clip->parent(), &m_model, modelIndex);
        }

        /* Only in/out points handled so far */
        if (info.changes & ClipInfoModified) {
            UNDOLOG << "resizing clip at" << currentIndex;
            playlist.resize_clip(currentIndex, info.frame_in, info.frame_out);

            QModelIndex modelIndex = m_model.createIndex(currentIndex, 0, info.oldTrackIndex);
            QVector<int> roles;
            roles << MultitrackModel::DurationRole;
            emit m_model.dataChanged(modelIndex, modelIndex, roles);
            QScopedPointer<Mlt::Producer> clip(playlist.get_clip(currentIndex));
            AudioLevelsTask::start(clip->parent(), &m_model, modelIndex);
        }
    }

    /* Finally we walk through the tracks once more, removing clips that
     * were added, and clearing the temporarily used uid property */
    int trackIndex = 0;
    foreach (const Track & track, m_model.trackList()) {

        Q_ASSERT(m_model.tractor()->track(track.mlt_index));

        QScopedPointer<Mlt::Producer> trackProducer(m_model.tractor()->track(track.mlt_index));

        Mlt::Playlist playlist(*trackProducer);
        for (int i = playlist.count() - 1; i >= 0; --i) {

            Q_ASSERT(playlist.get_clip(i));

            QScopedPointer<Mlt::Producer> clip(playlist.get_clip(i));

            Q_ASSERT(!clip.isNull());

            QUuid uid = MLT.uuid(*clip);
//            Q_ASSERT(!uid.isNull());
            if (m_clipsAdded.removeOne(uid) || uid.isNull()) {
                UNDOLOG << "Removing clip at" << i;
                m_model.beginRemoveRows(m_model.index(trackIndex), i, i);
                if (clip->parent().get_data("mlt_mix"))
                    clip->parent().set("mlt_mix", NULL, 0);
                if (clip->get_data("mix_in"))
                    clip->set("mix_in", NULL, 0);
                if (clip->get_data("mix_out"))
                    clip->set("mix_out", NULL, 0);
                playlist.remove(i);
                m_model.endRemoveRows();
            }


        }
        trackIndex++;
    }
    emit m_model.modified();
#ifdef UNDOHELPER_DEBUG
    debugPrintState();
#endif
}

void UndoHelper::setHints(OptimizationHints hints)
{
    m_hints = hints;
}

void UndoHelper::debugPrintState()
{
    qDebug("timeline state: {");
    for (int i = 0; i < m_model.trackList().count(); ++i)
    {
        int mltIndex = m_model.trackList()[i].mlt_index;
        QString trackStr = QString("   track %1 (mlt-idx %2):").arg(i).arg(mltIndex);

        Q_ASSERT(m_model.tractor()->track(mltIndex));

        QScopedPointer<Mlt::Producer> trackProducer(m_model.tractor()->track(mltIndex));
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
