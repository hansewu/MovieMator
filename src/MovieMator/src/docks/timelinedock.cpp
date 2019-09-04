/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: Dragon-S <15919917852@163.com>
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

#include "timelinedock.h"
#include "ui_timelinedock.h"
#include "models/audiolevelstask.h"
#include "models/multitrackmodel.h"
#include "qmltypes/thumbnailprovider.h"
#include <qmlapplication.h>
#include "mainwindow.h"
#include "commands/timelinecommands.h"
#include <qmlutilities.h>
#include <qmlview.h>
#include "shotcut_mlt_properties.h"
#include "settings.h"
#include "../mainwindow.h"
#include "controllers/filtercontroller.h"
#include <util.h>

#include <QtQml>
#include <QtQuick>
#include <Logger.h>
#include <QtConcurrent/QtConcurrentRun>
#include <QMessageBox>
#include <QAction>
#include <QFileDialog>

#include "../maincontroller.h"

void TimelineDock::filterScrollChild(QObject* parent)
{
    Q_ASSERT(parent);
    if (!parent) {
        return;
    }

    QObjectList children = parent->children();
 //   if(children.size() == 0)
    {
        const QMetaObject *mobj = parent->metaObject();
        Q_ASSERT(mobj);
        if (!mobj) {
            return;
        }
        qDebug()<<"class name is "<<mobj->className()<<"obj name is "<<parent->objectName();

        //if(QString(mobj->className()) == QString("QScrollArea"))
        //            ((QScrollArea *)parent)->viewport()->installEventFilter(this);
        //if(QString(parent->objectName()).contains(QString("qt_scrollarea_viewport")))
          //  ((QWidget *)parent)->installEventFilter(this);
        if(QString(mobj->className()) == QString("QScrollBar"))
                    (qobject_cast<QObject *>(parent))->installEventFilter(this);

    }
 //   else
    {
        Q_ASSERT(!children.isEmpty());
           for(int i=0;i<children.size();++i)
        {
            filterScrollChild(children[i]);
        }
    }
}


TimelineDock::TimelineDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TimelineDock),
    m_quickView(QmlUtilities::sharedEngine(), this),
    m_model(MAINCONTROLLER.multitrackModel()),
    m_position(-1),
    m_updateCommand(nullptr),
    m_ignoreNextPositionChange(false),
    m_filterSettingsView(QmlUtilities::sharedEngine(), nullptr)
{
    LOG_DEBUG() << "begin";

    ui->setupUi(this);
    setMinimumHeight(251);//toolbar+ruler+video+audio+30+margin 39+24+100+50+30+8
    //setContentsMargins(6,6,6,0);

    toggleViewAction()->setIcon(windowIcon());

    qmlRegisterType<MultitrackModel>("MovieMator.Models", 1, 0, "MultitrackModel");

    QDir importPath = QmlUtilities::qmlDir();
    importPath.cd("modules");
    m_quickView.engine()->addImportPath(importPath.path());

    m_quickView.engine()->addImageProvider(QString("thumbnail"), new ThumbnailProvider);
    QmlUtilities::setCommonProperties(m_quickView.rootContext());
//    m_quickView.rootContext()->setContextProperty("mainwindow", &MAIN);
    m_quickView.rootContext()->setContextProperty("view", new QmlView(m_quickView.quickWindow()));
    m_quickView.rootContext()->setContextProperty("timeline", this);
    m_quickView.rootContext()->setContextProperty("multitrack", &m_model);
    m_quickView.setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quickView.setClearColor(palette().window().color());

//    connect(&m_model, SIGNAL(modified()), this, SLOT(clearSelectionIfInvalid()));//sll:将modify放在mainwindow中建立连接，防止界面更新与数据操作顺序问题

    m_quickView.setFocusPolicy(Qt::StrongFocus);
    setWidget(&m_quickView);

    connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onShowFrame(const SharedFrame&)));
#ifdef Q_OS_WIN
    onVisibilityChanged(true);
#else
    connect(this, &QDockWidget::visibilityChanged, this, &TimelineDock::load);
#endif

    connect(&m_model, SIGNAL(selectionChanged(TIMELINE_SELECTION, TIMELINE_SELECTION)), this, SLOT(onSelectionChanged(TIMELINE_SELECTION, TIMELINE_SELECTION)));
    LOG_DEBUG() << "end";


    //filterScrollChild(this);//wzq
    //installEventFilter(this);//wzq
}

TimelineDock::~TimelineDock()
{
    delete m_updateCommand;
    delete ui;
}

void TimelineDock::setPosition(int position)
{
    LOG_DEBUG() << "begin";
    Q_ASSERT(m_model.tractor());
    if (!m_model.tractor()) return;
    if (position < 0 )
        position = 0;
    if (position < m_model.tractor()->get_length()) {
        emit seeked(position);

    } else {
        m_position = m_model.tractor()->get_length();
        emit positionChanged();
    }
    LOG_DEBUG() << "end";

}

QString TimelineDock::timecode(int frames)
{
    Q_ASSERT(MLT.producer());
    Q_ASSERT(MLT.producer()->is_valid());

    if (MLT.producer())
        return MLT.producer()->frames_to_time(frames, mlt_time_smpte);
    return QString("");
}

Mlt::ClipInfo *TimelineDock::getClipInfo(int trackIndex, int clipIndex)
{
    Mlt::ClipInfo* result = nullptr;
    Q_ASSERT(clipIndex >= 0);
    Q_ASSERT(trackIndex >= 0);
    if (clipIndex >= 0 && trackIndex >= 0) {
        Q_ASSERT(trackIndex < m_model.trackList().size());
        int i = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return result;
        }
        Q_ASSERT(i < m_model.tractor()->count());
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        Q_ASSERT(track);
        if (track) {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            if (!playlist.is_valid()) {
                return result;
            }
            Q_ASSERT(clipIndex < playlist.count());
            result = playlist.clip_info(clipIndex);
        }
    }
    return result;
}

Mlt::Producer *TimelineDock::producerForClip(int trackIndex, int clipIndex)
{
    Mlt::Producer* result = nullptr;
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(clipIndex >= 0);
    if (trackIndex < 0 || clipIndex < 0) {
        return result;
    }
    Mlt::ClipInfo* info = getClipInfo(trackIndex, clipIndex);
    Q_ASSERT(info);
    if (info) {
        result = new Mlt::Producer(info->producer);
        delete info;
    }
    return result;
}

int TimelineDock::clipIndexAtPlayhead(int trackIndex)
{
    return clipIndexAtPosition(trackIndex, m_position);
}

int TimelineDock::clipIndexAtPosition(int trackIndex, int position)
{
    int result = -1;
    if (trackIndex < 0)
        trackIndex = currentTrack();

    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    if (trackIndex >= 0 && trackIndex < m_model.trackList().size()) {
        int i = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return result;
        }
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        Q_ASSERT(track);
        if (track) {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            if (!playlist.is_valid()) {
                return result;
            }
            result = playlist.get_clip_index_at(position);
            if (result > playlist.count() - 1)
                result = -1;
        }
    }
    return result;
}

bool TimelineDock::isBlank(int trackIndex, int clipIndex)
{
//    Q_ASSERT(trackIndex >= 0);
//    Q_ASSERT(clipIndex >= 0);
    return m_model.index(clipIndex, 0, m_model.index(trackIndex))
        .data(MultitrackModel::IsBlankRole).toBool();
}

void TimelineDock::pulseLockButtonOnTrack(int trackIndex)
{
    QMetaObject::invokeMethod(m_quickView.rootObject(), "pulseLockButtonOnTrack",
            Qt::DirectConnection, Q_ARG(QVariant, trackIndex));
    emit showStatusMessage(tr("This track is locked"));
}

void TimelineDock::chooseClipAtPosition(int position, int * trackIndex, int * clipIndex)
{
    Q_ASSERT(trackIndex);
    Q_ASSERT(clipIndex);
    if (!trackIndex || !clipIndex) {
        return;
    }

    QScopedPointer<Mlt::Producer> clip;

    // Start by checking for a hit at the specified track
    if (*trackIndex != -1 && !isTrackLocked(*trackIndex)) {
        *clipIndex = clipIndexAtPosition(*trackIndex, position);
        if (*clipIndex != -1 && !isBlank(*trackIndex, *clipIndex))
            return;
    }

    // Next we try the current track
    *trackIndex = currentTrack();
    *clipIndex = clipIndexAtPosition(*trackIndex, position);

    if (!isTrackLocked(*trackIndex) && *clipIndex != -1 && !isBlank(*trackIndex, *clipIndex)) {
        return;
    }

    // if there was no hit, look through the other tracks
    for (*trackIndex = 0; *trackIndex < m_model.trackList().size(); (*trackIndex)++) {
        if (*trackIndex == currentTrack())
            continue;
        if (isTrackLocked(*trackIndex))
            continue;
        *clipIndex = clipIndexAtPosition(*trackIndex, position);
        if (*clipIndex != -1 && !isBlank(*trackIndex, *clipIndex))
            return;
    }
    *trackIndex = -1;
    *clipIndex = -1;
}

int TimelineDock::clipCount(int trackIndex) const
{
    if (trackIndex < 0)
        trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
//    Q_ASSERT(trackIndex < m_model.trackList().size());
    if (trackIndex >= 0 && trackIndex < m_model.trackList().size()) {
        int i = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return 0;
        }
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        Q_ASSERT(track);
        Q_ASSERT(track->is_valid());
        if (track) {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            if (playlist.is_valid()) {
                return playlist.count();
            }
        }
    }
    return 0;
}

void TimelineDock::setCurrentTrack(int currentTrack)
{
//    Q_ASSERT(m_quickView.rootObject());
    if (!m_quickView.rootObject())
        return;
    m_quickView.rootObject()->setProperty("currentTrack", currentTrack);
}

int TimelineDock::currentTrack() const
{
//    Q_ASSERT(m_quickView.rootObject());
    if (!m_quickView.rootObject())
        return 0;
    int nCurrentTrack = m_quickView.rootObject()->property("currentTrack").toInt();
    if(nCurrentTrack >= m_model.trackList().count())
    {
        nCurrentTrack = 0;
        m_quickView.rootObject()->setProperty("currentTrack", nCurrentTrack);
    }
    return nCurrentTrack;//m_quickView.rootObject()->property("currentTrack").toInt();
}

void TimelineDock::zoomIn()
{
    QMetaObject::invokeMethod(m_quickView.rootObject(), "zoomIn");
}

void TimelineDock::zoomOut()
{
    QMetaObject::invokeMethod(m_quickView.rootObject(), "zoomOut");
}

void TimelineDock::resetZoom()
{
    QMetaObject::invokeMethod(m_quickView.rootObject(), "resetZoom");
}


//wzq
void TimelineDock::wheelEvent(QWheelEvent * event)
{
    Q_UNUSED(event)
/*    if(event->delta() > 0)
    {
        if (isVisible()) {
            if (event->modifiers() & Qt::ControlModifier)
                ;//m_timelineDock->makeTracksShorter();
            else
                zoomOut();
        }

    }
    else
    {
        if (isVisible()) {
            if (event->modifiers() & Qt::ControlModifier)
                ;//m_timelineDock->makeTracksShorter();
            else
                zoomIn();
        }
    }

   // event->ignore();
   event->accept();
   */
}

bool TimelineDock::eventFilter( QObject * o, QEvent * e ) {
    Q_ASSERT(o || e);
    if (!o || !e) {
        return false;
    }

   if (e->type() == QEvent::Wheel)
   {
       const QMetaObject *mobj = o->metaObject();
       Q_ASSERT(mobj);
       if (!mobj) {
           return false;
       }
       qDebug()<<"class name is "<<mobj->className()<<"obj name is "<<o->objectName();

       //&& qobject_cast<ProjectTreeView*>( o ) ) {
        // Then do what you want, per example: ignore it.
        //e->ignore();
        return true;
   }
   return QWidget::eventFilter( o, e );
}


void TimelineDock::makeTracksShorter()
{
    QMetaObject::invokeMethod(m_quickView.rootObject(), "makeTracksShorter");
}

void TimelineDock::makeTracksTaller()
{
    QMetaObject::invokeMethod(m_quickView.rootObject(), "makeTracksTaller");
}

void TimelineDock::setSelection(QList<int> newSelection, int trackIndex, bool isMultitrack)
{
    int nClipIndexToSelect;
    if (newSelection.count() > 0)
        nClipIndexToSelect = newSelection.at(0);
    else
        nClipIndexToSelect = -1;

    if (nClipIndexToSelect != m_model.selection().nIndexOfSelectedClip
            || trackIndex != m_model.selection().nIndexOfSelectedTrack
            || isMultitrack != m_model.selection().bIsMultitrackSelected)
    {
        LOG_DEBUG() << "Changing selection to" << newSelection << " trackIndex" << trackIndex << "isMultitrack" << isMultitrack;
        TIMELINE_SELECTION aSelection;
        if (newSelection.count() > 0)
            aSelection.nIndexOfSelectedClip = newSelection[0];
        else
            aSelection.nIndexOfSelectedClip = -1;
        aSelection.nIndexOfSelectedTrack = trackIndex;
        aSelection.bIsMultitrackSelected = isMultitrack;
        m_model.setSelection(aSelection);

        MLT.pause();

        setCurrentFilter(nullptr, nullptr,0);

        //emit selectionChanged();

//        if (m_model.selection().nIndexOfSelectedClip >= 0)
//            emitSelectedFromSelection();
//        else
//            emit selected(nullptr);
    }

}

QList<int> TimelineDock::selection() const
{
    if (!m_quickView.rootObject())
        return QList<int>();
    QList<int> listSelection;
    if (m_model.selection().nIndexOfSelectedClip >= 0)
        listSelection.append(m_model.selection().nIndexOfSelectedClip);
    return listSelection;
}

void TimelineDock::selectClipUnderPlayhead()
{
    int track = -1, clip = -1;
    chooseClipAtPosition(m_position, &track, &clip);
    if (clip == -1) {
        if (isTrackLocked(currentTrack())) {
            pulseLockButtonOnTrack(currentTrack());
            return;
        }
        int idx = clipIndexAtPlayhead(-1);
        if (idx == -1)
            setSelection(QList<int>(), track);
        else
            setSelection(QList<int>() << idx, track);
        return;
    }

    setCurrentTrack(track);
    setSelection(QList<int>() << clip, track);
}

void TimelineDock::selectClipAtPosition(int trackIndex, int position)
{
    setCurrentTrack(trackIndex);
    int clipIndex = clipIndexAtPosition(trackIndex, position);
    setSelection(QList<int>() << clipIndex, trackIndex);
}


int TimelineDock::centerOfClip(int trackIndex, int clipIndex)
{
    Q_UNUSED(trackIndex)
    Q_UNUSED(clipIndex)
    return 0;
//    Mlt::ClipInfo * clip = getClipInfo(trackIndex, clipIndex);
//    Q_ASSERT(clip);
//    int centerOfClip = clip->start + clip->frame_count / 2;
//    delete clip;
//    clip = 0;
//    return centerOfClip;
}

bool TimelineDock::isTrackLocked(int trackIndex) const
{
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    if (trackIndex < 0 || trackIndex >= m_model.trackList().size())
        return false;
    int i = m_model.trackList().at(trackIndex).mlt_index;
    Q_ASSERT(m_model.tractor());
    if (!m_model.tractor()) {
        return false;
    }
    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
    Q_ASSERT(track);
    if (!track) {
        return false;
    }
    return track->get_int(kTrackLockProperty);
}

void TimelineDock::trimClipAtPlayhead(TrimLocation location, bool ripple)
{
    int trackIndex = currentTrack(), clipIndex = -1;
    chooseClipAtPosition(m_position, &trackIndex, &clipIndex);
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(clipIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    if (trackIndex < 0 || clipIndex < 0)
        return;
    setCurrentTrack(trackIndex);

    int i = m_model.trackList().at(trackIndex).mlt_index;
    Q_ASSERT(m_model.tractor());
    if (!m_model.tractor()) {
        return;
    }
    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
    Q_ASSERT(track);
    if (!track) {
        return;
    }

    QScopedPointer<Mlt::ClipInfo> info(getClipInfo(trackIndex, clipIndex));
    Q_ASSERT(info);
    if (!info) {
        return;
    }

    if (location == TrimInPoint) {
        MAIN.pushCommand(
            new Timeline::TrimClipInCommand(m_model, trackIndex, clipIndex, m_position - info->start, ripple));
        if (ripple)
            setPosition(info->start);
    } else {
        MAIN.pushCommand(
            new Timeline::TrimClipOutCommand(m_model, trackIndex, clipIndex, info->start + info->frame_count - m_position, ripple));
    }
}

bool TimelineDock::isRipple() const
{
    Q_ASSERT(m_quickView.rootObject());
    return m_quickView.rootObject()->property("ripple").toBool();
}

void TimelineDock::clearSelectionIfInvalid()
{
//    int count = clipCount(currentTrack());//应该使用和selection()中clip对应的track做处理，如果不对应则没有处理的必要，因为clip如果不在track上一个得到的数据是不对的
    int count = clipCount(m_model.selection().nIndexOfSelectedTrack);

    QList<int> newSelection;
    foreach (int index, selection()) {
        if (index >= count)
            continue;
        if (isBlank(m_model.selection().nIndexOfSelectedTrack, index))
            continue;

        newSelection << index;
    }
    //commit: 5834deadb70be3b2841ae7c4ede50b3724a1b34a
    //fix bug:
    //（1）The filter information of the new clip is error when a new clip is inserted at the beginning of the selected clip.
    //setSelection();

    setSelection(newSelection, m_model.selection().nIndexOfSelectedTrack);
    //emit selectionChanged();
}

void TimelineDock::insertTrack()
{
//    MAIN.pushCommand(
//                new Timeline::InsertTrackCommand(m_model, currentTrack()));
    MAINCONTROLLER.insertTrack(currentTrack());
}

void TimelineDock::removeTrack()
{

    int trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    const Track& track = m_model.trackList().value(trackIndex);

    //trackNumber
    //0 - default video and audio track
    int trackNumber = track.number;


    if (m_model.trackList().size() > 0 && trackNumber > 0)
//        MAIN.pushCommand(
//                new Timeline::RemoveTrackCommand(m_model, currentTrack()));
        MAINCONTROLLER.removeTrack(trackIndex);

    if(trackIndex > 0)
        setCurrentTrack(trackIndex-1);
}

void TimelineDock::onProducerChanged(Mlt::Producer* after)
{
    Q_ASSERT(after);
    Q_ASSERT(after->is_valid());
    if (!after) {
        return;
    }

    int trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(!selection().isEmpty());
    Q_ASSERT(m_updateCommand);
    if (trackIndex < 0 || selection().isEmpty() || !m_updateCommand || !after || !after->is_valid())
        return;

    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }

    QString service = after->get("mlt_service");
    if (service == "avformat" || service == "avformat-novalidate" || service == "timewarp") {
        Q_ASSERT(trackIndex < m_model.trackList().size());
        int i = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return;
        }
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        Q_ASSERT(track);
        if (track) {
            // Ensure the new XML has same in/out point as selected clip by making
            // a copy of the changed producer and copying the in/out from timeline.
            Mlt::Playlist playlist(*track);
            int clipIndex = selection().first();
            Q_ASSERT(playlist.is_valid());
            QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));
            Q_ASSERT(info);
            if (info) {
                Q_ASSERT(info->producer);
                if (!info->producer) {
                    return;
                }

                double oldSpeed = qstrcmp("timewarp", info->producer->get("mlt_service")) ? 1.0 : info->producer->get_double("warp_speed");
                double newSpeed = qstrcmp("timewarp", after->get("mlt_service")) ? 1.0 : after->get_double("warp_speed");
                double speedRatio = oldSpeed / newSpeed;

                //计算Producer的入点出点
                int length = qRound(info->length * speedRatio);
                int in = qRound(info->frame_in * speedRatio);
                int out = qRound(info->frame_out * speedRatio);

                //有转场的情况下，重新调节Producer的入点出点
                bool hasPreviousTransition = m_model.isTransition(playlist, clipIndex - 1);
                bool hasNextTransition = m_model.isTransition(playlist, clipIndex + 1);
                if (hasPreviousTransition || hasNextTransition) {
                    QScopedPointer<Mlt::ClipInfo> previousTransitionClipInfo(nullptr);
                    QScopedPointer<Mlt::ClipInfo> nextTransitionClipInfo(nullptr);
                    if (hasPreviousTransition) {
                        previousTransitionClipInfo.reset(playlist.clip_info(clipIndex - 1));
                    }
                    if (hasNextTransition) {
                        nextTransitionClipInfo.reset(playlist.clip_info(clipIndex + 1));
                    }

                    int previousTransitionDuration = 0;
                    int nextTransitionDuration = 0;
                    if (hasPreviousTransition && hasNextTransition) {//clip前后都有转场
                        previousTransitionDuration = previousTransitionClipInfo->frame_count;
                        nextTransitionDuration = nextTransitionClipInfo->frame_count;
                    } else if (hasPreviousTransition) {//clip前面有转场，后面没有
                        previousTransitionDuration = previousTransitionClipInfo->frame_count;
                    } else if (hasNextTransition) {//clip后面有转场，前面没有
                        nextTransitionDuration = nextTransitionClipInfo->frame_count;
                    }

                    in = qRound((info->frame_in - previousTransitionDuration) * speedRatio);
                    out = qRound((info->frame_out + nextTransitionDuration) * speedRatio);

                    m_updateCommand->setSpeedChanged(true);
                }

                //设置Producer入点出点
                after->set("length", length);
                after->set_in_and_out(qMin(in, length - 1), qMin(out, length - 1));

                //调节滤镜的入点出点
                int filterCount = after->filter_count();
                for (int index = 0; index < filterCount; index++) {
                    QScopedPointer<Mlt::Filter> filter(after->filter(index));
                    if (filter && filter->is_valid() && !filter->get_int("_loader")) {
                        int in = qMin(qRound(filter->get_in() * speedRatio), length - 1);
                        int out = qMin(qRound(filter->get_out() * speedRatio), length - 1);
                        filter->set_in_and_out(in, out);
                    }
                }
            }
        }
    }
    QList <int> originSelection = selection();
    QString xmlAfter = MLT.XML(after);
    m_updateCommand->setXmlAfter(xmlAfter);
    setSelection(QList<int>(), trackIndex); // clearing selection prevents a crash
    Timeline::UpdateClipCommand* command = m_updateCommand;
    Q_ASSERT(command);
    if (!command) {
        return;
    }
    m_updateCommand = nullptr;
    MAIN.pushCommand(command);

    //setSelection(originSelection);
}

void TimelineDock::addAudioTrack()
{
    MAIN.pushCommand(
        new Timeline::AddTrackCommand(m_model, AudioTrackType));
}

void TimelineDock::addVideoTrack()
{
    MAIN.pushCommand(
        new Timeline::AddTrackCommand(m_model, VideoTrackType));
}

void TimelineDock::addFilterTrack()
{
    MAIN.pushCommand(
        new Timeline::AddTrackCommand(m_model, FilterTrackType));
}

void TimelineDock::addTextTrack()
{
    MAIN.pushCommand(
        new Timeline::AddTrackCommand(m_model, TextTrackType));
}

void TimelineDock::onShowFrame(const SharedFrame& frame)
{
    if (m_ignoreNextPositionChange) {
        m_ignoreNextPositionChange = false;
    } else if (static_cast<void*>(MLT.producer()->get_producer()) == static_cast<void*>(MAIN.multitrack()->get_producer())) {
     //if ( MLT.isMultitrack() && (MLT.producer()->get_speed() != 0) ) {//加此判断解决点击clip有时指针不跳的问题
        if (frame.get_position() < MLT.producer()->get_playtime()) {
            m_position = frame.get_position();
            emit positionChanged();
        }
     //}
    }
}

void TimelineDock::onSeeked(int position)
{
    Q_ASSERT(position >= 0);
    if (MLT.isMultitrack()) {
        m_position = position;
        emit positionChanged();
    }
}

void TimelineDock::append(int trackIndex)
{
    if (trackIndex < 0)
        trackIndex = currentTrack();
    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }
    if (MLT.isSeekableClip() || MLT.savedProducer()) {
        Timeline::AppendClipCommand *appendCommand = new Timeline::AppendClipCommand(m_model, trackIndex,
                                                                                     MLT.XML(MLT.isClip()? nullptr : MLT.savedProducer()));
        MAIN.pushCommand(appendCommand);
    }
}

void TimelineDock::remove(int trackIndex, int clipIndex)
{
    LOG_DEBUG() << "trackIndex" << trackIndex << "clipIndex" << clipIndex;
    if (!m_model.trackList().count())
        return;
    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(clipIndex >= 0);
    Mlt::Producer* clip = producerForClip(trackIndex, clipIndex);
    Q_ASSERT(clip);
    if (clip) {
        QString xml = MLT.XML(clip);
        delete clip;


        MAIN.undoStack()->beginMacro("Remove from track");
        int newClipIndex = removeTransitionOnClipWithUndo(trackIndex, clipIndex);
        MAIN.pushCommand(
            new Timeline::RemoveClipCommand(m_model, *this, trackIndex, newClipIndex, xml));
        MAIN.undoStack()->endMacro();
    }
}

void TimelineDock::lift(int trackIndex, int clipIndex)
{
    LOG_DEBUG() << "trackIndex" << trackIndex << "clipIndex" << clipIndex;
    if (!m_model.trackList().count())
        return;
    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }
    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);

    QScopedPointer<Mlt::Producer> clip(producerForClip(trackIndex, clipIndex));
    Q_ASSERT(clip);
    if (clip) {
        if (clip->is_blank())
            return;

        bool leftBlank = false;
        if (clipIndex > 0 && isBlank(trackIndex, clipIndex - 1))
            leftBlank = true;
        Q_UNUSED(leftBlank);
        bool rightBlank = false;
        if (clipIndex <  clipCount(trackIndex) - 1 && isBlank(trackIndex, clipIndex + 1))
            rightBlank = true;
        Q_UNUSED(rightBlank);

        MAIN.undoStack()->beginMacro(tr("Lift from track"));

        int newClipIndex = removeTransitionOnClipWithUndo(trackIndex, clipIndex);
        QString xml = MLT.XML(clip.data());
        MAIN.pushCommand(
            new Timeline::LiftClipCommand(m_model, *this, trackIndex, newClipIndex, xml));

        MAIN.undoStack()->endMacro();

        setSelection(QList<int>(), trackIndex);
    }
}

void TimelineDock::removeSelection(bool withCopy)
{
    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }
    if (selection().isEmpty())
        selectClipUnderPlayhead();
    if (selection().isEmpty() || currentTrack() < 0)
        return;

    if (withCopy)
        copyClip(currentTrack(), selection().first());
    foreach (int index, selection())
        remove(currentTrack(), index);
}

void TimelineDock::liftSelection()
{
    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }
    if (selection().isEmpty())
        selectClipUnderPlayhead();
    if (selection().isEmpty())
        return;
    foreach (int index, selection())
        lift(currentTrack(), index);
}

void TimelineDock::selectTrack(int by)
{
    int newTrack = currentTrack();
    if (by < 0)
        newTrack = qMax(0, newTrack + by);
    else
        newTrack = qMin(m_model.trackList().size() - 1, newTrack + by);
    setCurrentTrack(newTrack);
}

void TimelineDock::selectTrackHead(int trackIndex)
{
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    if (trackIndex >= 0) {
        setSelection(QList<int>(), trackIndex);
        int i = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return;
        }
        Mlt::Producer* producer = m_model.tractor()->track(i);
        Q_ASSERT(producer);
        Q_ASSERT(producer->is_valid());
        if (producer && producer->is_valid())
            emit selected(producer);
        delete producer;
    }
}

void TimelineDock::selectMultitrack()
{
    setSelection(QList<int>(), -1, true);
    QMetaObject::invokeMethod(m_quickView.rootObject(), "selectMultitrack");
    emit selected(m_model.tractor());
}

void TimelineDock::copyClip(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);
    QScopedPointer<Mlt::ClipInfo> info(getClipInfo(trackIndex, clipIndex));
    Q_ASSERT(info);
    if (info) {
        MAIN.setPauseAfterOpen(false);
        QString xml = MLT.XML(info->producer);
        Mlt::Producer p(MLT.profile(), "xml-string", xml.toUtf8().constData());
        Q_ASSERT(p.is_valid());
        p.set_speed(0);
        p.seek(info->frame_in);
        p.set_in_and_out(info->frame_in, info->frame_out);
//        MLT.setSavedProducer(&p);
        m_model.setCopiedProducer(&p);
        emit clipCopied();
    }
}

void TimelineDock::paste()
{
    if(m_model.copiedProducer() != nullptr)
    {
        QString strXml = MLT.XML(m_model.copiedProducer());
        insert(-1, m_position, strXml);
    }
}

void TimelineDock::emitSelectedFromSelection()
{
    if (!m_model.trackList().count()) {
        if (m_model.tractor())
            selectMultitrack();
        else
            emit selected(nullptr);
        return;
    }

    int trackIndex = m_model.selection().nIndexOfSelectedTrack;
    int clipIndex = m_model.selection().nIndexOfSelectedClip;
    if (trackIndex < 0 || clipIndex < 0) {
        emit selected(nullptr);
        return;
    }

    Mlt::ClipInfo* info = getClipInfo(trackIndex, clipIndex);
    Q_ASSERT(info);
    Q_ASSERT(info->producer);
    Q_ASSERT(info->producer->is_valid());
    if (info && info->producer && info->producer->is_valid()) {
        delete m_updateCommand;
        m_updateCommand = new Timeline::UpdateClipCommand(*this, m_model, trackIndex, clipIndex, info->start);
        // We need to set these special properties so time-based filters
        // can get information about the cut while still applying filters
        // to the cut parent.
        info->producer->set(kFilterInProperty, info->frame_in);
        info->producer->set(kFilterOutProperty, info->frame_out);
        if (MLT.isImageProducer(info->producer))
            info->producer->set("out", info->cut->get_int("out"));
        info->producer->set(kMultitrackItemProperty, 1);
        m_ignoreNextPositionChange = true;

        info->producer->set_in_and_out(info->frame_in, info->frame_out);

        emit selected(info->producer);

        if(selection().isEmpty()
           || (model()->selectedProducer() == nullptr)
           || ((static_cast<void*>(model()->selectedProducer()->get_producer())
                != static_cast<void*>(info->producer->get_producer()))))
        {
            // 同一个 producer就不刷新滤镜列表
            model()->setSelectedProducer(info->producer);
        }

        info->producer->set_in_and_out(0, -1);

        delete info;
    }
}

void TimelineDock::remakeAudioLevels(int trackIndex, int clipIndex)
{
    QModelIndex modelIndex = m_model.index(clipIndex, 0, m_model.index(trackIndex));
    QScopedPointer<Mlt::ClipInfo> info(getClipInfo(trackIndex, clipIndex));
    Q_ASSERT(info);
    if (!info) {
        return;
    }
    AudioLevelsTask::start(*info->producer, &m_model, modelIndex, /* force */ true);
}

void TimelineDock::setTrackName(int trackIndex, const QString &value)
{
    MAIN.pushCommand(
        new Timeline::NameTrackCommand(m_model, trackIndex, value));
}

void TimelineDock::toggleTrackMute(int trackIndex)
{
    MAIN.pushCommand(
        new Timeline::MuteTrackCommand(m_model, trackIndex));
}

void TimelineDock::toggleTrackHidden(int trackIndex)
{
    MAIN.pushCommand(
        new Timeline::HideTrackCommand(m_model, trackIndex));
}

void TimelineDock::setTrackComposite(int trackIndex, Qt::CheckState composite)
{
    MAIN.pushCommand(
        new Timeline::CompositeTrackCommand(m_model, trackIndex, composite));
}

void TimelineDock::setTrackLock(int trackIndex, bool lock)
{
    MAIN.pushCommand(
        new Timeline::LockTrackCommand(m_model, trackIndex, lock));
}

bool TimelineDock::moveClip(int fromTrack, int toTrack, int clipIndex, int position)
{
    int newPosition;
    if (m_model.moveClipValid(fromTrack, toTrack, clipIndex, position)) {
        Timeline::MoveClipCommand *moveCommand = new Timeline::MoveClipCommand(m_model, fromTrack, toTrack, clipIndex, position);
        MAIN.pushCommand(moveCommand);
        return true;
    } else if (m_model.addTransitionValid(fromTrack, toTrack, clipIndex, position)) {

        MAIN.pushCommand(
            new Timeline::AddTransitionCommand(m_model, fromTrack, clipIndex, position));
        //emitSelectedFromSelection();
        return true;
    } else if ( ( newPosition = m_model.moveInsertClipValid(fromTrack, toTrack, clipIndex, position) )  >= 0) {
        Timeline::MoveInsertClipCommand *moveInsertCommand = new Timeline::MoveInsertClipCommand(m_model, fromTrack, toTrack, clipIndex, position);
        MAIN.pushCommand(moveInsertCommand);
        return true;
    } else {
        return false;
    }
}

void TimelineDock::addFilterClipCommand(int TrackIndex, int clipIndex, QString strFromXml, QString strToXml)
{
    Q_UNUSED(TrackIndex)
    Q_UNUSED(clipIndex)
    Q_UNUSED(strFromXml)
    Q_UNUSED(strToXml)
 //   MAIN.pushCommand(new Timeline::FilterClipCommand(m_model, TrackIndex, clipIndex, strFromXml, strToXml));
}


bool TimelineDock::trimClipIn(int trackIndex, int clipIndex, int delta, bool ripple)
{
    if (!ripple && m_model.addTransitionByTrimInValid(trackIndex, clipIndex, delta)) {
        MAIN.pushCommand(
            new Timeline::AddTransitionByTrimInCommand(m_model, trackIndex, clipIndex, delta));
    }
    else if (m_model.trimTransitionOutValid(trackIndex, clipIndex, delta)) {
        MAIN.pushCommand(
            new Timeline::TrimTransitionOutCommand(m_model, trackIndex, clipIndex, delta));
    }
    else if (m_model.trimClipInValid(trackIndex, clipIndex, delta, ripple)) {
        MAIN.pushCommand(
            new Timeline::TrimClipInCommand(m_model, trackIndex, clipIndex, delta, ripple));
    }
    else return false;
    return true;
}

bool TimelineDock::trimClipOut(int trackIndex, int clipIndex, int delta, bool ripple)
{
    if (!ripple && m_model.addTransitionByTrimOutValid(trackIndex, clipIndex, delta)) {
        MAIN.pushCommand(
            new Timeline::AddTransitionByTrimOutCommand(m_model, trackIndex, clipIndex, delta));
    }
    else if (m_model.trimTransitionInValid(trackIndex, clipIndex, delta)) {
        MAIN.pushCommand(
            new Timeline::TrimTransitionInCommand(m_model, trackIndex, clipIndex, delta));
    }
    else if (m_model.trimClipOutValid(trackIndex, clipIndex, delta, ripple)) {
        MAIN.pushCommand(
                new Timeline::TrimClipOutCommand(m_model, trackIndex, clipIndex, delta, ripple));
    }
    else return false;
    return true;
}

void TimelineDock::insert(int trackIndex, int position, const QString &xml)
{
    if (trackIndex < 0)
        trackIndex = currentTrack();
    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }
    if (MLT.isSeekableClip() || MLT.savedProducer() || !xml.isEmpty()) {
        QString xmlToUse = !xml.isEmpty()? xml
            : MLT.XML(MLT.isClip()? nullptr : MLT.savedProducer());
        if (position < 0)
            position = m_position;

        Timeline::InsertClipCommand *insertCommand = new Timeline::InsertClipCommand(m_model, trackIndex, position, xmlToUse);
        MAIN.pushCommand(insertCommand);
    }
}

void TimelineDock::overwrite(int trackIndex, int position, const QString &xml)
{
    if (trackIndex < 0)
        trackIndex = currentTrack();
    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }
    if (MLT.isSeekableClip() || MLT.savedProducer() || !xml.isEmpty()) {
        QString xmlToUse = !xml.isEmpty()? xml
            : MLT.XML(MLT.isClip()? nullptr : MLT.savedProducer());
        if (position < 0)
            position = m_position;
        Timeline::OverwriteClipCommand *overwriteCommand = new Timeline::OverwriteClipCommand(m_model, trackIndex, position, xmlToUse);
        MAIN.pushCommand(overwriteCommand);
    }
}

void TimelineDock::appendFromPlaylist(Mlt::Playlist *playlist)
{
    int trackIndex = currentTrack();
    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }
    m_model.appendFromPlaylist(playlist, trackIndex);
    selectClipUnderPlayhead();
}

void TimelineDock::splitClip(int trackIndex, int clipIndex)
{
    if (trackIndex < 0 || clipIndex < 0)
        chooseClipAtPosition(m_position, &trackIndex, &clipIndex);
    if (trackIndex < 0 || clipIndex < 0)
        return;

    setCurrentTrack(trackIndex);

    if (isTrackLocked(trackIndex))
    {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }

    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    if (clipIndex >= 0 && trackIndex >= 0) {
        int i = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return;
        }
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        Q_ASSERT(track);
        if (track) {
            Mlt::Playlist playlist(*track);
            if (!m_model.isTransition(playlist, clipIndex)) {
//                QScopedPointer<Mlt::ClipInfo> info(getClipInfo(trackIndex, clipIndex));
//                if (info && m_position >= info->start + 2 && m_position < info->start + info->frame_count - 1 - 2) {
//                    MAIN.pushCommand(
//                        new Timeline::SplitCommand(m_model, trackIndex, clipIndex, m_position));
//                }
                MAINCONTROLLER.splitClip(trackIndex, clipIndex, m_position);
            } else {
                emit showStatusMessage(tr("You cannot split a transition."));
            }
        }
    }
}


void TimelineDock::AttachedfilterChanged()
{
    int trackIndex = currentTrack();;
    int clipIndex = -1;
    if (m_model.selection().nIndexOfSelectedClip >= 0)
           clipIndex = m_model.selection().nIndexOfSelectedClip;

    if (trackIndex < 0) return;
    if (clipIndex < 0) return;

    m_model.AttachedfilterChanged(trackIndex, clipIndex);
}

void TimelineDock::fadeIn(int trackIndex, int clipIndex, int duration)
{
    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }
    if (duration < 0) return;
    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);
    MAIN.pushCommand(
        new Timeline::FadeInCommand(m_model, trackIndex, clipIndex, duration));
    emit fadeInChanged(duration);
}

void TimelineDock::fadeOut(int trackIndex, int clipIndex, int duration)
{
    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }
    if (duration < 0) return;
    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);
    MAIN.pushCommand(
        new Timeline::FadeOutCommand(m_model, trackIndex, clipIndex, duration));
    emit fadeOutChanged(duration);
}

void TimelineDock::seekPreviousEdit()
{
    if (!MLT.isMultitrack()) return;
    Q_ASSERT(m_model.tractor());
    if (!m_model.tractor()) return;

    int newPosition = -1;
    int n = m_model.tractor()->count();
    for (int i = 0; i < n; i++) {
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        Q_ASSERT(track);
        if (track) {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            int clipIndex = playlist.get_clip_index_at(m_position);
            if (clipIndex >= 0 && m_position == playlist.clip_start(clipIndex))
                --clipIndex;
            if (clipIndex >= 0)
                newPosition = qMax(newPosition, playlist.clip_start(clipIndex));
        }
    }
    if (newPosition != m_position)
        setPosition(newPosition);
}

void TimelineDock::seekNextEdit()
{
    if (!MLT.isMultitrack()) return;
    Q_ASSERT(m_model.tractor());
    if (!m_model.tractor()) return;

    int newPosition = std::numeric_limits<int>::max();
    int n = m_model.tractor()->count();
    for (int i = 0; i < n; i++) {
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        Q_ASSERT(track);
        if (track) {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            int clipIndex = playlist.get_clip_index_at(m_position) + 1;
            if (clipIndex < playlist.count())
                newPosition = qMin(newPosition, playlist.clip_start(clipIndex));
            else if (clipIndex == playlist.count())
                newPosition = qMin(newPosition, playlist.clip_start(clipIndex) + playlist.clip_length(clipIndex));
        }
    }
    if (newPosition != m_position)
        setPosition(newPosition);
}

void TimelineDock::seekInPoint(int clipIndex)
{
    if (!MLT.isMultitrack()) return;
    Q_ASSERT(m_model.tractor());
    if (!m_model.tractor()) return;
    if (clipIndex < 0) return;

    Q_ASSERT(currentTrack() >= 0);
    Q_ASSERT(currentTrack() < m_model.trackList().size());
    int mltTrackIndex = m_model.trackList().at(currentTrack()).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mltTrackIndex));
    Q_ASSERT(track);
    if (track) {
        Mlt::Playlist playlist(*track);
        Q_ASSERT(playlist.is_valid());
        if (m_position != playlist.clip_start(clipIndex))
            setPosition(playlist.clip_start(clipIndex));
    }
}

void TimelineDock::dragEnterEvent(QDragEnterEvent *event)
{
    Q_ASSERT(event);
    Q_ASSERT(event->mimeData());
    if (event->mimeData()->hasFormat(MLT.MltXMLMimeType())) {
        event->acceptProposedAction();
    }
}

void TimelineDock::dragMoveEvent(QDragMoveEvent *event)
{
    Q_ASSERT(event);
    Q_ASSERT(event->mimeData());
    emit dragging(event->posF(), event->mimeData()->text().toInt());
}

void TimelineDock::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
    emit dropped();
}

void TimelineDock::dropEvent(QDropEvent *event)
{
    Q_ASSERT(event);
    Q_ASSERT(event->mimeData());
    if (event->mimeData()->hasFormat(MLT.MltXMLMimeType())) {
        int trackIndex = currentTrack();
        if (trackIndex >= 0) {
            emit dropAccepted(QString::fromUtf8(event->mimeData()->data(MLT.MltXMLMimeType())));
            event->acceptProposedAction();
        }
    }
    emit dropped();
}

bool TimelineDock::event(QEvent *event)
{
    Q_ASSERT(event);
    bool result = QDockWidget::event(event);
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange)
        load(true);
    if (event->type() == QEvent::Wheel)
    {
        //&& qobject_cast<ProjectTreeView*>( o ) ) {
         // Then do what you want, per example: ignore it.
         event->ignore();
         return true;
    }

    return result;
}

void TimelineDock::load(bool force)
{
    if (m_quickView.source().isEmpty() || force) {
        QDir sourcePath = QmlUtilities::qmlDir();
        sourcePath.cd("timeline");
        m_quickView.setSource(QUrl::fromLocalFile(sourcePath.filePath("timeline.qml")));
        disconnect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(onVisibilityChanged(bool)));
        connect(m_quickView.rootObject(), SIGNAL(currentTrackChanged()),
                this, SIGNAL(currentTrackChanged()));
        connect(m_quickView.rootObject(), SIGNAL(clipClicked()),
                this, SIGNAL(clipClicked()));
        if (force && Settings.timelineShowWaveforms())

            m_model.reload();
    } else if (Settings.timelineShowWaveforms()) {
        m_model.reload();
    }
}

void TimelineDock::onVisibilityChanged(bool visible)
{
    if (visible)
        load();
}
//add filter to timeline
//TODO: use undo/redo implement
void TimelineDock::addFilterToTimeline(Mlt::Producer *producer)
{
    Q_ASSERT(producer);
    if (!producer) {
        return;
    }
    m_model.addFilter(currentTrack(), producer);
}

//add text to timeline
//TODO: use undo/redo implement
void TimelineDock::addTextToTimeline(Mlt::Producer *producer)
{
    Q_ASSERT(producer);
    if (!producer) {
        return;
    }
    m_model.addText(currentTrack(), producer);
}
void TimelineDock::onShowProperties(int trackIndex, int clipIndex)
{
    if (!m_model.trackList().count())
        return;

    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);

    Q_ASSERT(trackIndex < m_model.trackList().size());
    int i = m_model.trackList().at(trackIndex).mlt_index;
    Q_ASSERT(m_model.tractor());
    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
    Q_ASSERT(track);
    if (track) {
        Mlt::Playlist playlist(*track);
        Q_ASSERT(playlist.is_valid());
        Mlt::ClipInfo* i = playlist.clip_info(clipIndex);
        Q_ASSERT(i);
        if (i) {
//            QString xml = MLT.XML(i->producer);
//            Mlt::Producer* p = new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData());
//            p->set_in_and_out(i->frame_in, i->frame_out);
//            delete i;

//            Mlt::Producer *producer1 = playlist.get_clip(clipIndex);
//            Mlt::Producer *producer2 = i->producer;
//            Mlt::Producer *producer3 = i->cut;

//            i->producer->set_in_and_out(i->frame_in, i->frame_out);

//            MAIN.loadProducerWidget(i->producer);
//            MAIN.onPropertiesDockTriggered();
            MAIN.showPropertiesDock();
        }

    }
//    Mlt::Producer* clip = producerForClip(trackIndex, clipIndex);

}




//add Position&Size to current clip
void TimelineDock::addPositionAndSizeFilter()
{
    if (selection().count() <=0)
        return;

    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }


    //判断选中的是视频clip
    int trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    const Track& track = m_model.trackList().value(trackIndex);
    if (track.type != VideoTrackType)
        return;


    emit addPositionAndSize();
    emit filterAdded();
}

//add Rotate Filter to current clip
void TimelineDock::addRotateFilter()
{
    if (selection().count() <=0)
        return;

    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }

    //判断选中的是视频clip
    int trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    const Track& track = m_model.trackList().value(trackIndex);
    if (track.type != VideoTrackType)
        return;


    emit addRotate();
    emit filterAdded();
}

//add Crop Filter to current clip
void TimelineDock::addCropFilter()
{
    if (selection().count() <=0)
        return;

    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }


    //判断选中的是视频clip
    int trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    const Track& track = m_model.trackList().value(trackIndex);
    if (track.type != VideoTrackType)
        return;


    emit addCrop();
    emit filterAdded();

}

void TimelineDock::addFadeInAudioFilter()//
{

    if (selection().count() <=0)
        return;

    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }


    //判断选中的是视频clip
    int trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    const Track& track = m_model.trackList().value(trackIndex);
    if (track.type != VideoTrackType)
        return;


    emit addFadeInAudio();
    emit filterAdded();

}

void TimelineDock::addFadeInVideoFilter()//同时添加fade in video, fade in audio
{
    if (selection().count() <=0)
        return;

    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }


    //判断选中的是视频clip
    int trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex <  m_model.trackList().size());
    const Track& track = m_model.trackList().value(trackIndex);
    if (track.type != VideoTrackType)
        return;


    emit addFadeInVideo();
    emit filterAdded();

}
void TimelineDock::addFadeOutAudioFilter()
{

    if (selection().count() <=0)
        return;
    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }



    //判断选中的是视频clip
    int trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    const Track& track = m_model.trackList().value(trackIndex);
    if (track.type != VideoTrackType)
        return;


    emit addFadeOutAudio();
    emit filterAdded();
}

void TimelineDock::addFadeOutVideoFilter()
{
    if (selection().count() <=0)
        return;
    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }


    //判断选中的是视频clip
    int trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    const Track& track = m_model.trackList().value(trackIndex);
    if (track.type != VideoTrackType)
        return;


    emit addFadeOutVideo();
    emit filterAdded();

}

void TimelineDock::addVolumeFilter()
{
    if (selection().count() <=0)
        return;

    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }



//    //判断选中的是视频clip
//    int trackIndex = currentTrack();
//    const Track& track = m_model.trackList().value(trackIndex);
//    if (track.type != VideoTrackType)
//        return;

    emit addVolume();
    emit filterAdded();
}

void TimelineDock::addTextFilter()
{
    if (selection().count() <=0)
        return;

    if (isTrackLocked(currentTrack())) {
        pulseLockButtonOnTrack(currentTrack());
        return;
    }


    //判断选中的是视频clip
    int trackIndex = currentTrack();
    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    const Track& track = m_model.trackList().value(trackIndex);
    if (track.type != VideoTrackType)
        return;

    emit addText();
    emit filterAdded();
}

void TimelineDock::appendFromPath(int trackIndex, const QString &path)
{
#if SHARE_VERSION
#else
    if (path.endsWith(".vob", Qt::CaseInsensitive) || path.endsWith(".m4p", Qt::CaseInsensitive))
    {
        QMessageBox dialog(QMessageBox::NoIcon,
                                     qApp->applicationName(),
                                     tr("For reasons of copyright protection, you can not import vob or m4p files"),
                                     QMessageBox::Ok,
                                     this);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
        dialog.setWindowModality(QmlApplication::dialogModality());
        dialog.setDefaultButton(QMessageBox::Ok);
        int r = dialog.exec();
        return;
    }
#endif

    if (path.endsWith(".mmp", Qt::CaseInsensitive))
    {
        MAIN.open(path);
        return;
    }

    Mlt::Producer *p = new Mlt::Producer(MLT.profile(), path.toUtf8().constData());
    Q_ASSERT(p);
    if (!p) {
        return;
    }
    Q_ASSERT(p->is_valid());
    if (p->is_valid()) {
//            if (!MLT.producer())
//                MAIN.open(path);
        MLT.setProducer(p);

        // Convert avformat to avformat-novalidate so that XML loads faster.
        if (!qstrcmp(p->get("mlt_service"), "avformat")) {
            p->set("mlt_service", "avformat-novalidate");
            p->set("mute_on_pause", 0);
        }
        MLT.setImageDurationFromDefault(p);
        MAIN.appendClipToPlaylist();
        MAIN.onFileOpened(path);

        qApp->processEvents();

        Timeline::AppendClipCommand *appendCommand = new Timeline::AppendClipCommand(m_model, trackIndex, MLT.XML(p));
        MAIN.pushCommand(appendCommand);

        qApp->processEvents();
    }
}

void TimelineDock::appendFromUrls(int trackIndex, QList<QUrl> urlList)
{
    foreach (QUrl url, urlList) {
        QString path = Util::removeFileScheme(url);
        //this->appendFromPath(trackIndex, path);

        //模板文件拷贝到模板目录，再加到时间线
        MAINCONTROLLER.appendToTimelineFromPath(trackIndex, path);
    }
}


void TimelineDock::appendFromAbstractModelItemDataList(int trackIndex, QByteArray array)
{
    QDataStream stream(&array, QIODevice::ReadOnly);
    QMap<int,  QVariant> roleDataMap;
    while (!stream.atEnd()) {
        int row, col;
        stream >> row >> col >> roleDataMap;
    }
    if (roleDataMap.contains(Qt::ToolTipRole)) {
        // DisplayRole is just basename, ToolTipRole contains full path
        //this->appendFromPath(trackIndex, roleDataMap[Qt::ToolTipRole].toString());
        MAINCONTROLLER.appendToTimelineFromPath(trackIndex, roleDataMap[Qt::ToolTipRole].toString());

    }
    else
    {
         MAIN.appendTextToTimeline(roleDataMap[Qt::StatusTipRole].toString().toInt());
    }
}

int TimelineDock::removeTransitionOnClip(int trackIndex, int clipIndex)
{
    if (clipIndex >= 0 && trackIndex >= 0) {
        Q_ASSERT(trackIndex < m_model.trackList().size());
        int i = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        Q_ASSERT(track);
        if (track) {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            if (!m_model.isTransition(playlist, clipIndex)) {

                if (clipIndex > 0 )
                {
                    if (m_model.isTransition(playlist, clipIndex - 1))
                    {

                        int position = playlist.clip_start(clipIndex - 1);
                        Q_UNUSED(position)
//                        MAIN.pushCommand(
//                                    new Timeline::RemoveTransitionCommand(m_model, trackIndex, clipIndex - 1, clipIndex - 1, position)
//                                    );
                        m_model.removeTransition(trackIndex, clipIndex - 1);
                        clipIndex --;
                    }
                }

                if (clipIndex < playlist.count() - 1)
                {
                    if (m_model.isTransition(playlist, clipIndex + 1))
                    {
                        int position = playlist.clip_start(clipIndex + 1);
                        Q_UNUSED(position)
//                        MAIN.pushCommand(
//                                    new Timeline::RemoveTransitionCommand(m_model, trackIndex, clipIndex + 1, clipIndex + 1, position)
//                                    );
                        m_model.removeTransition(trackIndex, clipIndex + 1);
                    }
                }
            }
        }
    }
    return clipIndex;
}

int TimelineDock::removeTransitionOnClipWithUndo(int trackIndex, int clipIndex)
{
    int newClipIndex = clipIndex;
    bool hasTransition = false;
    if (clipIndex >= 0 && trackIndex >= 0) {
        Q_ASSERT(trackIndex < m_model.trackList().size());
        int i = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        Q_ASSERT(track);
        if (track) {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            if (!m_model.isTransition(playlist, clipIndex)) {

                if (clipIndex > 0 )
                {
                    if (m_model.isTransition(playlist, clipIndex - 1))
                    {
                        hasTransition = true;
                        newClipIndex --;
                    }

                }

                if (clipIndex < playlist.count() - 1)
                {
                    if (m_model.isTransition(playlist, clipIndex + 1))
                    {
                        hasTransition = true;
                    }
                }

                if (hasTransition)
                    MAIN.pushCommand(
                                new Timeline::RemoveTransitionsOnClipCommand(m_model, *this, trackIndex, clipIndex));
            }
        }
    }
    return newClipIndex;
}

void TimelineDock::onTextSettings(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);
        m_selectedTextProducer = producerForClip(trackIndex, clipIndex);
            MAIN.showCurrentTextSettingWidget(m_selectedTextProducer);

}

void TimelineDock::addTransitionOnClipAfterAppend()
{
    int trackIndex = currentTrack();

    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(trackIndex < m_model.trackList().size());
    const Track& track = m_model.trackList().value(trackIndex);
    int trackNumber = track.number;
    TrackType trackType = track.type;

    int i = m_model.trackList().at(trackIndex).mlt_index;

    int clipStart = 0;
    Q_ASSERT(m_model.tractor());
    QScopedPointer<Mlt::Producer> trackProducer(m_model.tractor()->track(i));
    Q_ASSERT(trackProducer);
    if (trackProducer) {
        Mlt::Playlist playlist(*trackProducer);
        Q_ASSERT(playlist.is_valid());
        int clipIndex = playlist.count() - 1;
        clipStart = playlist.clip_start(clipIndex);
        if (trackNumber == 0 && trackType == VideoTrackType)
            if (m_model.addTransitionValid(trackIndex, trackIndex, clipIndex, clipStart - int(MLT.profile().fps())))
                MAIN.pushCommand(
                    new Timeline::AddTransitionCommand(m_model, trackIndex, clipIndex, clipStart - int(MLT.profile().fps())));
    }
}


void TimelineDock::setTransitionDuration(int duration)
{
    int trackIndex = currentTrack();
    Q_ASSERT(m_model.selection().nIndexOfSelectedClip >= 0);
    int clipIndex = m_model.selection().nIndexOfSelectedClip;
    m_model.setTransitionDuration(trackIndex, clipIndex, duration);
}


void TimelineDock::removeTransition(int trackIndex, int transitionIndex)
{
    if (!m_model.trackList().count())
        return;
    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }
    Q_ASSERT(trackIndex >= 0 && transitionIndex >= 0);

    Q_ASSERT(trackIndex < m_model.trackList().size());
    int i = m_model.trackList().at(trackIndex).mlt_index;
    Q_ASSERT(m_model.tractor());
    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
    Q_ASSERT(track);
    if (track) {
        Mlt::Playlist playlist(*track);
        Q_ASSERT(playlist.is_valid());
        if (m_model.isTransition(playlist, transitionIndex)) {
            int position = playlist.clip_start(transitionIndex);
            MAIN.pushCommand(
                        new Timeline::RemoveTransitionCommand(m_model, trackIndex, transitionIndex, transitionIndex, position)
                        );
        }
    }
}

//#ifdef MOVIEMATOR_PRO
void TimelineDock::changePostionFromFilter(int position)
{
    int trackIndex = currentTrack();
    if ( selection().count() > 0)
    {
        int clipIndex = selection().first();

        Q_ASSERT(trackIndex >= 0);
        Q_ASSERT(trackIndex < m_model.trackList().size());
        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        Q_ASSERT(track);
        if (track)
        {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            int start = playlist.clip_start(clipIndex);
            position += start;

            seeked(position);
        }
    }
}


void TimelineDock::setCurrentFilter(QObject *filter, QmlMetadata *meta, int index)
{
    Q_UNUSED(meta)
    Q_UNUSED(index)
    Q_ASSERT(m_quickView.rootContext());

    m_quickView.rootContext()->setContextProperty("currentFilter", filter);
}

//#endif

int TimelineDock::getPositionInCurrentClip()
{
//    int trackIndex = currentTrack();//需要使用和clip统一的track
    int trackIndex = selectedTrackIndex();
    int position = -1;
    if ( selection().count() > 0)
    {
        int clipIndex = selection().first();
//        Q_ASSERT(trackIndex >= 0);
        Q_ASSERT(trackIndex < m_model.trackList().length());
        if((trackIndex < 0) || (trackIndex >= m_model.trackList().length())) return position;
        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return position;
        }
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        Q_ASSERT(track);
        if (track)
        {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            Q_ASSERT(clipIndex < playlist.count());
            int start = playlist.clip_start(clipIndex);
            int end = playlist.clip_start(clipIndex) + playlist.clip_length(clipIndex) - 1;
            if (m_position < start)
                position = -1;//0;
            else if (m_position > end)
                position = -1;//end - start;
            else
                position = m_position - start;
        }
    }

    return position;
}

int TimelineDock::getCurrentClipLength()
{
//    int trackIndex = currentTrack();//需要使用和clip统一的track
    int trackIndex = selectedTrackIndex();
    int length = -1;
    if ( selection().count() > 0)
    {
        int clipIndex = selection().first();
//        Q_ASSERT(trackIndex >= 0);
        Q_ASSERT(trackIndex < m_model.trackList().length());
        if((trackIndex < 0) || (trackIndex >= m_model.trackList().length())) return -1;
        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return -1;
        }
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        Q_ASSERT(track);
        if (track)
        {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            Q_ASSERT(clipIndex < playlist.count());
            length = playlist.clip_length(clipIndex);
        }
    }

    return length;
}

int TimelineDock::getCurrentClipParentLength()
{
//    int trackIndex = currentTrack();//需要使用和clip统一的track
    int trackIndex = selectedTrackIndex();
    int length = -1;
    if ( selection().count() > 0)
    {
        int clipIndex = selection().first();
        Q_ASSERT(trackIndex >= 0);
        Q_ASSERT(trackIndex < m_model.trackList().length());
        if((trackIndex < 0) || (trackIndex >= m_model.trackList().length())) return -1;
        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return -1;
        }
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        Q_ASSERT(track);
        if (track)
        {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            Q_ASSERT(clipIndex >= 0);
            Q_ASSERT(clipIndex < playlist.count());
            QScopedPointer<Mlt::ClipInfo> clipInfo(playlist.clip_info(clipIndex));
            Q_ASSERT(clipInfo);
            if(!clipInfo)
            {
                qDebug() << "clipInfo" << clipInfo << "clipIndex" << clipIndex << "trackIndex" << trackIndex;
                return -1;
            }
            Mlt::Producer *testproducer = clipInfo->producer;
            Q_ASSERT(testproducer);
            if (!testproducer) {
                return -1;
            }
            int in = testproducer->get_in();
            int out = testproducer->get_out();
            length = out - in + 1;

        }
    }

    return length;
}

void TimelineDock::seekToKeyFrame(int position)
{
//    int trackIndex = currentTrack();//需要使用和clip统一的track
    int trackIndex = selectedTrackIndex();
    if ( selection().count() > 0)
    {
        int clipIndex = selection().first();

        Q_ASSERT(trackIndex >= 0);
        Q_ASSERT(trackIndex < m_model.trackList().size());
        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        Q_ASSERT(track);
        if (track)
        {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            Q_ASSERT(clipIndex < playlist.count());
            int start = playlist.clip_start(clipIndex);
            setPosition(start + position);
        }
    }
}

int TimelineDock::getPositionOnParentProducer(int position)
{
    int ret = -1;
    if (selection().count() > 0)
    {
        int clipIndex = selection().first();
//        int trackIndex = currentTrack();//需要使用和clip统一的track
        int trackIndex = selectedTrackIndex();
//        Q_ASSERT(trackIndex >= 0);
        Q_ASSERT(trackIndex < m_model.trackList().length());
        if((trackIndex < 0) || (trackIndex >= m_model.trackList().length())) {
            return -1;
        }
        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return -1;
        }
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        Q_ASSERT(track);
        if (track)
        {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            Q_ASSERT(clipIndex < playlist.count());
            Mlt::ClipInfo *info = playlist.clip_info(clipIndex);
            Q_ASSERT(info);
            if (info && (info->frame_in > 0))
                ret = position + info->frame_in;
            else
                ret = position;
        }
    }

    return ret;
}

int TimelineDock::getPositionOnClip(int position)
{
//    int trackIndex = currentTrack();//需要使用和clip统一的track
    int trackIndex = selectedTrackIndex();
    int ret = -1;
    if (selection().count() > 0 && trackIndex != -1)
    {
        int clipIndex = selection().first();
        Q_ASSERT(trackIndex >= 0);
        Q_ASSERT(trackIndex < m_model.trackList().size());
        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        if (!m_model.tractor()) {
            return -1;
        }
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        Q_ASSERT(track);
        if (track)
        {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            Mlt::ClipInfo *info = playlist.clip_info(clipIndex);
            Q_ASSERT(info);
            if (info && (info->frame_in > 0)) {
                ret = position - info->frame_in;
            } else {
                ret = position;
            }
        }
    }
    return ret;
}

int TimelineDock::timeToFrames(QString timecode)
{
    // 宏定义警告，无法消除
#if MOVIEMATOR_FREE
    if (MAIN.multitrack())
        return MAIN.multitrack()->time_to_frames(timecode.toUtf8().constData());
#endif
    Q_UNUSED(timecode)
    return 0;
}

void TimelineDock::emitShowFilterDock()
{
    emit showFilterDock();
}

QRect TimelineDock::dockPosition()
{
    return this->geometry();
}

void TimelineDock::setExtraQmlContextProperty(QString name, QObject *object)
{
    Q_ASSERT(m_quickView.rootContext());
    m_quickView.rootContext()->setContextProperty(name, object);
}

void TimelineDock::selectSizeAndPositionFilter(int index)
{
    emit sizeAndPositionFilterSelected(index);
}

void TimelineDock::exportAsTemplate(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);

    if (trackIndex >= 0 && clipIndex >= 0)
    {
        Q_ASSERT(trackIndex < m_model.trackList().size());
        int i = m_model.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_model.tractor());
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        Q_ASSERT(track);
        if (track)
        {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));
            Q_ASSERT(info);
            if (!info) {
                return;
            }
            QString xml = MLT.XML(info->producer);
            qDebug() << xml;
            //MLT.saveXML("C:\\Users\\gdbwin\\Desktop\\test.mlt", info->producer, true);

            Mlt::Producer *tempProducer = new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData());
            Q_ASSERT(tempProducer);
            if (!tempProducer) {
                return;
            }
            //QString templatePath = Util::templatePath();
            //QString sampleFile = QString("%1/Samples/1.png").arg(templatePath);
            // get temp filename
            //QString tmpTemplate = QString("%1/tmp_XXXXX").arg(templatePath);

            //QTemporaryFile tmp(tmpTemplate);
            //tmp.open();
            //tmp.close();
            QString path = Settings.savePath();
            path.append("/template.xml");
            QString filename = QFileDialog::getSaveFileName(this, tr("Save Template"), path, tr("Template (*.xml)"));
            if (!filename.isEmpty()) {
                QFileInfo fi(filename);
                Settings.setSavePath(fi.path());
                if (fi.suffix() != "xml")
                    filename += ".xml";

                tempProducer->set_in_and_out(info->frame_in, info->frame_out);
                tempProducer->set("length", info->frame_count);
                //MLT.saveXML(tmp.fileName(), tempProducer, true);

//                MLT.saveXML(filename, tempProducer, true);
                MLT.saveXMLWithoutProfile(filename, tempProducer, true);

                //info->producer->set_in_and_out(-1, -1);
                //QFile::remove(filename);
                //QFile::copy(tmp.fileName(), filename);
                //QFile::remove(tmp.fileName());
                delete tempProducer;
            }
        }
    }
}

void TimelineDock::exportSelectedClipAsTemplate()
{
    if(!isAClipSelected())
        return;
    int trackIndex = m_model.selection().nIndexOfSelectedTrack;
    int clipIndex = m_model.selection().nIndexOfSelectedClip;
    exportAsTemplate(trackIndex, clipIndex);
}

bool replaceVideofilePath(QString filePath,QString originalStr,QString newStr){
    QString strAll;
    QStringList strList;
    QFile readFile(filePath);
    if(readFile.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
     QTextStream stream(&readFile);
     strAll=stream.readAll();
    }
    readFile.close();
    QFile writeFile(filePath);
    if(writeFile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
         QTextStream stream(&writeFile);
         strList=strAll.split("\n");
         for(int i=0;i<strList.count();i++)
         {
             if(i==strList.count()-1)
             {
                 //最后一行不需要换行
                 stream<<strList.at(i);
             }
             else
             {
                 if(strList.at(i).contains(originalStr))
                 {
                    QString tempStr=strList.at(i);
                    tempStr.replace(0,tempStr.length(),newStr);
                    stream<<tempStr<<'\n';
                 }else {
                    stream<<strList.at(i)<<'\n';
                 }
             }
         }
    }
    writeFile.close();
    return true;
}

void TimelineDock::unitTestCommand()
{
    //    替换工程文件中的资源文件的路径
    QDir testFileDir = QDir(Util::resourcesPath() + "/template/unitTest/");
    QString testFile = testFileDir.absoluteFilePath("test.mmp");
    QString videoFilePath = testFileDir.absoluteFilePath("test.mp4");
    QString newStr = QString("    <property name=\"resource\">")+videoFilePath+QString("</property>");
    replaceVideofilePath(testFile,"test.mp4",newStr);

    testFile = testFileDir.absoluteFilePath("test1.mmp");
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::OverwriteClipCommand(m_model, 1, 0, MLT.XML(MLT.savedProducer())));
        MAIN.undoStack()->undo();
    }

    testFile = testFileDir.absoluteFilePath("test.mmp");

    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        new Timeline::AppendClipCommand(m_model, 1,MLT.XML(MLT.isClip()? nullptr : MLT.savedProducer()));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::InsertClipCommand(m_model, 1, m_position, MLT.XML(MLT.savedProducer())));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        lift(1,0); //LiftClipCommand
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::RemoveClipCommand(m_model, *this, 1, 0, MLT.XML(MLT.savedProducer())));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::NameTrackCommand(m_model, 1, "value"));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::MuteTrackCommand(m_model, 1));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::HideTrackCommand(m_model, 1));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::CompositeTrackCommand(m_model, 1,Qt::CheckState(m_model.data(m_model.index(1), MultitrackModel::IsCompositeRole).toInt())));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::LockTrackCommand(m_model, 1, true));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::MoveClipCommand(m_model, 1, 0, 0, 0));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::TrimClipInCommand(m_model, 1, 0, 266, false));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::TrimClipOutCommand(m_model, 1, 0, 266, false));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::SplitCommand(MAINCONTROLLER.multitrackModel(), 1, 0, 10));
        MAIN.undoStack()->undo();
    }
//    if (!MLT.open(testFile)) {
//        MAIN.open(MLT.producer());
//        MAIN.pushCommand(new Timeline::FadeOutCommand(m_model, 1, 0, 5));
//        MAIN.undoStack()->undo();
//    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::AddTransitionCommand(m_model, 1, 0, m_position));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::TrimTransitionInCommand(m_model, 1, 0, 266));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::TrimTransitionOutCommand(m_model, 1, 2, -1));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::AddTransitionByTrimInCommand(m_model, 1, 0, 266));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::AddTransitionByTrimOutCommand(m_model, 1, 0, 266));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::AddTrackCommand(m_model, VideoTrackType));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::InsertTrackCommand(MAINCONTROLLER.multitrackModel(), 1));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::RemoveTrackCommand(MAINCONTROLLER.multitrackModel(), 1));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        Timeline::UpdateClipCommand* command = new Timeline::UpdateClipCommand(*this, m_model, 1, 0, 10);
        command->setXmlAfter(MLT.XML(MLT.producer()));
        MAIN.pushCommand(command);
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::RemoveTransitionCommand(m_model, 1, 0, 0, 266));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::RemoveTransitionsOnClipCommand(m_model, *this, 1, 0));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::MoveInsertClipCommand(m_model, 0, 1, 1, m_position));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::FilterCommand(*(MAIN.timelineDock()->model()), *(MAIN.filterController()->attachedModel()), 0, "u",  105, 110,false));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::FilterAttachCommand(*(MAIN.timelineDock()->model()), MAIN.filterController()->metadataModel()->get(45), 0, 0, true,false));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::FilterMoveCommand(*(MAIN.timelineDock()->model()), 0, 1,false));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        key_frame_item paraFrom;
        paraFrom.keyFrame = 93;
        paraFrom.paraMap.insert("u", "107.5");

        QVector<key_frame_item> keyFrameFrom;
        keyFrameFrom.insert(0, paraFrom);


        key_frame_item para;
        para.keyFrame = 93;
        para.paraMap.insert("u", "115");

        QVector<key_frame_item> keyFrameAdd;
        keyFrameAdd.insert(0, para);

        MAIN.pushCommand(new Timeline::KeyFrameInsertCommand(*(MAIN.timelineDock()->model()), MLT.producer()->filter(0), keyFrameFrom, keyFrameAdd,false));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        key_frame_item para;
        para.keyFrame = 52;
        para.paraMap.insert("u", "105");

        QVector<key_frame_item> keyFrameRemove;
        keyFrameRemove.insert(0, para);
        MAIN.pushCommand(new Timeline::KeyFrameRemoveCommand(*(MAIN.timelineDock()->model()), MLT.producer()->filter(0), keyFrameRemove,false));
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(new Timeline::KeyFrameUpdateCommand(*(MAIN.timelineDock()->model()), MLT.producer()->filter(0), 52, "u", "105", "121",false));
        MAIN.undoStack()->undo();
    }
//    if (!MLT.open(testFile))
//    {
//        MAIN.open(MLT.producer());
//        MAIN.pushCommand(
//                new Timeline::ClipsSelectCommand(*(MAIN.timelineDock()->model()), QList<int>(), 1, false,
//                                                 m_model.selection().selectedClips, m_model.selection().selectedTrack, m_model.selection().isMultitrackSelected,false));
//        MAIN.undoStack()->undo();
//    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(
                    new Timeline::TransitionPropertyCommand(*(MAIN.timelineDock()), *(MAIN.timelineDock()->model()), 1, 1, "luma", "invert", "0",false)
                    );
        MAIN.undoStack()->undo();
    }
    if (!MLT.open(testFile))
    {
        MAIN.open(MLT.producer());
        MAIN.pushCommand(
                    new Timeline::TransitionDurationSettingCommand(*(MAIN.timelineDock()), *(MAIN.timelineDock()->model()), 1, 1, 10,false)
                    );
        MAIN.undoStack()->undo();
    }

    qDebug()<<"end";
}


void TimelineDock::onSelectionChanged(TIMELINE_SELECTION selectionOld, TIMELINE_SELECTION selectionNew)
{
    emit selectionChanged();
    emitSelectedFromSelection();
}












