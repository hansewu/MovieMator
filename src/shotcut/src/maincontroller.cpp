//#include <PythonQt.h>
#include "maincontroller.h"
#include "mainwindow.h"
#include "Docks/timelinedock.h"
#include <QUndoStack>
#include "commands/timelinecommands.h"
#include "models/multitrackmodel.h"
#include "controllers/filtercontroller.h"
#include "settings.h"
#include "docks/encodedock.h"
#include <QMessageBox>
#include "qmltypes/qmlapplication.h"



static MainController *g_mainController = NULL;
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
    if (p->is_valid()) {
        MLT.setProducer(p);

        // Convert avformat to avformat-novalidate so that XML loads faster.
        if (!qstrcmp(p->get("mlt_service"), "avformat")) {
            p->set("mlt_service", "avformat-novalidate");
            p->set("mute_on_pause", 0);
        }
        MLT.setImageDurationFromDefault(p);
        MAIN.appendClipToPlaylist();
        MAIN.addToRecentDock(path);

        qApp->processEvents();

        MAIN.undoStack()->push(
            new Timeline::AppendCommand(m_multitrackModel, trackIndex,
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
    MAIN.undoStack()->push(
                new Timeline::InsertTrackCommand(m_multitrackModel, trackIndex));
}

void MainController::removeTrack(int trackIndex)
{
    MAIN.undoStack()->push(
            new Timeline::RemoveTrackCommand(m_multitrackModel, trackIndex));
}

void MainController::splitClip(int trackIndex, int clipIndex, int position)
{
    if (trackIndex < 0 || clipIndex < 0)
        return;
    if (clipIndex >= 0 && trackIndex >= 0) {
        int i = m_multitrackModel.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_multitrackModel.tractor()->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
            if (!m_multitrackModel.isTransition(playlist, clipIndex)) {
                QScopedPointer<Mlt::ClipInfo> info(getClipInfo(trackIndex, clipIndex));
                if (info && position >= info->start + 2 && position < info->start + info->frame_count - 1 - 2) {
                    MAIN.undoStack()->push(
                        new Timeline::SplitCommand(m_multitrackModel, trackIndex, clipIndex, position));
                }

            }
        }
    }
}

Mlt::ClipInfo *MainController::getClipInfo(int trackIndex, int clipIndex)
{
    Mlt::ClipInfo* result = 0;
    if (clipIndex >= 0 && trackIndex >= 0) {
        int i = m_multitrackModel.trackList().at(trackIndex).mlt_index;
        QScopedPointer<Mlt::Producer> track(m_multitrackModel.tractor()->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
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
    QScopedPointer<Mlt::Service> service(producer->producer());
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
    return 0;
}


void MainController::setTransitionType(int trackIndex, int clipIndex, int transitionIndex)
{
    if (!m_multitrackModel.trackList().count())
        return;

    Q_ASSERT(trackIndex >= 0 && clipIndex >= 0 && transitionIndex >= 0);

    int i = m_multitrackModel.trackList().at(trackIndex).mlt_index;
    QScopedPointer<Mlt::Producer> track(m_multitrackModel.tractor()->track(i));

    Mlt::Producer *producer = 0;
    if (track) {
        Mlt::Playlist playlist(*track);
        Mlt::ClipInfo* i = playlist.clip_info(clipIndex);
        if (i) {
            producer = i->producer;
        }
    }

    if(!producer)
        return;

    QScopedPointer<Mlt::Transition> transition(getTransition(producer, "luma"));

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

    const Track& track = m_multitrackModel.trackList().value(trackIndex);


    int i = m_multitrackModel.trackList().at(trackIndex).mlt_index;

    int clipStart = 0;
    QScopedPointer<Mlt::Producer> trackProducer(m_multitrackModel.tractor()->track(i));
    if (trackProducer) {
        Mlt::Playlist playlist(*trackProducer);
        clipStart = playlist.clip_start(clipIndex);
        if (m_multitrackModel.addTransitionValid(trackIndex, trackIndex, clipIndex, clipStart + duration * MLT.profile().fps() ))
                MAIN.undoStack()->push(
                    new Timeline::AddTransitionCommand(m_multitrackModel, trackIndex, clipIndex, clipStart + duration* MLT.profile().fps() ));
    }
}

int MainController::getStartPositionOfClip(int trackIndex, int clipIndex)
{
    return m_multitrackModel.getStartPositionOfClip(trackIndex, clipIndex);
}

void MainController::setClipSpeed(int trackIndex, int clipIndex, double speed)
{
    MAIN.timelineDock()->setCurrentTrack(trackIndex);
    MAIN.timelineDock()->setSelection(QList<int>() << clipIndex);
    m_multitrackModel.setClipSpeed(trackIndex, clipIndex, speed);
}

void MainController::addFilter(int trackIndex, int clipIndex, const QString &filterID)
{
    MAIN.showFilterDock();
    MAIN.timelineDock()->setCurrentTrack(trackIndex);
    MAIN.timelineDock()->setSelection(QList<int>() << clipIndex);
    MAIN.filterController()->addFilter(filterID);
    qApp->processEvents();
}

void MainController::removeFilter(int trackIndex, int clipIndex, int row)
{
//    MAIN.showFilterDock();
//    MAIN.timelineDock()->setCurrentTrack(trackIndex);
//    MAIN.timelineDock()->setSelection(QList<int>() << clipIndex);
    MAIN.filterController()->removeFilter(row);
}

void MainController::copyClip(int trackIndex, int clipIndex)
{
    MAIN.timelineDock()->setCurrentTrack(trackIndex);
    MAIN.timelineDock()->setSelection(QList<int>() << clipIndex);
    MAIN.on_actionCopy_triggered();
}

void MainController::cutClip(int trackIndex, int clipIndex)
{
    MAIN.timelineDock()->setCurrentTrack(trackIndex);
    MAIN.timelineDock()->setSelection(QList<int>() << clipIndex);
    MAIN.on_actionCut_triggered();
}

void MainController::pasteClip(int trackIndex, int position)
{
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
    MAIN.undoStack()->undo();
}

void MainController::redo()
{
    MAIN.undoStack()->redo();
}

void MainController::exportFile(const QString &path)
{
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
