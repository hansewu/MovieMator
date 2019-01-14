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


#include "../maincontroller.h"
#include <recentdockinterface.h>

void TimelineDock::filterScrollChild(QObject* parent)
{
    QObjectList children = parent->children();
 //   if(children.size() == 0)
    {
        const QMetaObject *mobj = parent->metaObject();
        qDebug()<<"class name is "<<mobj->className()<<"obj name is "<<parent->objectName();

        //if(QString(mobj->className()) == QString("QScrollArea"))
        //            ((QScrollArea *)parent)->viewport()->installEventFilter(this);
        //if(QString(parent->objectName()).contains(QString("qt_scrollarea_viewport")))
          //  ((QWidget *)parent)->installEventFilter(this);
        if(QString(mobj->className()) == QString("QScrollBar"))
                    ((QObject *)parent)->installEventFilter(this);

    }
 //   else
    {
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
    m_position(-1),
    m_updateCommand(0),
    m_ignoreNextPositionChange(false),
    m_filterSettingsView(QmlUtilities::sharedEngine(), 0),
    m_model(MAINCONTROLLER.multitrackModel())
{
    LOG_DEBUG() << "begin";
    m_selection.selectedTrack = -1;
    m_selection.isMultitrackSelected = false;


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

    connect(&m_model, SIGNAL(modified()), this, SLOT(clearSelectionIfInvalid()));

    m_quickView.setFocusPolicy(Qt::StrongFocus);
    setWidget(&m_quickView);

    connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onShowFrame(const SharedFrame&)));
#ifdef Q_OS_WIN
    onVisibilityChanged(true);
#else
    connect(this, &QDockWidget::visibilityChanged, this, &TimelineDock::load);
#endif
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
    if (MLT.producer())
        return MLT.producer()->frames_to_time(frames, mlt_time_smpte);
    return QString("");
}

Mlt::ClipInfo *TimelineDock::getClipInfo(int trackIndex, int clipIndex)
{
    Mlt::ClipInfo* result = 0;
    if (clipIndex >= 0 && trackIndex >= 0) {
        int i = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
            result = playlist.clip_info(clipIndex);
        }
    }
    return result;
}

Mlt::Producer *TimelineDock::producerForClip(int trackIndex, int clipIndex)
{
    Mlt::Producer* result = 0;
    Mlt::ClipInfo* info = getClipInfo(trackIndex, clipIndex);
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
    if (trackIndex >= 0 && trackIndex < m_model.trackList().size()) {
        int i = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
            result = playlist.get_clip_index_at(position);
            if (result > playlist.count() - 1)
                result = -1;
        }
    }
    return result;
}

bool TimelineDock::isBlank(int trackIndex, int clipIndex)
{
    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);
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
    if (trackIndex >= 0 && trackIndex < m_model.trackList().size()) {
        int i = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
            return playlist.count();
        }
    }
    return 0;
}

void TimelineDock::setCurrentTrack(int currentTrack)
{
    if (!m_quickView.rootObject())
        return;
    m_quickView.rootObject()->setProperty("currentTrack", currentTrack);
}

int TimelineDock::currentTrack() const
{
    if (!m_quickView.rootObject())
        return 0;
    return m_quickView.rootObject()->property("currentTrack").toInt();
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
   if (e->type() == QEvent::Wheel)
   {
       const QMetaObject *mobj = o->metaObject();
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

void TimelineDock::setSelection(QList<int> newSelection, int trackIndex, bool isMultitrack, bool bFromUndo)
{
    if (newSelection != selection()
            || trackIndex != m_selection.selectedTrack
            || isMultitrack != m_selection.isMultitrackSelected)
    {
        LOG_DEBUG() << "Changing selection to" << newSelection << " trackIndex" << trackIndex << "isMultitrack" << isMultitrack;
        if(!bFromUndo)
        {
            MAIN.undoStack()->push(
                new Timeline::ClipsSelectCommand(newSelection, trackIndex, isMultitrack,
                                                 m_selection.selectedClips, m_selection.selectedTrack, m_selection.isMultitrackSelected));
        }
        m_selection.selectedClips = newSelection;
        m_selection.selectedTrack = trackIndex;
        m_selection.isMultitrackSelected = isMultitrack;

        MLT.pause();

        //MovieMator Pro
//#ifdef MOVIEMATOR_PRO
        setCurrentFilter(NULL, 0,0);
//#endif
        //end

        emit selectionChanged();

        if (!m_selection.selectedClips.isEmpty())
            emitSelectedFromSelection();
        else
            emit selected(0);
    }

}

QList<int> TimelineDock::selection() const
{
    if (!m_quickView.rootObject())
        return QList<int>();
    return m_selection.selectedClips;
}

void TimelineDock::saveAndClearSelection()
{
    m_savedSelection = m_selection;
    m_selection.selectedClips = QList<int>();
    m_selection.selectedTrack = -1;
    m_selection.isMultitrackSelected = false;
    emit selectionChanged();
}

void TimelineDock::restoreSelection()
{
    m_selection = m_savedSelection;
    emit selectionChanged();
    emitSelectedFromSelection();
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
            setSelection();
        else
            setSelection(QList<int>() << idx);
        return;
    }

    setCurrentTrack(track);
    setSelection(QList<int>() << clip);
}

void TimelineDock::selectClipAtPosition(int trackIndex, int position)
{
    setCurrentTrack(trackIndex);
    int clipIndex = clipIndexAtPosition(trackIndex, position);
    setSelection(QList<int>() << clipIndex);
}


int TimelineDock::centerOfClip(int trackIndex, int clipIndex)
{
//    Mlt::ClipInfo * clip = getClipInfo(trackIndex, clipIndex);
//    Q_ASSERT(clip);
//    int centerOfClip = clip->start + clip->frame_count / 2;
//    delete clip;
//    clip = 0;
//    return centerOfClip;
}

bool TimelineDock::isTrackLocked(int trackIndex) const
{
    if (trackIndex < 0 || trackIndex >= m_model.trackList().size())
        return false;
    int i = m_model.trackList().at(trackIndex).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
    return track->get_int(kTrackLockProperty);
}

void TimelineDock::trimClipAtPlayhead(TrimLocation location, bool ripple)
{
    int trackIndex = currentTrack(), clipIndex = -1;
    chooseClipAtPosition(m_position, &trackIndex, &clipIndex);
    if (trackIndex < 0 || clipIndex < 0)
        return;
    setCurrentTrack(trackIndex);

    int i = m_model.trackList().at(trackIndex).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
    if (!track)
        return;

    QScopedPointer<Mlt::ClipInfo> info(getClipInfo(trackIndex, clipIndex));
    if (!info)
        return;

    if (location == TrimInPoint) {
        MAIN.undoStack()->push(
            new Timeline::TrimClipInCommand(m_model, trackIndex, clipIndex, m_position - info->start, ripple));
        if (ripple)
            setPosition(info->start);
    } else {
        MAIN.undoStack()->push(
            new Timeline::TrimClipOutCommand(m_model, trackIndex, clipIndex, info->start + info->frame_count - m_position, ripple));
    }
}

bool TimelineDock::isRipple() const
{
    return m_quickView.rootObject()->property("ripple").toBool();
}

void TimelineDock::clearSelectionIfInvalid()
{
    int count = clipCount(currentTrack());

    QList<int> newSelection;
    foreach (int index, selection()) {
        if (index >= count)
            continue;
        if (isBlank(currentTrack(), index))
            continue;

        newSelection << index;
    }
    setSelection(newSelection);
    emit selectionChanged();
}

void TimelineDock::insertTrack()
{
//    MAIN.undoStack()->push(
//                new Timeline::InsertTrackCommand(m_model, currentTrack()));
    MAINCONTROLLER.insertTrack(currentTrack());
}

void TimelineDock::removeTrack()
{

    int trackIndex = currentTrack();
    const Track& track = m_model.trackList().value(trackIndex);

    //trackNumber
    //0 - default video and audio track
    int trackNumber = track.number;


    if (m_model.trackList().size() > 0 && trackNumber > 0)
//        MAIN.undoStack()->push(
//                new Timeline::RemoveTrackCommand(m_model, currentTrack()));
        MAINCONTROLLER.removeTrack(trackIndex);
}

void TimelineDock::onProducerChanged(Mlt::Producer* after)
{
    int trackIndex = currentTrack();
    if (trackIndex < 0 || selection().isEmpty() || !m_updateCommand || !after || !after->is_valid())
        return;

    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }

    QString service = after->get("mlt_service");
    if (service == "avformat" || service == "avformat-novalidate" || service == "timewarp") {
            int i = m_model.trackList().at(trackIndex).mlt_index;
            QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
            if (track) {
                // Ensure the new XML has same in/out point as selected clip by making
                // a copy of the changed producer and copying the in/out from timeline.
                Mlt::Playlist playlist(*track);
                int clipIndex = selection().first();
                QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipIndex));
                if (info) {
                    double oldSpeed = qstrcmp("timewarp", info->producer->get("mlt_service")) ? 1.0 : info->producer->get_double("warp_speed");
                    double newSpeed = qstrcmp("timewarp", after->get("mlt_service")) ? 1.0 : after->get_double("warp_speed");
                    double speedRatio = oldSpeed / newSpeed;

                    int length = qRound(info->length * speedRatio);
                    after->set("length", length);
                    after->set_in_and_out(qMin(qRound(info->frame_in * speedRatio), length - 1),
                                         qMin(qRound(info->frame_out * speedRatio), length - 1));
            }
        }
    }
    QList <int> originSelection = selection();
    QString xmlAfter = MLT.XML(after);
    m_updateCommand->setXmlAfter(xmlAfter);
    setSelection(); // clearing selection prevents a crash
    Timeline::UpdateCommand* command = m_updateCommand;
    m_updateCommand = 0;
    MAIN.undoStack()->push(command);

    //setSelection(originSelection);
}

void TimelineDock::addAudioTrack()
{
    MAIN.undoStack()->push(
        new Timeline::AddTrackCommand(m_model, AudioTrackType));
}

void TimelineDock::addVideoTrack()
{
    MAIN.undoStack()->push(
        new Timeline::AddTrackCommand(m_model, VideoTrackType));
}

void TimelineDock::addFilterTrack()
{
    MAIN.undoStack()->push(
        new Timeline::AddTrackCommand(m_model, FilterTrackType));
}

void TimelineDock::addTextTrack()
{
    MAIN.undoStack()->push(
        new Timeline::AddTrackCommand(m_model, TextTrackType));
}

void TimelineDock::onShowFrame(const SharedFrame& frame)
{
    if (m_ignoreNextPositionChange) {
        m_ignoreNextPositionChange = false;
    } else if (MLT.isMultitrack()) {
     if ( MLT.isMultitrack() && (MLT.producer()->get_speed() != 0) ) {//加此判断解决点击clip有时指针不跳的问题
        m_position = frame.get_position();
        emit positionChanged();
     }
    }
}

void TimelineDock::onSeeked(int position)
{
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


    if (MLT.isSeekableClip())
        MAIN.undoStack()->push(
                                new Timeline::AppendCommand(m_model, trackIndex,
                                MLT.XML(MLT.isClip()? 0 : MLT.savedProducer())));
    else
    {

        QList<FILE_HANDLE> fileList = RecentDock_getSelectedFiles();
        foreach (FILE_HANDLE fileHandle, fileList)
        {
            MAININTERFACE.addToTimeLine(fileHandle);
        }
        if (fileList.count() <= 0)
        {
            if (MLT.isSeekableClip() || MLT.savedProducer()) {
                MAIN.undoStack()->push(
                new Timeline::AppendCommand(m_model, trackIndex,
                        MLT.XML(MLT.isClip()? 0 : MLT.savedProducer())));
            }
        }
    }
    selectClipUnderPlayhead();

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
    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);
    Mlt::Producer* clip = producerForClip(trackIndex, clipIndex);
    if (clip) {
        QString xml = MLT.XML(clip);
        delete clip;


        MAIN.undoStack()->beginMacro("Remove from track");
        int newClipIndex = removeTransitionOnClipWithUndo(trackIndex, clipIndex);
        MAIN.undoStack()->push(
            new Timeline::RemoveCommand(m_model, *this, trackIndex, newClipIndex, xml));
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
    if (clip) {
        if (clip->is_blank())
            return;

        bool leftBlank = false;
        if (clipIndex > 0 && isBlank(trackIndex, clipIndex - 1))
            leftBlank = true;
        bool rightBlank = false;
        if (clipIndex <  clipCount(trackIndex) - 1 && isBlank(trackIndex, clipIndex + 1))
            rightBlank = true;

        MAIN.undoStack()->beginMacro(tr("Lift from track"));

        int newClipIndex = removeTransitionOnClipWithUndo(trackIndex, clipIndex);
        QString xml = MLT.XML(clip.data());
        MAIN.undoStack()->push(
            new Timeline::LiftCommand(m_model, *this, trackIndex, newClipIndex, xml));

        MAIN.undoStack()->endMacro();
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
    if (trackIndex >= 0) {
        setSelection(QList<int>(), trackIndex);
        int i = m_model.trackList().at(trackIndex).mlt_index;
        Mlt::Producer* producer = m_model.tractor()->track(i);
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
    if (info) {
        MAIN.setPauseAfterOpen(false);
        QString xml = MLT.XML(info->producer);
        Mlt::Producer p(MLT.profile(), "xml-string", xml.toUtf8().constData());
        p.set_speed(0);
        p.seek(info->frame_in);
        p.set_in_and_out(info->frame_in, info->frame_out);
        MLT.setSavedProducer(&p);
        emit clipCopied();
    }
}

void TimelineDock::emitSelectedFromSelection()
{
    if (!m_model.trackList().count()) {
        if (m_model.tractor())
            selectMultitrack();
        else
            emit selected(0);
        return;
    }

    int trackIndex = currentTrack();
    int clipIndex = selection().isEmpty()? 0 : selection().first();
    Mlt::ClipInfo* info = getClipInfo(trackIndex, clipIndex);
    if (info && info->producer && info->producer->is_valid()) {
        delete m_updateCommand;
        m_updateCommand = new Timeline::UpdateCommand(*this, trackIndex, clipIndex, info->start);
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
        info->producer->set_in_and_out(0, -1);

        delete info;
    }
}

void TimelineDock::remakeAudioLevels(int trackIndex, int clipIndex)
{
    QModelIndex modelIndex = m_model.index(clipIndex, 0, m_model.index(trackIndex));
    QScopedPointer<Mlt::ClipInfo> info(getClipInfo(trackIndex, clipIndex));
    AudioLevelsTask::start(*info->producer, &m_model, modelIndex, /* force */ true);
}

void TimelineDock::setTrackName(int trackIndex, const QString &value)
{
    MAIN.undoStack()->push(
        new Timeline::NameTrackCommand(m_model, trackIndex, value));
}

void TimelineDock::toggleTrackMute(int trackIndex)
{
    MAIN.undoStack()->push(
        new Timeline::MuteTrackCommand(m_model, trackIndex));
}

void TimelineDock::toggleTrackHidden(int trackIndex)
{
    MAIN.undoStack()->push(
        new Timeline::HideTrackCommand(m_model, trackIndex));
}

void TimelineDock::setTrackComposite(int trackIndex, Qt::CheckState composite)
{
    MAIN.undoStack()->push(
        new Timeline::CompositeTrackCommand(m_model, trackIndex, composite));
}

void TimelineDock::setTrackLock(int trackIndex, bool lock)
{
    MAIN.undoStack()->push(
        new Timeline::LockTrackCommand(m_model, trackIndex, lock));
}

bool TimelineDock::moveClip(int fromTrack, int toTrack, int clipIndex, int position)
{
    int newPosition;
    if (m_model.moveClipValid(fromTrack, toTrack, clipIndex, position)) {
        MAIN.undoStack()->push(
            new Timeline::MoveClipCommand(m_model, fromTrack, toTrack, clipIndex, position));
        selectClipAtPosition(toTrack, position);
        return true;
    } else if (m_model.addTransitionValid(fromTrack, toTrack, clipIndex, position)) {

        MAIN.undoStack()->push(
            new Timeline::AddTransitionCommand(m_model, fromTrack, clipIndex, position));
        emitSelectedFromSelection();
        return true;
    } else if ( ( newPosition = m_model.moveInsertClipValid(fromTrack, toTrack, clipIndex, position) )  >= 0) {
        MAIN.undoStack()->push(
                    new Timeline::MoveInsertClipCommand(m_model, fromTrack, toTrack, clipIndex, position));
        selectClipAtPosition(toTrack, newPosition);
        return true;
    } else {
        return false;
    }
}

void TimelineDock::addFilterClipCommand(int TrackIndex, int clipIndex, QString strFromXml, QString strToXml)
{
 //   MAIN.undoStack()->push(new Timeline::FilterClipCommand(m_model, TrackIndex, clipIndex, strFromXml, strToXml));
}


bool TimelineDock::trimClipIn(int trackIndex, int clipIndex, int delta, bool ripple)
{
    if (!ripple && m_model.addTransitionByTrimInValid(trackIndex, clipIndex, delta)) {
        MAIN.undoStack()->push(
            new Timeline::AddTransitionByTrimInCommand(m_model, trackIndex, clipIndex, delta));
    }
    else if (m_model.trimTransitionOutValid(trackIndex, clipIndex, delta)) {
        MAIN.undoStack()->push(
            new Timeline::TrimTransitionOutCommand(m_model, trackIndex, clipIndex, delta));
    }
    else if (m_model.trimClipInValid(trackIndex, clipIndex, delta, ripple)) {
        MAIN.undoStack()->push(
            new Timeline::TrimClipInCommand(m_model, trackIndex, clipIndex, delta, ripple));
    }
    else return false;
    return true;
}

bool TimelineDock::trimClipOut(int trackIndex, int clipIndex, int delta, bool ripple)
{
    if (!ripple && m_model.addTransitionByTrimOutValid(trackIndex, clipIndex, delta)) {
        MAIN.undoStack()->push(
            new Timeline::AddTransitionByTrimOutCommand(m_model, trackIndex, clipIndex, delta));
    }
    else if (m_model.trimTransitionInValid(trackIndex, clipIndex, delta)) {
        MAIN.undoStack()->push(
            new Timeline::TrimTransitionInCommand(m_model, trackIndex, clipIndex, delta));
    }
    else if (m_model.trimClipOutValid(trackIndex, clipIndex, delta, ripple)) {
        MAIN.undoStack()->push(
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
            : MLT.XML(MLT.isClip()? 0 : MLT.savedProducer());
        if (position < 0)
            position = m_position;
        MAIN.undoStack()->push(
            new Timeline::InsertCommand(m_model, trackIndex, position, xmlToUse));
         selectClipUnderPlayhead();
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
            : MLT.XML(MLT.isClip()? 0 : MLT.savedProducer());
        if (position < 0)
            position = m_position;
        MAIN.undoStack()->push(
            new Timeline::OverwriteCommand(m_model, trackIndex, position, xmlToUse));
        selectClipUnderPlayhead();
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

    if (clipIndex >= 0 && trackIndex >= 0) {
        int i = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
            if (!m_model.isTransition(playlist, clipIndex)) {
//                QScopedPointer<Mlt::ClipInfo> info(getClipInfo(trackIndex, clipIndex));
//                if (info && m_position >= info->start + 2 && m_position < info->start + info->frame_count - 1 - 2) {
//                    MAIN.undoStack()->push(
//                        new Timeline::SplitCommand(m_model, trackIndex, clipIndex, m_position));
//                }
                MAINCONTROLLER.splitClip(trackIndex, clipIndex, m_position);
            } else {
                emit showStatusMessage(tr("You cannot split a transition."));
            }
        }
    }
}

void TimelineDock::fadeIn(int trackIndex, int clipIndex, int duration)
{
    if (isTrackLocked(trackIndex)) {
        pulseLockButtonOnTrack(trackIndex);
        return;
    }
    if (duration < 0) return;
    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);
    MAIN.undoStack()->push(
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
    MAIN.undoStack()->push(
        new Timeline::FadeOutCommand(m_model, trackIndex, clipIndex, duration));
    emit fadeOutChanged(duration);
}

void TimelineDock::seekPreviousEdit()
{
    if (!MLT.isMultitrack()) return;
    if (!m_model.tractor()) return;

    int newPosition = -1;
    int n = m_model.tractor()->count();
    for (int i = 0; i < n; i++) {
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
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
    if (!m_model.tractor()) return;

    int newPosition = std::numeric_limits<int>::max();
    int n = m_model.tractor()->count();
    for (int i = 0; i < n; i++) {
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
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
    if (!m_model.tractor()) return;
    if (clipIndex < 0) return;

    int mltTrackIndex = m_model.trackList().at(currentTrack()).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mltTrackIndex));
    if (track) {
        Mlt::Playlist playlist(*track);
        if (m_position != playlist.clip_start(clipIndex))
            setPosition(playlist.clip_start(clipIndex));
    }
}

void TimelineDock::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(MLT.MltXMLMimeType())) {
        event->acceptProposedAction();
    }
}

void TimelineDock::dragMoveEvent(QDragMoveEvent *event)
{
    emit dragging(event->posF(), event->mimeData()->text().toInt());
}

void TimelineDock::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
    emit dropped();
}

void TimelineDock::dropEvent(QDropEvent *event)
{
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
    m_model.addFilter(currentTrack(), producer);
}

//add text to timeline
//TODO: use undo/redo implement
void TimelineDock::addTextToTimeline(Mlt::Producer *producer)
{


    m_model.addText(currentTrack(), producer);
}
void TimelineDock::onShowProperties(int trackIndex, int clipIndex)
{
    if (!m_model.trackList().count())
        return;

    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0);

    int i = m_model.trackList().at(trackIndex).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);

        Mlt::ClipInfo* i = playlist.clip_info(clipIndex);
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
//            if (row == -1)
//                MAIN.undoStack()->push(new Playlist::AppendCommand(m_model, MLT.XML(&p)));
//            else
//                MAIN.undoStack()->push(new Playlist::InsertCommand(m_model, MLT.XML(&p), insertNextAt++));

        MAIN.undoStack()->push(
            new Timeline::AppendCommand(m_model, trackIndex,
                MLT.XML(p)));
        selectClipUnderPlayhead();

//        addTransitionOnClipAfterAppend();
        qApp->processEvents();
    }
}

void TimelineDock::appendFromUrls(int trackIndex, QList<QUrl> urlList)
{
    foreach (QUrl url, urlList) {
        QString path = Util::removeFileScheme(url);
        //this->appendFromPath(trackIndex, path);
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
        int i = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
            if (!m_model.isTransition(playlist, clipIndex)) {

                if (clipIndex > 0 )
                {
                    if (m_model.isTransition(playlist, clipIndex - 1))
                    {

                        int position = playlist.clip_start(clipIndex - 1);
//                        MAIN.undoStack()->push(
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
//                        MAIN.undoStack()->push(
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
        int i = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
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
                    MAIN.undoStack()->push(
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

    const Track& track = m_model.trackList().value(trackIndex);
    int trackNumber = track.number;
    TrackType trackType = track.type;

    int i = m_model.trackList().at(trackIndex).mlt_index;

    int clipStart = 0;
    QScopedPointer<Mlt::Producer> trackProducer(m_model.tractor()->track(i));
    if (trackProducer) {
        Mlt::Playlist playlist(*trackProducer);
        int clipIndex = playlist.count() - 1;
        clipStart = playlist.clip_start(clipIndex);
        if (trackNumber == 0 && trackType == VideoTrackType)
            if (m_model.addTransitionValid(trackIndex, trackIndex, clipIndex, clipStart - MLT.profile().fps()))
                MAIN.undoStack()->push(
                    new Timeline::AddTransitionCommand(m_model, trackIndex, clipIndex, clipStart - MLT.profile().fps()));
    }
}


void TimelineDock::setTransitionDuration(int duration)
{
    int trackIndex = currentTrack();
    int clipIndex = m_selection.selectedClips[0];
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

    int i = m_model.trackList().at(trackIndex).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(i));
    if (track) {
        Mlt::Playlist playlist(*track);
        if (m_model.isTransition(playlist, transitionIndex)) {
            int position = playlist.clip_start(transitionIndex);
            MAIN.undoStack()->push(
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

        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        if (track)
        {
            Mlt::Playlist playlist(*track);
            int start = playlist.clip_start(clipIndex);
            position += start;

            seeked(position);
        }
    }
}


void TimelineDock::setCurrentFilter(QObject *filter, QmlMetadata *meta, int index)
{
    m_quickView.rootContext()->setContextProperty("currentFilter", filter);
}

//#endif

int TimelineDock::getPositionInCurrentClip()
{
    int trackIndex = currentTrack();
    int trackIndex2 = selectedTrack();
    int position = -1;
    if ( selection().count() > 0)
    {
        int clipIndex = selection().first();

        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        if (track)
        {
            Mlt::Playlist playlist(*track);
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
    int trackIndex = currentTrack();
    int length = -1;
    if ( selection().count() > 0)
    {
        int clipIndex = selection().first();

        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        if (track)
        {
            Mlt::Playlist playlist(*track);
            length = playlist.clip_length(clipIndex);
        }
    }

    return length;
}

int TimelineDock::getCurrentClipParentLength()
{
    int trackIndex = currentTrack();
    int length = -1;
    if ( selection().count() > 0)
    {
        int clipIndex = selection().first();

        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        if (track)
        {
            Mlt::Playlist playlist(*track);

            QScopedPointer<Mlt::ClipInfo> clipInfo(playlist.clip_info(clipIndex));
            Mlt::Producer *testproducer = clipInfo->producer;
            int in = testproducer->get_in();
            int out = testproducer->get_out();
            length = out - in + 1;

        }
    }

    return length;
}

void TimelineDock::seekToKeyFrame(int position)
{
    int trackIndex = currentTrack();
    if ( selection().count() > 0)
    {
        int clipIndex = selection().first();

        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        if (track)
        {
            Mlt::Playlist playlist(*track);
            int start = playlist.clip_start(clipIndex);
            setPosition(start + position);
        }
    }
}

int TimelineDock::getPositionOnParentProducer(int position)
{
    int trackIndex = currentTrack();
    int ret = -1;
    if (selection().count() > 0)
    {
        int clipIndex = selection().first();
        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        if (track)
        {
            Mlt::Playlist playlist(*track);
            Mlt::ClipInfo *info = playlist.clip_info(clipIndex);
            if (info->frame_in > 0)
                ret = position + info->frame_in;
            else
                ret = position;
        }
    }

    return ret;
}

int TimelineDock::getPositionOnClip(int position)
{
    int trackIndex = currentTrack();
    int ret = -1;
    if (selection().count() > 0)
    {
        int clipIndex = selection().first();
        int mlt_index = m_model.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_model.tractor()->track(mlt_index));
        if (track)
        {
            Mlt::Playlist playlist(*track);
            Mlt::ClipInfo *info = playlist.clip_info(clipIndex);
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
#if MOVIEMATOR_FREE
    if (MAIN.multitrack())
        return MAIN.multitrack()->time_to_frames(timecode.toUtf8().constData());
#endif
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
    m_quickView.rootContext()->setContextProperty(name, object);
}
