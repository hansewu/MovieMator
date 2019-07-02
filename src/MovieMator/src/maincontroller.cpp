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

//#include <PythonQt.h>
#include "maincontroller.h"
#include "mainwindow.h"
#include "docks/timelinedock.h"
#include <QUndoStack>
#include "commands/timelinecommands.h"
#include "models/multitrackmodel.h"
#include "controllers/filtercontroller.h"
#include "settings.h"
#include "docks/encodedock.h"
#include <QMessageBox>
#include <qmlapplication.h>
#include <shotcut_mlt_properties.h>



static MainController *g_mainController = nullptr;
MainController::MainController(QObject *parent) : QObject(parent)
{

}

MainController& MainController::sharedInstance()
{
    if(!g_mainController)
    {
        g_mainController = new MainController();
    }

    return *g_mainController;
}


int MainController::initPythonQt()
{
  /*  PythonQt::init();
    PythonQtObjectPtr  mainContext = PythonQt::self()->getMainModule();
    mainContext.addObject("mainwindow", &MAIN);
    mainContext.addObject("timeline", MAIN.timelineDock());
    mainContext.addObject("player", (QObject *)MAIN.player());
    mainContext.addObject("maincontroller", this);
*/
    return 0;
}

void MainController::evalFile(const QString &file)
{
    Q_UNUSED(file)
  //  PythonQtObjectPtr  mainContext = PythonQt::self()->getMainModule();
  //  mainContext.evalFile(file);
}


void MainController::appendToTimelineTrack(int trackIndex, const QStringList &files)
{
//     QList<QUrl> fileList;
//     foreach (QString file, files) {
//         fileList.append(QUrl::fromLocalFile(file));
//     }
//     MAIN.timelineDock()->appendFromUrls(trackIndex, fileList);

    foreach (QString file, files) {
        appendToTimelineFromPath(trackIndex, file);
    }
}

int MainController::appendToTimelineFromPath(int trackIndex, const QString &path)
{
#if SHARE_VERSION
#else
    if (path.endsWith(".vob", Qt::CaseInsensitive) || path.endsWith(".m4p", Qt::CaseInsensitive))
    {
        QMessageBox dialog(QMessageBox::NoIcon,
                                     qApp->applicationName(),
                                     tr("For reasons of copyright protection, you can not import vob or m4p files"),
                                     QMessageBox::Ok);
                                    // this);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
        dialog.setWindowModality(QmlApplication::dialogModality());
        dialog.setDefaultButton(QMessageBox::Ok);
        int r = dialog.exec();
        return -1;
    }
#endif

    if (path.endsWith(".mmp", Qt::CaseInsensitive))
    {
        MAIN.open(path);
        return -1;
    }

    Mlt::Producer *p = new Mlt::Producer(MLT.profile(), path.toUtf8().constData());
    Q_ASSERT(p);
    Q_ASSERT(p->is_valid());
    if (p->is_valid()) {
        //if (path.endsWith(".mlt"))
        //    p->set(kShotcutVirtualClip, 1);
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

        MAIN.pushCommand(
            new Timeline::AppendClipCommand(m_multitrackModel, trackIndex,
                MLT.XML(p)));
//        selectClipUnderPlayhead();
        MAIN.timelineDock()->selectClipUnderPlayhead();

        qApp->processEvents();

        int clipCount = m_multitrackModel.rowCount(m_multitrackModel.index(trackIndex));
        return clipCount - 1;
    }

    return -1;
}

void MainController::insertTrack(int trackIndex)
{
    MAIN.pushCommand(
                new Timeline::InsertTrackCommand(m_multitrackModel, trackIndex));
}

void MainController::removeTrack(int trackIndex)
{
    MAIN.pushCommand(
            new Timeline::RemoveTrackCommand(m_multitrackModel, trackIndex));
}

void MainController::splitClip(int trackIndex, int clipIndex, int position)
{
    Q_ASSERT(clipIndex >= 0 && trackIndex >= 0);
    if (trackIndex < 0 || clipIndex < 0)
        return;
    if (clipIndex >= 0 && trackIndex >= 0) {
        Q_ASSERT(trackIndex < m_multitrackModel.trackList().size());
        int i = m_multitrackModel.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_multitrackModel.tractor());
        QScopedPointer<Mlt::Producer> track(m_multitrackModel.tractor()->track(i));
        Q_ASSERT(track);
        if (track) {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            if (!m_multitrackModel.isTransition(playlist, clipIndex)) {
                QScopedPointer<Mlt::ClipInfo> info(getClipInfo(trackIndex, clipIndex));
                Q_ASSERT(info);
                if (info && position >= info->start + 2 && position < info->start + info->frame_count - 1 - 2) {
                    MAIN.pushCommand(
                        new Timeline::SplitCommand(m_multitrackModel, trackIndex, clipIndex, position));
                }

            }
        }
    }
}

Mlt::ClipInfo *MainController::getClipInfo(int trackIndex, int clipIndex)
{
    Mlt::ClipInfo* result = nullptr;
    Q_ASSERT(clipIndex >= 0 && trackIndex >= 0);
    if (clipIndex >= 0 && trackIndex >= 0) {
        Q_ASSERT(trackIndex < m_multitrackModel.trackList().size());
        int i = m_multitrackModel.trackList().at(trackIndex).mlt_index;
        Q_ASSERT(m_multitrackModel.tractor());
        QScopedPointer<Mlt::Producer> track(m_multitrackModel.tractor()->track(i));
        Q_ASSERT(track);
        if (track) {
            Mlt::Playlist playlist(*track);
            Q_ASSERT(playlist.is_valid());
            Q_ASSERT(clipIndex < playlist.count());
            result = playlist.clip_info(clipIndex);
        }
    }
    return result;
}

void MainController::setTransitionDuration(int trackIndex, int clipIndex, int duration)
{
    m_multitrackModel.setTransitionDuration(trackIndex, clipIndex, duration);
}

Mlt::Transition *MainController::getTransition(Mlt::Producer *producer, const QString &name)
{
    Q_ASSERT(producer);
    QScopedPointer<Mlt::Service> service(producer->producer());
    Q_ASSERT(service);
    Q_ASSERT(service->is_valid());
    while (service && service->is_valid()) {
        if (service->type() == transition_type) {
            Mlt::Transition transition(*service);
            if (name == transition.get("mlt_service"))
                return new Mlt::Transition(transition);
            else if (name == "luma" && QString("movit.luma_mix") == transition.get("mlt_service"))
                return new Mlt::Transition(transition);
        }
        service.reset(service->producer());
    }
    return nullptr;
}


void MainController::setTransitionType(int trackIndex, int clipIndex, int transitionIndex)
{
    Q_ASSERT(m_multitrackModel.trackList().count());
    if (!m_multitrackModel.trackList().count())
        return;

    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0 && transitionIndex >= 0);

    Q_ASSERT(trackIndex < m_multitrackModel.trackList().size());
    int i = m_multitrackModel.trackList().at(trackIndex).mlt_index;
    Q_ASSERT(m_multitrackModel.tractor());
    QScopedPointer<Mlt::Producer> track(m_multitrackModel.tractor()->track(i));
    Q_ASSERT(track);

    Mlt::Producer *producer = nullptr;
    if (track) {
        Mlt::Playlist playlist(*track);
        Q_ASSERT(playlist.is_valid());
        Q_ASSERT(clipIndex < playlist.count());
        Mlt::ClipInfo* i = playlist.clip_info(clipIndex);
        Q_ASSERT(i);
        if (i) {
            producer = i->producer;
        }
    }

    if(!producer)
        return;

    QScopedPointer<Mlt::Transition> transition(getTransition(producer, "luma"));
    Q_ASSERT(transition);
    Q_ASSERT(transition->is_valid());

    if (transition && transition->is_valid()) {
        if (transitionIndex == 0) {
            transition->set("resource", "");
        } else if (transitionIndex == 1) { // Cut
        }
        else {
            transition->set("resource", QString("%luma%1.pgm").arg(transitionIndex - 1, 2, 10, QChar('0')).toLatin1().constData());
        }
        if (qstrcmp(transition->get("resource"), "")) {
            transition->set("progressive", 1);
            if (transitionIndex == 1) {
                transition->set("invert", 0);
                transition->set("softness", 0);
            }
        }
        MLT.refreshConsumer();
    }
}

void MainController::addTransition(int trackIndex, int clipIndex, double duration)
{

    Q_ASSERT(trackIndex >= 0);
    Q_ASSERT(clipIndex >= 0);
    Q_ASSERT(trackIndex < m_multitrackModel.trackList().size());
    const Track& track = m_multitrackModel.trackList().value(trackIndex);
    Q_UNUSED(track)


    int i = m_multitrackModel.trackList().at(trackIndex).mlt_index;

    int clipStart = 0;
    Q_ASSERT(m_multitrackModel.tractor());
    QScopedPointer<Mlt::Producer> trackProducer(m_multitrackModel.tractor()->track(i));
    Q_ASSERT(trackProducer);
    if (trackProducer) {
        Mlt::Playlist playlist(*trackProducer);
        Q_ASSERT(playlist.is_valid());
        Q_ASSERT(clipIndex < playlist.count());
        clipStart = playlist.clip_start(clipIndex);
        if (m_multitrackModel.addTransitionValid(trackIndex, trackIndex, clipIndex, clipStart + int(duration * MLT.profile().fps()) ))
                MAIN.pushCommand(
                    new Timeline::AddTransitionCommand(m_multitrackModel, trackIndex, clipIndex, clipStart + int(duration* MLT.profile().fps()) ));
    }
}

int MainController::getStartPositionOfClip(int trackIndex, int clipIndex)
{
    return m_multitrackModel.getStartPositionOfClip(trackIndex, clipIndex);
}

void MainController::setClipSpeed(int trackIndex, int clipIndex, double speed)
{
    Q_ASSERT(MAIN.timelineDock());
    MAIN.timelineDock()->setCurrentTrack(trackIndex);
    MAIN.timelineDock()->setSelection(QList<int>() << clipIndex, trackIndex);
    m_multitrackModel.setClipSpeed(trackIndex, clipIndex, speed);
}

void MainController::addFilter(int trackIndex, int clipIndex, const QString &filterID)
{
    Q_ASSERT(MAIN.timelineDock());
    MAIN.showFilterDock();
    MAIN.timelineDock()->setCurrentTrack(trackIndex);
    MAIN.timelineDock()->setSelection(QList<int>() << clipIndex, trackIndex);
    MAIN.filterController()->addFilter(filterID);
    qApp->processEvents();
}

void MainController::removeFilter(int trackIndex, int clipIndex, int row)
{
    Q_UNUSED(trackIndex)
    Q_UNUSED(clipIndex)
//    MAIN.showFilterDock();
//    MAIN.timelineDock()->setCurrentTrack(trackIndex);
//    MAIN.timelineDock()->setSelection(QList<int>() << clipIndex);
    Q_ASSERT(MAIN.filterController());
    MAIN.filterController()->removeFilter(row);
}

void MainController::copyClip(int trackIndex, int clipIndex)
{
    Q_ASSERT(MAIN.timelineDock());
    MAIN.timelineDock()->setCurrentTrack(trackIndex);
    MAIN.timelineDock()->setSelection(QList<int>() << clipIndex, trackIndex);
    MAIN.on_actionCopy_triggered();
}

void MainController::cutClip(int trackIndex, int clipIndex)
{
    Q_ASSERT(MAIN.timelineDock());
    MAIN.timelineDock()->setCurrentTrack(trackIndex);
    MAIN.timelineDock()->setSelection(QList<int>() << clipIndex, trackIndex);
    MAIN.on_actionCut_triggered();
}

void MainController::pasteClip(int trackIndex, int position)
{
    Q_ASSERT(MAIN.timelineDock());
    MAIN.timelineDock()->setCurrentTrack(trackIndex);
    MAIN.timelineDock()->setPosition(position);
    MAIN.on_actionPaste_triggered();
}


void MainController::setProfile(const QString &name)
{
    Settings.setPlayerProfile(name);
    MAIN.setProfile(name);
    MLT.restart();
}

void MainController::undo()
{
    Q_ASSERT(MAIN.undoStack());
    MAIN.undoStack()->undo();
}

void MainController::redo()
{
    Q_ASSERT(MAIN.undoStack());
    MAIN.undoStack()->redo();
}

void MainController::exportFile(const QString &path)
{
    Q_ASSERT(MAIN.encodeDock());
    MAIN.encodeDock()->enqueueMelt(path, -1);
}

int MainController::videoTrackNums()
{
    return m_multitrackModel.trackList().first().number + 1;
}

int MainController::audioTrackNums()
{
    return m_multitrackModel.trackList().last().number + 1;
}

int MainController::trackNums()
{
    return m_multitrackModel.trackList().count();
}

int MainController::clipsNum(int trackIndex)
{
    return m_multitrackModel.rowCount(m_multitrackModel.index(trackIndex));
}
