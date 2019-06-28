/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
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

#include "timelinecommands.h"
#include "mltcontroller.h"
#include "shotcut_mlt_properties.h"
#include "mainwindow.h"
#include "controllers/filtercontroller.h"
#include "docks/timelinedock.h"
#include <Logger.h>
#include <QMetaType>
#include <QVariant>
#include <QMetaObject>


typedef struct
{
    double fix_rotate_x;
    double scale_x;
    double scale_y;
    double offsetx;
    double offsety;

}MY_ROTATE_COMMAND;

Q_DECLARE_METATYPE(MY_ROTATE_COMMAND);

namespace Timeline {

AppendClipCommand::AppendClipCommand(MultitrackModel &model, int trackIndex, const QString &xml, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_xml(xml)
    , m_undoHelper(m_model)
{
    setText(QObject::tr("Append to track"));
}


void AppendClipCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex;
    m_undoHelper.recordBeforeState();
    Mlt::Producer producer(MLT.profile(), "xml-string", m_xml.toUtf8().constData());
    Q_ASSERT(producer.is_valid());
    m_model.appendClip(m_trackIndex, producer);
    m_undoHelper.recordAfterState();
}

void AppendClipCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex;
    m_undoHelper.undoChanges();
}

InsertClipCommand::InsertClipCommand(MultitrackModel &model, int trackIndex,
    int position, const QString &xml, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_position(position)
    , m_xml(xml)
    , m_undoHelper(m_model)
{
    setText(QObject::tr("Insert into track"));
}

void InsertClipCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "position" << m_position;
    m_undoHelper.recordBeforeState();
    Mlt::Producer clip(MLT.profile(), "xml-string", m_xml.toUtf8().constData());
    Q_ASSERT(clip.is_valid());
    m_model.insertClip(m_trackIndex, clip, m_position);
    m_undoHelper.recordAfterState();
}

void InsertClipCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "position" << m_position;
    m_undoHelper.undoChanges();
}

OverwriteClipCommand::OverwriteClipCommand(MultitrackModel &model, int trackIndex,
    int position, const QString &xml, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_position(position)
    , m_xml(xml)
    , m_undoHelper(m_model)
{
    setText(QObject::tr("Overwrite onto track"));
}

void OverwriteClipCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "position" << m_position;
    m_undoHelper.recordBeforeState();
    Mlt::Producer clip(MLT.profile(), "xml-string", m_xml.toUtf8().constData());
    Q_ASSERT(clip.is_valid());
    m_playlistXml = m_model.overwrite(m_trackIndex, clip, m_position);
    m_undoHelper.recordAfterState();
}

void OverwriteClipCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "position" << m_position;
    m_undoHelper.undoChanges();
}

LiftClipCommand::LiftClipCommand(MultitrackModel &model, TimelineDock &timeline, int trackIndex,
    int clipIndex, const QString &xml, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_xml(xml)
    , m_undoHelper(m_model)
    , m_timeline(timeline)
{
    setText(QObject::tr("Lift from track"));
    Q_UNUSED(m_timeline);
}

void LiftClipCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex;
    m_undoHelper.recordBeforeState();
//    int clipIndex = m_timeline.removeTransitionOnClip(m_trackIndex, m_clipIndex);
//    m_model.liftClip(m_trackIndex, clipIndex);
    m_model.liftClip(m_trackIndex, m_clipIndex);
    m_undoHelper.recordAfterState();
}

void LiftClipCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex;
    m_undoHelper.undoChanges();
}

RemoveClipCommand::RemoveClipCommand(MultitrackModel &model, TimelineDock& timeline, int trackIndex,
    int clipIndex, const QString &xml, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_xml(xml)
    , m_undoHelper(m_model)
    , m_timeline(timeline)
{
    setText(QObject::tr("Remove from track"));
    Q_UNUSED(m_timeline);
}

void RemoveClipCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex;
    m_undoHelper.recordBeforeState();
//    int clipIndex = m_timeline.removeTransitionOnClip(m_trackIndex, m_clipIndex);
//    m_model.removeClip(m_trackIndex, clipIndex);
    m_model.removeClip(m_trackIndex, m_clipIndex);
    m_undoHelper.recordAfterState();
}

void RemoveClipCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex;
    m_undoHelper.undoChanges();
}


NameTrackCommand::NameTrackCommand(MultitrackModel &model, int trackIndex,
    const QString &name, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_name(name)
    , m_oldName(model.data(m_model.index(trackIndex), MultitrackModel::NameRole).toString())
{
    setText(QObject::tr("Change track name"));
}

void NameTrackCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "name" << m_name;
    m_model.setTrackName(m_trackIndex, m_name);
}

void NameTrackCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "name" << m_name;
    m_model.setTrackName(m_trackIndex, m_oldName);
}

MuteTrackCommand::MuteTrackCommand(MultitrackModel &model, int trackIndex, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_oldValue(model.data(m_model.index(trackIndex), MultitrackModel::IsMuteRole).toBool())
{
    setText(QObject::tr("Toggle track mute"));
}

void MuteTrackCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "mute" << !m_oldValue;
    m_model.setTrackMute(m_trackIndex, !m_oldValue);
}

void MuteTrackCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "mute" << !m_oldValue;
    m_model.setTrackMute(m_trackIndex, m_oldValue);
}

HideTrackCommand::HideTrackCommand(MultitrackModel &model, int trackIndex, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_oldValue(model.data(m_model.index(trackIndex), MultitrackModel::IsHiddenRole).toBool())
{
    setText(QObject::tr("Toggle track hidden"));
}

void HideTrackCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "hide" << !m_oldValue;
    m_model.setTrackHidden(m_trackIndex, !m_oldValue);
}

void HideTrackCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "hide" << !m_oldValue;
    m_model.setTrackHidden(m_trackIndex, m_oldValue);
}

CompositeTrackCommand::CompositeTrackCommand(MultitrackModel &model, int trackIndex, Qt::CheckState value, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_value(value)
    , m_oldValue(Qt::CheckState(model.data(m_model.index(trackIndex), MultitrackModel::IsCompositeRole).toInt()))
{
    setText(QObject::tr("Change track compositing"));
}

void CompositeTrackCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "composite" << m_value;
    m_model.setTrackComposite(m_trackIndex, m_value);
}

void CompositeTrackCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "composite" << m_value;
    m_model.setTrackComposite(m_trackIndex, m_oldValue);
}

LockTrackCommand::LockTrackCommand(MultitrackModel &model, int trackIndex, bool value, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_value(value)
    , m_oldValue(model.data(m_model.index(trackIndex), MultitrackModel::IsLockedRole).toBool())
{
    setText(QObject::tr("Lock track"));
}

void LockTrackCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "lock" << m_value;
    m_model.setTrackLock(m_trackIndex, m_value);
}

void LockTrackCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "lock" << m_value;
    m_model.setTrackLock(m_trackIndex, m_oldValue);
}

MoveClipCommand::MoveClipCommand(MultitrackModel &model, int fromTrackIndex, int toTrackIndex, int clipIndex, int position, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_fromTrackIndex(fromTrackIndex)
    , m_toTrackIndex(toTrackIndex)
    , m_fromClipIndex(clipIndex)
    , m_fromStart(model.data(
        m_model.index(clipIndex, 0, m_model.index(fromTrackIndex)),
            MultitrackModel::StartRole).toInt())
    , m_toStart(position)
    , m_undoHelper(m_model)
{
    setText(QObject::tr("Move clip"));
}

void MoveClipCommand::redo_impl()
{
    LOG_DEBUG() << "fromTrack" << m_fromTrackIndex << "toTrack" << m_toTrackIndex;
    m_undoHelper.recordBeforeState();
    MAIN.timelineDock()->setCurrentTrack(m_toTrackIndex);
    m_model.moveClip(m_fromTrackIndex, m_toTrackIndex, m_fromClipIndex, m_toStart);
    m_undoHelper.recordAfterState();
    qDebug()<<"redo ends";
}

void MoveClipCommand::undo_impl()
{
    LOG_DEBUG() << "fromTrack" << m_fromTrackIndex << "toTrack" << m_toTrackIndex;
    MAIN.timelineDock()->setCurrentTrack(m_fromTrackIndex);
    m_undoHelper.undoChanges();
}

TrimClipInCommand::TrimClipInCommand(MultitrackModel &model, int trackIndex, int clipIndex, int delta, bool ripple, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_originalClipIndex(clipIndex)
    , m_delta(delta)
    , m_ripple(ripple)
    , m_undoHelper(m_model)
{
    setText(QObject::tr("Trim clip in point"));
    if (!ripple)
        m_undoHelper.setHints(UndoHelper::SkipXML);
}

void TrimClipInCommand::redo_impl()
{
    m_undoHelper.recordBeforeState();
    m_clipIndex = m_model.trimClipIn(m_trackIndex, m_originalClipIndex, m_delta, m_ripple);
    m_undoHelper.recordAfterState();
}

void TrimClipInCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "delta" << m_delta;
    m_undoHelper.undoChanges();
}

bool TrimClipInCommand::mergeWith(const QUndoCommand *other)
{
    const TrimClipInCommand* that = static_cast<const TrimClipInCommand*>(other);
    Q_ASSERT(that);
    if (!that) {
        return false;
    }
    if (that->id() != id() || that->m_trackIndex != m_trackIndex || (that->m_clipIndex != m_clipIndex && that->m_clipIndex != m_originalClipIndex))
        return false;
    m_undoHelper.recordAfterState();
    m_delta += static_cast<const TrimClipInCommand*>(other)->m_delta;
    return true;
}

TrimClipOutCommand::TrimClipOutCommand(MultitrackModel &model, int trackIndex, int clipIndex, int delta, bool ripple, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_delta(delta)
    , m_ripple(ripple)
    , m_undoHelper(m_model)
{
    setText(QObject::tr("Trim clip out point"));
    if (!ripple)
        m_undoHelper.setHints(UndoHelper::SkipXML);
}

void TrimClipOutCommand::redo_impl()
{
    m_undoHelper.recordBeforeState();
    m_clipIndex = m_model.trimClipOut(m_trackIndex, m_clipIndex, m_delta, m_ripple);
    m_undoHelper.recordAfterState();
}

void TrimClipOutCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "delta" << m_delta;
    m_undoHelper.undoChanges();
}

bool TrimClipOutCommand::mergeWith(const QUndoCommand *other)
{
    const TrimClipOutCommand* that = static_cast<const TrimClipOutCommand*>(other);
    Q_ASSERT(that);
    if (!that) {
        return false;
    }
    if (that->id() != id() || that->m_trackIndex != m_trackIndex || that->m_clipIndex != m_clipIndex)
        return false;
    m_undoHelper.recordAfterState();
    m_delta += static_cast<const TrimClipOutCommand*>(other)->m_delta;
    return true;
}

SplitCommand::SplitCommand(MultitrackModel &model, int trackIndex,
    int clipIndex, int position, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_position(position)
{
    setText(QObject::tr("Split clip"));
}

void SplitCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "position" << m_position;
    m_model.splitClip(m_trackIndex, m_clipIndex, m_position);
}

void SplitCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "position" << m_position;
    m_model.joinClips(m_trackIndex, m_clipIndex);
}

FadeInCommand::FadeInCommand(MultitrackModel &model, int trackIndex, int clipIndex, int duration, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_duration(duration)
{
    QModelIndex modelIndex = m_model.index(clipIndex, 0, m_model.index(trackIndex));
    m_previous = model.data(modelIndex, MultitrackModel::FadeInRole).toInt();
    setText(QObject::tr("Adjust fade in"));
}

void FadeInCommand::redo_impl()
{
    m_model.fadeIn(m_trackIndex, m_clipIndex, m_duration);
}

void FadeInCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "duration" << m_duration;
    m_model.fadeIn(m_trackIndex, m_clipIndex, m_previous);
}

bool FadeInCommand::mergeWith(const QUndoCommand *other)
{
    const FadeInCommand* that = static_cast<const FadeInCommand*>(other);
    Q_ASSERT(that);
    if (!that) {
        return false;
    }
    if (that->id() != id() || that->m_trackIndex != m_trackIndex || that->m_clipIndex != m_clipIndex)
        return false;
    m_duration = static_cast<const FadeInCommand*>(other)->m_duration;
    return true;
}

FadeOutCommand::FadeOutCommand(MultitrackModel &model, int trackIndex, int clipIndex, int duration, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_duration(duration)
{
    QModelIndex modelIndex = m_model.index(clipIndex, 0, m_model.index(trackIndex));
    m_previous = model.data(modelIndex, MultitrackModel::FadeOutRole).toInt();
    setText(QObject::tr("Adjust fade out"));
}

void FadeOutCommand::redo_impl()
{
    m_model.fadeOut(m_trackIndex, m_clipIndex, m_duration);
}

void FadeOutCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "duration" << m_duration;
    m_model.fadeOut(m_trackIndex, m_clipIndex, m_previous);
}

bool FadeOutCommand::mergeWith(const QUndoCommand *other)
{
    const FadeOutCommand* that = static_cast<const FadeOutCommand*>(other);
    Q_ASSERT(that);
    if (!that) {
        return false;
    }
    if (that->id() != id() || that->m_trackIndex != m_trackIndex || that->m_clipIndex != m_clipIndex)
        return false;
    m_duration = static_cast<const FadeOutCommand*>(other)->m_duration;
    return true;
}

AddTransitionCommand::AddTransitionCommand(MultitrackModel &model, int trackIndex, int clipIndex, int position, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_position(position)
    , m_transitionIndex(-1)
    , m_undoHelper(model)
{
    setText(QObject::tr("Add transition"));
}

void AddTransitionCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "position" << m_position;
    m_undoHelper.recordBeforeState();
    m_transitionIndex = m_model.addTransition(m_trackIndex, m_clipIndex, m_position);
    MAIN.timelineDock()->emitSelectedFromSelection();
    m_undoHelper.recordAfterState();
}

void AddTransitionCommand::undo_impl()
{
    m_undoHelper.undoChanges();
    MAIN.timelineDock()->emitSelectedFromSelection();
//    if (m_transitionIndex >= 0) {
//        LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "position" << m_position;
//        m_model.removeTransition(m_trackIndex, m_transitionIndex);
//        // Delete the blank that was inserted.
//        int i = m_model.trackList().at(m_trackIndex).mlt_index;
//        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
//        if (track) {
//            Mlt::Playlist playlist(*track);
//            if (playlist.is_blank(m_clipIndex + 1) && m_transitionIndex == m_clipIndex) { // dragged left
//                m_model.removeClip(m_trackIndex, m_clipIndex + 1);
//            } else if (playlist.is_blank(m_clipIndex)) {
//                m_model.removeClip(m_trackIndex, m_clipIndex);
//            }
//        }
//    } else {
//        LOG_WARNING() << "Failed to undo the transition!";
//    }
}

TrimTransitionInCommand::TrimTransitionInCommand(MultitrackModel &model, int trackIndex, int clipIndex, int delta, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_delta(delta)
    , m_notify(false)
{
    setText(QObject::tr("Trim transition in point"));
}

void TrimTransitionInCommand::redo_impl()
{
    m_model.trimTransitionIn(m_trackIndex, m_clipIndex, m_delta);
    if (m_notify && m_clipIndex >= 0)
        m_model.notifyClipIn(m_trackIndex, m_clipIndex);
}

void TrimTransitionInCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "delta" << m_delta;
    if (m_clipIndex >= 0) {
        m_model.trimTransitionIn(m_trackIndex, m_clipIndex, -m_delta);
        m_model.notifyClipIn(m_trackIndex, m_clipIndex);
        m_notify = true;
    }
    else LOG_WARNING() << "invalid clip index" << m_clipIndex;
}

bool TrimTransitionInCommand::mergeWith(const QUndoCommand *other)
{
    const TrimTransitionInCommand* that = static_cast<const TrimTransitionInCommand*>(other);
    Q_ASSERT(that);
    if (!that) {
        return false;
    }
    if (that->id() != id() || that->m_trackIndex != m_trackIndex || that->m_clipIndex != m_clipIndex)
        return false;
    m_delta += static_cast<const TrimTransitionInCommand*>(other)->m_delta;
    return true;
}

TrimTransitionOutCommand::TrimTransitionOutCommand(MultitrackModel &model, int trackIndex, int clipIndex, int delta, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_delta(delta)
    , m_notify(false)
{
    setText(QObject::tr("Trim transition out point"));
}

void TrimTransitionOutCommand::redo_impl()
{
    m_model.trimTransitionOut(m_trackIndex, m_clipIndex, m_delta);
    if (m_notify && m_clipIndex >= 0)
        m_model.notifyClipOut(m_trackIndex, m_clipIndex);
}

void TrimTransitionOutCommand::undo_impl()
{
    if (m_clipIndex >= 0) {
        LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "delta" << m_delta;
        m_model.trimTransitionOut(m_trackIndex, m_clipIndex, -m_delta);
        m_model.notifyClipOut(m_trackIndex, m_clipIndex);
        m_notify = true;
    }
    else LOG_WARNING() << "invalid clip index" << m_clipIndex;
}

bool TrimTransitionOutCommand::mergeWith(const QUndoCommand *other)
{
    const TrimTransitionOutCommand* that = static_cast<const TrimTransitionOutCommand*>(other);
    Q_ASSERT(that);
    if (!that) {
        return false;
    }
    if (that->id() != id() || that->m_trackIndex != m_trackIndex || that->m_clipIndex != m_clipIndex)
        return false;
    m_delta += static_cast<const TrimTransitionOutCommand*>(other)->m_delta;
    return true;
}

AddTransitionByTrimInCommand::AddTransitionByTrimInCommand(MultitrackModel &model, int trackIndex, int clipIndex, int delta, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_delta(delta)
    , m_notify(false)
{
    setText(QObject::tr("Add transition"));
}

void AddTransitionByTrimInCommand::redo_impl()
{
    m_model.addTransitionByTrimIn(m_trackIndex, m_clipIndex, m_delta);
    if (m_notify && m_clipIndex > 0)
        m_model.notifyClipOut(m_trackIndex, m_clipIndex - 1);
}

void AddTransitionByTrimInCommand::undo_impl()
{
    if (m_clipIndex > 0) {
        LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "delta" << m_delta;
        m_model.removeTransitionByTrimIn(m_trackIndex, m_clipIndex, m_delta);
        m_notify = true;
    }
    else LOG_WARNING() << "invalid clip index" << m_clipIndex;
}

bool AddTransitionByTrimInCommand::mergeWith(const QUndoCommand *other)
{
    const AddTransitionByTrimInCommand* that = static_cast<const AddTransitionByTrimInCommand*>(other);
    Q_ASSERT(that);
    if (!that) {
        return false;
    }
    if (that->id() != id() || that->m_trackIndex != m_trackIndex ||
        (that->m_clipIndex != m_clipIndex && m_clipIndex != that->m_clipIndex - 1))
        return false;
    return true;
}

AddTransitionByTrimOutCommand::AddTransitionByTrimOutCommand(MultitrackModel &model, int trackIndex, int clipIndex, int delta, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_delta(delta)
    , m_notify(false)
{
    setText(QObject::tr("Add transition"));
}

void AddTransitionByTrimOutCommand::redo_impl()
{
    m_model.addTransitionByTrimOut(m_trackIndex, m_clipIndex, m_delta);
    if (m_notify)
        m_model.notifyClipIn(m_trackIndex, m_clipIndex + 2);
}

void AddTransitionByTrimOutCommand::undo_impl()
{
    if (m_clipIndex + 2 < m_model.rowCount(m_model.index(m_trackIndex))) {
        LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "delta" << m_delta;
        m_model.removeTransitionByTrimOut(m_trackIndex, m_clipIndex, m_delta);
        m_notify = true;
    }
    else LOG_WARNING() << "invalid clip index" << m_clipIndex;
}

bool AddTransitionByTrimOutCommand::mergeWith(const QUndoCommand *other)
{
    const AddTransitionByTrimOutCommand* that = static_cast<const AddTransitionByTrimOutCommand*>(other);
    Q_ASSERT(that);
    if (!that) {
        return false;
    }
    if (that->id() != id() || that->m_trackIndex != m_trackIndex || that->m_clipIndex != m_clipIndex)
        return false;
    return true;
}

AddTrackCommand::AddTrackCommand(MultitrackModel& model, TrackType trackType, AbstractCommand* parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackType(trackType)
{
    if (trackType == VideoTrackType)
        setText(QObject::tr("Add video track"));
    else if (trackType == AudioTrackType)
        setText(QObject::tr("Add audio track"));
    else if (trackType == FilterTrackType)
        setText(QObject::tr("Add filter track"));
    else if (trackType == TextTrackType)
        setText(QObject::tr("Add text track"));
}

void AddTrackCommand::redo_impl()
{
    LOG_DEBUG() << (m_isVideo? "video" : "audio");
//    if (m_isVideo)

    if (m_trackType == VideoTrackType)
        m_trackIndex = m_model.addVideoTrack();
    else if (m_trackType == AudioTrackType)
        m_trackIndex = m_model.addAudioTrack();
    else if (m_trackType == FilterTrackType)
        m_trackIndex = m_model.addFilterTrack();
    else if(m_trackType == TextTrackType)
        m_trackIndex = m_model.addTextTrack();
}

void AddTrackCommand::undo_impl()
{
    LOG_DEBUG() << (m_isVideo? "video" : "audio");
    m_model.removeTrack(m_trackIndex);
}

InsertTrackCommand::InsertTrackCommand(MultitrackModel& model, int trackIndex, AbstractCommand* parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_trackType(model.trackList().size() > 0 ? model.trackList().at(trackIndex).type : VideoTrackType)
{
    if (m_trackType == AudioTrackType)
        setText(QObject::tr("Insert audio track"));
    else if (m_trackType == VideoTrackType)
        setText(QObject::tr("Insert video track"));
}

void InsertTrackCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "type" << (m_trackType == AudioTrackType? "audio" : "video");
    m_model.insertTrack(m_trackIndex, m_trackType);
}

void InsertTrackCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "type" << (m_trackType == AudioTrackType? "audio" : "video");
    m_model.removeTrack(m_trackIndex);
}

RemoveTrackCommand::RemoveTrackCommand(MultitrackModel& model, int trackIndex, AbstractCommand* parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_trackType(model.trackList().at(trackIndex).type)
{
    if (m_trackType == AudioTrackType)
        setText(QObject::tr("Remove audio track"));
    else if (m_trackType == VideoTrackType)
        setText(QObject::tr("Remove video track"));

    // Save track XML.
    Q_ASSERT(m_trackIndex >= 0);
    Q_ASSERT(m_trackIndex < m_model.trackList().size());
    int mlt_index = m_model.trackList().at(m_trackIndex).mlt_index;
    Q_ASSERT(m_model.tractor());
    Q_ASSERT(m_model.tractor()->multitrack());
    QScopedPointer<Mlt::Producer> producer(m_model.tractor()->multitrack()->track(mlt_index));
    Q_ASSERT(producer);
    Q_ASSERT(producer->is_valid());
    if (producer && producer->is_valid()) {
        m_xml = MLT.XML(producer.data());
        m_trackName = QString::fromUtf8(producer->get(kTrackNameProperty));
    }
}

void RemoveTrackCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "type" << (m_trackType == AudioTrackType? "audio" : "video");
    LOG_DEBUG() << "Remove track begin *****";
    m_model.removeTrack(m_trackIndex);
    LOG_DEBUG() << "Remove track end *****";
}

void RemoveTrackCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "type" << (m_trackType == AudioTrackType? "audio" : "video");
    m_model.insertTrack(m_trackIndex, m_trackType);
    m_model.setTrackName(m_trackIndex, m_trackName);

    // Restore track from XML.
    Mlt::Producer producer(MLT.profile(), "xml-string", m_xml.toUtf8().constData());
    Q_ASSERT(producer.is_valid());
    Mlt::Playlist playlist(producer);
    Q_ASSERT(playlist.is_valid());
    m_model.appendFromPlaylist(&playlist, m_trackIndex);

    // Re-attach filters.
    int n = playlist.filter_count();
    if (n > 0) {
        Q_ASSERT(m_trackIndex >= 0);
        Q_ASSERT(m_trackIndex < m_model.trackList().size());
        int mlt_index = m_model.trackList().at(m_trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        Q_ASSERT(m_model.tractor()->multitrack());
        QScopedPointer<Mlt::Producer> producer(m_model.tractor()->multitrack()->track(mlt_index));
        Q_ASSERT(producer);
        Q_ASSERT(producer->is_valid());
        for (int i = 0; i < n; ++i) {
            QScopedPointer<Mlt::Filter> filter(playlist.filter(i));
            Q_ASSERT(filter);
            Q_ASSERT(filter->is_valid());
            if (filter && filter->is_valid())
                producer->attach(*filter);
        }
    }
}

ChangeBlendModeCommand::ChangeBlendModeCommand(Mlt::Transition& transition, const QString& propertyName, const QString& mode, AbstractCommand* parent)
    : AbstractCommand(parent)
    , m_transition(transition)
    , m_propertyName(propertyName)
    , m_newMode(mode)
{
    setText(QObject::tr("Change track blend mode"));
    m_oldMode = m_transition.get(m_propertyName.toLatin1().constData());
}

void ChangeBlendModeCommand::redo_impl()
{
    LOG_DEBUG() << "mode" << m_newMode;
    if (!m_newMode.isEmpty()) {
        m_transition.set(m_propertyName.toLatin1().constData(), m_newMode.toUtf8().constData());
        MLT.refreshConsumer();
        emit modeChanged(m_newMode);
    }
}

void ChangeBlendModeCommand::undo_impl()
{
    LOG_DEBUG() << "mode" << m_newMode;
    if (!m_oldMode.isEmpty()) {
        m_transition.set(m_propertyName.toLatin1().constData(), m_oldMode.toUtf8().constData());
        MLT.refreshConsumer();
        emit modeChanged(m_oldMode);
    }
}

UpdateClipCommand::UpdateClipCommand(TimelineDock& timeline, int trackIndex, int clipIndex,
    int position, AbstractCommand* parent)
    : AbstractCommand(parent)
    , m_timeline(timeline)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_position(position)
    , m_isFirstRedo(true)
    , m_isSpeedChanged(false)
    , m_undoHelper(*timeline.model())
{
    setText(QObject::tr("Change clip properties"));
    m_undoHelper.recordBeforeState();
}

void UpdateClipCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "position" << m_position;
    if (!m_isFirstRedo)
        m_undoHelper.recordBeforeState();
    Mlt::Producer clip(MLT.profile(), "xml-string", m_xmlAfter.toUtf8().constData());
    Q_ASSERT(clip.is_valid());
    Q_ASSERT(m_timeline.model());
//    m_timeline.model()->liftClip(m_trackIndex, m_clipIndex);
//    m_timeline.model()->overwrite(m_trackIndex, clip, m_position, false);
    m_timeline.model()->removeClip(m_trackIndex, m_clipIndex);
    int newClipIndex = m_timeline.model()->insertClip(m_trackIndex, clip, m_position);

    //更新转场信息
    if (m_isSpeedChanged) {
        m_timeline.model()->updateTransition(m_trackIndex, newClipIndex);
    }
    m_undoHelper.recordAfterState();
}

void UpdateClipCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "position" << m_position;
    m_undoHelper.undoChanges();
    m_timeline.emitSelectedFromSelection();
    m_isFirstRedo = false;
}

RemoveTransitionCommand::RemoveTransitionCommand(MultitrackModel &model, int trackIndex, int clipIndex, int transitionIndex, int position, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_transitionIndex(transitionIndex)
    , m_position(position)
    , m_undoHelper(model)
{
    setText(QObject::tr("Remove transition"));
}

void RemoveTransitionCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "transitionIndex" << m_transitionIndex << "position" << m_position;
    m_undoHelper.recordBeforeState();
    m_model.removeTransition(m_trackIndex, m_transitionIndex);
    m_undoHelper.recordAfterState();
}

void RemoveTransitionCommand::undo_impl()
{
//    QString projectXML =  MLT.XML(m_model.tractor());
//    QFile outFile1("/Users/gdb/Desktop/file1.xml");
//    outFile1.open(QIODevice::WriteOnly);
//    QTextStream ts1(&outFile1);
//    ts1 << projectXML << endl;

    m_undoHelper.undoChanges();

//    projectXML =  MLT.XML(m_model.tractor());
//    QFile outFile2("/Users/gdb/Desktop/file2.xml");
//    outFile2.open(QIODevice::WriteOnly);
//    QTextStream ts2(&outFile2);
//    ts2 << projectXML << endl;


//    m_model.addTransition(m_trackIndex, m_clipIndex, m_position);
//    // Delete the blank that was inserted.
//    int i = m_model.trackList().at(m_trackIndex).mlt_index;
//    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
//    if (track) {
//        Mlt::Playlist playlist(*track);
//        if (playlist.is_blank(m_clipIndex + 2) && m_transitionIndex == m_clipIndex) { // dragged left
//            m_model.removeClip(m_trackIndex, m_clipIndex + 2);
//        }
//    }
}

RemoveTransitionsOnClipCommand::RemoveTransitionsOnClipCommand(MultitrackModel &model, TimelineDock &timeline, int trackIndex, int clipIndex, AbstractCommand *parent)
    :AbstractCommand(parent)
    , m_model(model)
    , m_timeline(timeline)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_undoHelper(m_model)
{
    setText("Remove transition");
}

void RemoveTransitionsOnClipCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex;
    m_undoHelper.recordBeforeState();
    m_timeline.removeTransitionOnClip(m_trackIndex, m_clipIndex);
    m_undoHelper.recordAfterState();
}

void RemoveTransitionsOnClipCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex;
    m_undoHelper.undoChanges();
}

MoveInsertClipCommand::MoveInsertClipCommand(MultitrackModel &model, int fromTrack, int toTrack, int clipIndex, int position, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_fromTrackIndex(fromTrack)
    , m_toTrackIndex(toTrack)
    , m_clipIndex(clipIndex)
    , m_position(position)
    , m_undoHelper(m_model)
{
    setText(QObject::tr("Move clip"));
}

void MoveInsertClipCommand::redo_impl()
{
    LOG_DEBUG() << "fromTrack" << m_fromTrackIndex << "toTrack" << m_toTrackIndex << "clipIndex" << m_clipIndex << "position" << m_position;
    m_undoHelper.recordBeforeState();
    m_model.moveInsertClip(m_fromTrackIndex, m_toTrackIndex, m_clipIndex, m_position);
    m_undoHelper.recordAfterState();
}

void MoveInsertClipCommand::undo_impl()
{
    LOG_DEBUG() << "fromTrack" << m_fromTrackIndex << "toTrack" << m_toTrackIndex << "clipIndex" << m_clipIndex << "position" << m_position;
    m_undoHelper.undoChanges();
}




FilterCommand::FilterCommand(Mlt::Filter* filter, QString name, double from_value, double to_value, AbstractCommand * parent)
 : AbstractCommand(parent)
{
    setText(QObject::tr("FilterCommand"));
    m_bFirstExec = true;
    LOG_DEBUG() << "FilterCommand: " <<  name;
    Q_ASSERT(filter);
    m_filter = new Mlt::Filter(filter->get_filter());
    m_keyName   = name;

    int transed_filter = 0;
    if(name.startsWith("transition.") == true)
    {
        QVariant varFrom, varTo;
        if(0 == transitionValue(varFrom, varTo, m_filter, name, from_value, to_value))
        {
            transed_filter = 1;
            m_from_value    = varFrom;
            m_to_value      = varTo;
        }
    }

    if(transed_filter == 0)
    {
        m_from_value    = QVariant(from_value);
        m_to_value      = QVariant(to_value);
    }
}

FilterCommand::FilterCommand(Mlt::Filter* filter, QString name,  int from_value, int to_value, AbstractCommand * parent)
: AbstractCommand(parent)
{
    setText(QObject::tr("FilterCommand"));
    m_bFirstExec = true;
    Q_ASSERT(filter);
    m_filter = new Mlt::Filter(filter->get_filter());
    m_keyName   = name;
    m_from_value    = QVariant(from_value);
    m_to_value      = QVariant(to_value);
}

FilterCommand::FilterCommand(Mlt::Filter* filter, QString name,  QString from_value, QString to_value, AbstractCommand * parent)
: AbstractCommand(parent)
{
    setText(QObject::tr("FilterCommand"));
    m_bFirstExec = true;
    Q_ASSERT(filter);
    m_filter = new Mlt::Filter(filter->get_filter());
    m_keyName   = name;
    m_from_value    = QVariant(from_value);
    m_to_value      = QVariant(to_value);
}

FilterCommand::FilterCommand(Mlt::Filter* filter, QString name,  QRectF from_value, QRectF to_value, AbstractCommand * parent)
: AbstractCommand(parent)
{
    setText(QObject::tr("FilterCommand"));
    m_bFirstExec = true;
    Q_ASSERT(filter);
    m_filter = new Mlt::Filter(filter->get_filter());
    m_keyName   = name;
    m_from_value = QVariant(from_value);
    m_to_value      = QVariant(to_value);
}

FilterCommand::~FilterCommand()
{
    delete m_filter;
}

static const char *s_key_name[5] ={"transition.fix_rotate_x", "transition.scale_x", "transition.scale_y",
                                   "transition.offsetx", "transition.offsety"};

static int which_rotate_command(QString strKey)
{
    if(strKey.startsWith("transition.") == false)
        return -1;

    for(int i=0; i<5; i++)
    {
        if(strKey == QString(s_key_name[i]))
        {
            return i;
        }
    }

    return -2;
}

int FilterCommand::transitionValue(QVariant &varFrom, QVariant &varTo, Mlt::Filter* filter, QString name,  double from_value, double to_value)
{
    if(name.startsWith("transition.") == false)
        return -1;

    int found_key = which_rotate_command(name);
    if(found_key < 0 )  return -2;

    Q_ASSERT(filter);
    if (!filter) {
        return -1;
    }

    double value_from[5], value_to[5];
    for(int i=0; i<5; i++)
    {
        if(!filter->get(s_key_name[i]))
        {
            if(strcmp(s_key_name[i], "transition.scale_x") == 0
                || strcmp(s_key_name[i], "transition.scale_y") == 0)
                value_from[i]   =  1.0;
            else value_from[i]   =  0.0;
        }
        else
            value_from[i]   = filter->get_double(s_key_name[i]);

        value_to[i]     = value_from[i];
        if(found_key == i)
        {
            found_key = i;
            value_from[i]   = from_value;
            value_to[i]     = to_value;
        }
    }



    MY_ROTATE_COMMAND rotateCmd_from, rotateCmd_to;


    rotateCmd_from.fix_rotate_x = value_from[0];
    rotateCmd_from.scale_x      = value_from[1];
    rotateCmd_from.scale_y      = value_from[2];
    rotateCmd_from.offsetx      = value_from[3];
    rotateCmd_from.offsety      = value_from[4];

    rotateCmd_to.fix_rotate_x = value_to[0];
    rotateCmd_to.scale_x      = value_to[1];
    rotateCmd_to.scale_y      = value_to[2];
    rotateCmd_to.offsetx      = value_to[3];
    rotateCmd_to.offsety      = value_to[4];

    varFrom = QVariant::fromValue(rotateCmd_from);
    varTo = QVariant::fromValue(rotateCmd_to);

    return 0;
}


bool FilterCommand::mergeWith(const QUndoCommand *other)
{
//    return false;
    Q_ASSERT(other);
    if (!other) {
        return false;
    }
    if (other->id() != id()) // make sure other is also an AppendText command
              return false;

    const FilterCommand *other_command =  static_cast<const FilterCommand*>(other);
    Q_ASSERT(other_command);
    if (!other_command) {
        return false;
    }
    if(other_command->m_filter->get_filter() != m_filter->get_filter()|| m_keyName.isEmpty())
        return false;

    if((which_rotate_command(m_keyName) >=0 && which_rotate_command(other_command->m_keyName) >= 0
        && m_to_value.canConvert<MY_ROTATE_COMMAND>() && other_command->m_to_value.canConvert<MY_ROTATE_COMMAND>())
            || other_command->m_keyName == m_keyName)
    {
        m_to_value = other_command->m_to_value;

        return true;
    }

    return false;
}

void FilterCommand::notify()
{
    MLT.refreshConsumer();
    MAIN.filterController()->refreshCurrentFilter(m_filter);


    //emit MAIN.filterController()->attachedModel()->changed();
    //emit MAIN.timelineDock()->positionChanged();
}

void FilterCommand::set_value(QVariant value)
{
    QVariant::Type value_type = value.type();

    if (value.canConvert<MY_ROTATE_COMMAND>())
    {
        MY_ROTATE_COMMAND rotateCmd = value.value<MY_ROTATE_COMMAND>();

        m_filter->set(s_key_name[0], rotateCmd.fix_rotate_x);
        m_filter->set(s_key_name[1], rotateCmd.scale_x);
        m_filter->set(s_key_name[2], rotateCmd.scale_y);
        m_filter->set(s_key_name[3], rotateCmd.offsetx);
        m_filter->set(s_key_name[4], rotateCmd.offsety);

        return;
    }

    if(value_type == QVariant::Double)
    {
        m_filter->set(m_keyName.toUtf8().constData(), value.toDouble());
    }
    else if(value_type == QVariant::Int)
    {
         m_filter->set(m_keyName.toUtf8().constData(), value.toInt());
    }
    else if(value_type == QVariant::String)
    {
        m_filter->set(m_keyName.toUtf8().constData(), value.toString().toUtf8().constData());
    }
    else if(value_type == QVariant::RectF)
    {
        QRectF rectF = value.toRectF();

        m_filter->set(m_keyName.toUtf8().constData(), double(rectF.left()), double(rectF.top()), double(rectF.width()), double(rectF.height()), 1.0);
    }

}

void FilterCommand::redo_impl()
{
    if(m_bFirstExec)//第一次自动执行不调用，外部已经执行
    {
        m_bFirstExec = false;
        return;
    }
    set_value(m_to_value);
    notify();
}

void FilterCommand::undo_impl()
{
    set_value(m_from_value);
    notify();
}


//增加关键帧
KeyFrameInsertCommand::KeyFrameInsertCommand(Mlt::Filter* filter, const QVector<key_frame_item>  &from_value, const QVector<key_frame_item> &insert_value, AbstractCommand *parent)
: AbstractCommand(parent)
,m_from_value(from_value)
,m_insert_value(insert_value)
{
    setText(QObject::tr("KeyFrameInsertCommand"));
    Q_ASSERT(filter);

    m_filter        = new Mlt::Filter(filter->get_filter());

    m_bFirstExec    = true;
    m_execTime.start();
}

KeyFrameInsertCommand::~KeyFrameInsertCommand()
{
    delete m_filter;
}

void KeyFrameInsertCommand::redo_impl()
{
    if(m_bFirstExec)//第一次自动执行不调用，外部已经执行
    {
        m_bFirstExec = false;
        return;
    }

    Q_ASSERT(MAIN.filterController());
    MAIN.filterController()->insertKeyFrame(m_filter, m_insert_value);
}

void KeyFrameInsertCommand::undo_impl()
{
    Q_ASSERT(MAIN.filterController());
    MAIN.filterController()->removeKeyFrame(m_filter, m_insert_value);

    MAIN.filterController()->refreshNoAnimation(m_filter, m_from_value);
}

bool KeyFrameInsertCommand::mergeWith(const QUndoCommand *other)
{
    Q_ASSERT(other);
    if (!other)                     return false;
    if (other->id() != id())        return false;

    const KeyFrameInsertCommand *other_command =  static_cast<const KeyFrameInsertCommand*>(other);
    Q_ASSERT(other_command);
    if(other_command->m_filter->get_filter() != m_filter->get_filter())     return false;

    if(m_execTime.elapsed() > 1000)   //不合并
    {
        m_execTime.restart();
        return false;
    }

    //合并m_insert_value
    key_frame_item newPara = other_command->m_insert_value.at(0);

    int nIndex = 0;
    for(nIndex = 0; nIndex < m_insert_value.count(); nIndex++)
    {
        key_frame_item para = m_insert_value.at(nIndex);
        if(para.keyFrame == newPara.keyFrame)
        {
            QMap<QString, QString>::iterator iter = newPara.paraMap.begin();
            para.paraMap.insert(iter.key(), iter.value());
            m_insert_value.removeAt(nIndex);
            m_insert_value.insert(nIndex, para);
            break;
        }
    }
    if(nIndex == m_insert_value.count()) //没有存储,没有已经为keyFrame这一帧的存储数据
    {
        m_insert_value.insert(nIndex, other_command->m_insert_value.at(0));
    }

    //合并m_from_value:这里可以考虑进一步优化，优化只存储非动画时候的值，因为这个值存下来只是非动画增加帧变成动画时需要用（undo时用到），可优化的地方：存储这个值+mergeWith两个地方。
    key_frame_item newFromPara = other_command->m_from_value.at(0);

    for(nIndex = 0; nIndex < m_from_value.count(); nIndex++)
    {
        key_frame_item para = m_from_value.at(nIndex);
        if(para.keyFrame == newFromPara.keyFrame)
        {
            QMap<QString, QString>::iterator iter = newFromPara.paraMap.begin();
            para.paraMap.insert(iter.key(), iter.value());
            m_from_value.removeAt(nIndex);
            m_from_value.insert(nIndex, para);
            break;
        }
    }
    if(nIndex == m_from_value.count()) //没有存储,没有已经为keyFrame这一帧的存储数据
    {
        m_from_value.insert(nIndex, other_command->m_from_value.at(0));
    }

    m_execTime.restart();

    return true;
}


//删除关键帧
KeyFrameRemoveCommand::KeyFrameRemoveCommand(Mlt::Filter* filter, const QVector<key_frame_item> &remove_value, AbstractCommand *parent)
: AbstractCommand(parent)
,m_remove_value(remove_value)
{
    setText(QObject::tr("KeyFrameRemoveCommand"));
    Q_ASSERT(filter);

    m_filter        = new Mlt::Filter(filter->get_filter());

    m_bFirstExec    = true;
    m_execTime.start();
}

KeyFrameRemoveCommand::~KeyFrameRemoveCommand()
{
    delete m_filter;
}

void KeyFrameRemoveCommand::redo_impl()
{
    if(m_bFirstExec)//第一次自动执行不调用，外部已经执行
    {
        m_bFirstExec = false;
        return;
    }

    Q_ASSERT(MAIN.filterController());
    MAIN.filterController()->removeKeyFrame(m_filter, m_remove_value);
}

void KeyFrameRemoveCommand::undo_impl()
{
    Q_ASSERT(MAIN.filterController());
    MAIN.filterController()->insertKeyFrame(m_filter, m_remove_value);
}

bool KeyFrameRemoveCommand::mergeWith(const QUndoCommand *other)
{
    Q_ASSERT(other);
    if (!other)                     return false;
    if (other->id() != id())        return false;

    const KeyFrameRemoveCommand *other_command =  static_cast<const KeyFrameRemoveCommand*>(other);
    Q_ASSERT(other_command);
    if(other_command->m_filter->get_filter() != m_filter->get_filter())     return false;

    if(m_execTime.elapsed() > 1000)   //不合并
    {
        m_execTime.restart();
        return false;
    }

    //合并
    key_frame_item newPara = other_command->m_remove_value.at(0);

    int nIndex = 0;
    for(nIndex = 0; nIndex < m_remove_value.count(); nIndex++)
    {
        key_frame_item para = m_remove_value.at(nIndex);
        if(para.keyFrame == newPara.keyFrame)
        {
            QMap<QString, QString>::iterator iter = newPara.paraMap.begin();
            para.paraMap.insert(iter.key(), iter.value());
            m_remove_value.removeAt(nIndex);
            m_remove_value.insert(nIndex, para);
            break;
        }
    }
    if(nIndex == m_remove_value.count()) //没有存储,没有已经为keyFrame这一帧的存储数据
    {
        m_remove_value.insert(nIndex, other_command->m_remove_value.at(0));
    }

    m_execTime.restart();

    return true;
}


//修改关键帧数据
KeyFrameUpdateCommand::KeyFrameUpdateCommand(Mlt::Filter* filter, int nFrame, QString name, QString from_value, QString to_value, AbstractCommand *parent)
: AbstractCommand(parent)
{
    setText(QObject::tr("KeyFrameUpdateCommand"));
    Q_ASSERT(filter);

    m_bFirstExec    = true;
    m_filter        = new Mlt::Filter(filter->get_filter());
    m_nKeyFrame     = nFrame;
    m_sKeyName      = name;
    m_from_value    = from_value;
    m_to_value      = to_value;

    m_execTime.start();
}

KeyFrameUpdateCommand::~KeyFrameUpdateCommand()
{
    delete m_filter;
}

void KeyFrameUpdateCommand::redo_impl()
{
    if(m_bFirstExec)//第一次自动执行不调用，外部已经执行
    {
        m_bFirstExec = false;
        return;
    }

    Q_ASSERT(MAIN.filterController());
    MAIN.filterController()->updateKeyFrame(m_filter, m_nKeyFrame, m_sKeyName, m_to_value);
}

void KeyFrameUpdateCommand::undo_impl()
{
    Q_ASSERT(MAIN.filterController());
    MAIN.filterController()->updateKeyFrame(m_filter, m_nKeyFrame, m_sKeyName, m_from_value);
}

bool KeyFrameUpdateCommand::mergeWith(const QUndoCommand *other)
{
    Q_ASSERT(other);
    if (!other)                     return false;
    // make sure other is also an AppendText command
    if (other->id() != id())        return false;

    const KeyFrameUpdateCommand *other_command =  static_cast<const KeyFrameUpdateCommand*>(other);
    Q_ASSERT(other_command);
    if(other_command->m_filter->get_filter() != m_filter->get_filter())
    {
            return false;
    }
    if(m_execTime.elapsed() > 800)
    {
        m_execTime.restart();
        return false;
    }

    m_to_value = other_command->m_to_value;
    m_execTime.restart();

    return true;
}



FilterAttachCommand::FilterAttachCommand( QmlMetadata *meta, int rowIndex, int metaIndex, bool bAdd, AbstractCommand * parent)
: AbstractCommand(parent)
{
    setText(QObject::tr("FilterAttachCommand"));
    m_bFirstExec    = true;
    m_meta          = meta;
    m_rowIndex      = rowIndex;
    m_metaIndex     = metaIndex;
    m_bAdd          = bAdd;
}

void FilterAttachCommand::redo_impl()
{
    if(m_bFirstExec)//第一次自动执行不调用，外部已经执行
    {
        m_bFirstExec = false;
        return;
    }

    Q_ASSERT(MAIN.filterController());

    if(m_bAdd)
    {
        MAIN.filterController()->attachedModel()->add(m_meta, true);
        MAIN.filterController()->attachedModel()->move(-1, m_rowIndex, true);
    }
    else
    {
        MAIN.filterController()->attachedModel()->remove(m_rowIndex, true);
    }
}

void FilterAttachCommand::undo_impl()
{
    Q_ASSERT(MAIN.filterController());

    if(m_bAdd)
    {
        MAIN.filterController()->attachedModel()->remove(m_rowIndex, true);
    }
    else
    {
        MAIN.filterController()->attachedModel()->add(m_meta, true);
        MAIN.filterController()->attachedModel()->move(-1, m_rowIndex, true);
    }

}


/*
protected:
 //   Mlt::Filter*    m_filter;

    QmlMetadata     m_meta;
    int             m_rowIndex;
    int             m_metaIndex;
    bool            m_bAdd;

    bool            m_bFirstExec;
*/

FilterMoveCommand::FilterMoveCommand(int rowIndexFrom, int rowIndexTo, AbstractCommand * parent)
: AbstractCommand(parent)
{
    setText(QObject::tr("FilterMoveCommand"));
    m_bFirstExec    = true;

    m_rowIndexFrom  = rowIndexFrom;
    m_rowIndexTo    = rowIndexTo;
}
void FilterMoveCommand::redo_impl()
{
    if(m_bFirstExec)//第一次自动执行不调用，外部已经执行
    {
        m_bFirstExec = false;
        return;
    }
    Q_ASSERT(MAIN.filterController());
    Q_ASSERT(MAIN.filterController()->attachedModel());
    MAIN.filterController()->attachedModel()->move(m_rowIndexFrom, m_rowIndexTo, true);
}

void FilterMoveCommand::undo_impl()
{
    Q_ASSERT(MAIN.filterController());
    Q_ASSERT(MAIN.filterController()->attachedModel());
    MAIN.filterController()->attachedModel()->move(m_rowIndexTo, m_rowIndexFrom, true);
}


ClipsSelectCommand::ClipsSelectCommand(QList<int> newSelection, int newTrackIndex, bool isNewMultitrack,
                                       QList<int> oldSelection, int oldTrackIndex, bool isOldMultitrack,
                                       AbstractCommand * parent)
: AbstractCommand(parent)
{
    m_bFirstExec    = true;
    m_newSelection  = newSelection;
    m_oldSelection  = oldSelection;

    m_newTrackIndex = newTrackIndex;
    m_oldTrackIndex = oldTrackIndex;

    m_bNewIsMultitrack  = isNewMultitrack;
    m_bOldIsMultitrack  = isOldMultitrack;
}

void ClipsSelectCommand::redo_impl()
{
    LOG_DEBUG() << "newTrackIndex: " << m_newTrackIndex << "newSelection" << m_newSelection;
    if(m_bFirstExec)//第一次自动执行不调用，外部已经执行
    {
        m_bFirstExec = false;
        return;
    }

    Q_ASSERT(MAIN.timelineDock());
    MAIN.timelineDock()->setSelection(m_newSelection, m_newTrackIndex, m_bNewIsMultitrack, true);
}

void ClipsSelectCommand::undo_impl()
{
    Q_ASSERT(MAIN.timelineDock());
    MAIN.timelineDock()->setSelection(m_oldSelection, m_oldTrackIndex, m_bOldIsMultitrack, true);
}


/*
protected:
//    void notify();

protected:
    Mlt::Filter*    m_filter;

    int             m_rowIndexFrom;
    int             m_rowIndexTo;

    bool            m_bFirstExec;
};
*/

/*
FilterClipCommand::FilterClipCommand(MultitrackModel& model, int trackIndex, int clipIndex, QString strFromXml, QString strToXml, AbstractCommand * parent)
    : AbstractCommand(parent)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_strFromXml(strFromXml)
    , m_strToXml(strToXml)
    , m_undoHelper(m_model)
{

   setText(QObject::tr("FilterClipCommand"));
}

void FilterClipCommand::redo_impl()
{
    //m_undoHelper.recordBeforeState();
    m_model.refreshClipFromXmlForFilter(m_trackIndex, m_clipIndex, m_strToXml);

    //m_model.moveClip(m_fromTrackIndex, m_toTrackIndex, m_fromClipIndex, m_toStart);
    //m_undoHelper.recordAfterState();
    //qDebug()<<"redo ends";
}

void FilterClipCommand::undo_impl()
{
   // LOG_DEBUG() << "fromTrack" << m_fromTrackIndex << "toTrack" << m_toTrackIndex;
   // m_undoHelper.undoChanges();
     m_model.refreshClipFromXmlForFilter(m_trackIndex, m_clipIndex, m_strFromXml);

}
*/


TransitionPropertyCommand::TransitionPropertyCommand(TimelineDock& timeline, MultitrackModel &model, int trackIndex, int clipIndex, const QString& transitionName, const QString &propertyName, const QString &propertyValue, int invert, double softness, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_timeline(timeline)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_transitionName(transitionName)
    , m_propertyName(propertyName)
    , m_propertyValue(propertyValue)
    , m_undoHelper(m_model)
    , m_invert(invert)
    , m_softness(softness)
    , m_isFirstRedo(true)
{
    setText(QObject::tr("Transition Property Change"));
}

void TransitionPropertyCommand::redo_impl()
{
    m_undoHelper.recordBeforeState();
    m_model.setTransitionProperty(m_trackIndex, m_clipIndex, m_transitionName, m_propertyName, m_propertyValue);
    if (m_propertyName == "resource")
    {
        m_model.setTransitionProperty(m_trackIndex, m_clipIndex, m_transitionName, "progressive", "1");
        m_model.setTransitionProperty(m_trackIndex, m_clipIndex, m_transitionName, "invert", QString("%1").arg(m_invert));
        m_model.setTransitionProperty(m_trackIndex, m_clipIndex, m_transitionName, "softness", QString::number(m_softness));
    }
    m_undoHelper.recordAfterState();
    if (m_isFirstRedo == false)
    {
        //redo时刷新属性界面
        m_timeline.emitSelectedFromSelection();
    }
    m_isFirstRedo = false;
}

void TransitionPropertyCommand::undo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "propertyName" << m_propertyName << "propertyValue" << m_propertyValue;
    m_undoHelper.undoChanges();
    //刷新属性界面
    m_timeline.emitSelectedFromSelection();
}

bool TransitionPropertyCommand::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id())
        return false;

    const TransitionPropertyCommand* that = static_cast<const TransitionPropertyCommand*>(other);

    //需要合并的参数softness、start、Cut类型的resource
    bool isSoftnessProperty = m_propertyName.compare("softness") == 0 && that->m_propertyName.compare("softness") == 0;
    bool isStartProperty = m_propertyName.compare("start") == 0 && that->m_propertyName.compare("start") == 0;
    bool isCutTransition = m_propertyName.compare("resource") == 0 && m_propertyValue.startsWith("color:")
            && that->m_propertyName.compare("resource") == 0 && that->m_propertyValue.startsWith("color:");
    if (isSoftnessProperty || isStartProperty || isCutTransition)
    {
        m_undoHelper.recordAfterState();
        m_propertyValue = that->m_propertyValue;
        return true;
    }

    return false;
}




TransitionDurationSettingCommand::TransitionDurationSettingCommand(TimelineDock &timeline, MultitrackModel &model, int trackIndex, int clipIndex, int duration, AbstractCommand *parent)
    : AbstractCommand(parent)
    , m_timeline(timeline)
    , m_model(model)
    , m_trackIndex(trackIndex)
    , m_clipIndex(clipIndex)
    , m_duration(duration)
    , m_undoHelper(m_model)
    , m_isFirstRedo(true)
{
    setText(QObject::tr("Change Transition Duration"));
}

void TransitionDurationSettingCommand::redo_impl()
{
    LOG_DEBUG() << "trackIndex" << m_trackIndex << "clipIndex" << m_clipIndex << "duration" << m_duration;
    m_undoHelper.recordBeforeState();
    m_model.setTransitionDuration(m_trackIndex, m_clipIndex, m_duration);
    if (m_isFirstRedo == false)
    {
        //redo时刷新属性界面
        m_timeline.emitSelectedFromSelection();
    }
    m_undoHelper.recordAfterState();
    m_isFirstRedo = false;
}

void TransitionDurationSettingCommand::undo_impl()
{
    m_undoHelper.undoChanges();
    //刷新属性界面
    m_timeline.emitSelectedFromSelection();
}

bool TransitionDurationSettingCommand::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id()) // make sure other is also an AppendText command
        return false;

    const TransitionDurationSettingCommand* that = static_cast<const TransitionDurationSettingCommand*>(other);
    m_undoHelper.recordAfterState();
    m_duration = that->m_duration;
    return true;
}




} // namespace

#include "moc_timelinecommands.cpp"
