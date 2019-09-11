/*
 * Copyright (c) 2011-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
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

//#include <PythonQt.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scrubbar.h"
#include "openotherdialog.h"
#include "player.h"

#include "widgets/alsawidget.h"
#include "widgets/colorbarswidget.h"
#include "widgets/colorproducerwidget.h"
#include "widgets/decklinkproducerwidget.h"
#include "widgets/directshowvideowidget.h"
#include "widgets/isingwidget.h"
#include "widgets/jackproducerwidget.h"
#include "widgets/toneproducerwidget.h"
#include "widgets/lissajouswidget.h"
#include "widgets/networkproducerwidget.h"
#include "widgets/noisewidget.h"
#include "widgets/plasmawidget.h"
#include "widgets/pulseaudiowidget.h"
#include "widgets/video4linuxwidget.h"
#include "widgets/x11grabwidget.h"
#include "widgets/avformatproducerwidget.h"
#include "widgets/avformatproducersimplewidget.h"
#include "widgets/imageproducerwidget.h"
#include "widgets/webvfxproducer.h"
#include "docks/encodedock.h"
#include "docks/jobsdock.h"
#include "jobqueue.h"
//#include <playlistdock.h>
#include "glwidget.h"
#include "controllers/filtercontroller.h"
#include "controllers/scopecontroller.h"
#include "docks/filtersdock.h"
#include "dialogs/customprofiledialog.h"
//#include "htmleditor/htmleditor.h"
#include "settings.h"
//#include "leapnetworklistener.h"
#include "database.h"
#include "widgets/gltestwidget.h"
#include "docks/timelinedock.h"
#include "widgets/lumamixtransition.h"
#include "qmltypes/mmqmlutilities.h"
#include <qmlapplication.h>
#include "autosavefile.h"
//#include <commands/playlistcommands.h>
#include "shotcut_mlt_properties.h"
#include "widgets/avfoundationproducerwidget.h"
#include "dialogs/textviewerdialog.h"
#include "widgets/gdigrabwidget.h"
#include "models/audiolevelstask.h"
#include "widgets/trackpropertieswidget.h"
#include "widgets/timelinepropertieswidget.h"
#include "dialogs/unlinkedfilesdialog.h"
#include "widgets/filterwidget.h"
#include "widgets/textmanagerwidget.h"
#include "docks/textlistdock.h"
#include "qmlmetadata.h"
#include "docks/resourcebuttondockwidget.h"
#include "dialogs/registrationdialog.h"
#include "dialogs/registrationtipsdialog.h"
#include "registrationchecker.h"
#include "dialogs/profeaturepromptdialog.h"
#include "dialogs/upgradetopropromptdialog.h"
#include "maincontroller.h"
#include "docks/encodetaskdock.h"
#include "encodetaskqueue.h"
#include "dialogs/invalidprojectdialog.h"
#include "maininterface.h"
#include <resourcedockgenerator_global.h>
#include "containerdock.h"
#include "templateeidtor.h"
#include <util.h>

#include <QtWidgets>
#include <Logger.h>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrentRun>
#include <QMutexLocker>
#include <QQuickItem>
#include <QtNetwork>
#include <QJsonDocument>
#include <QJSEngine>
#include <QQmlEngine>
#include <QQmlContext>

#include "dialogs/videomodesettingsdialog.h"

#include "commands/timelinecommands.h"

#if defined (Q_OS_MAC)
    #include "securitybookmark/transport_security_bookmark.h"
#endif



#if defined(Q_OS_WIN)

#if SHARE_VERSION

#if MOVIEMATOR_PRO
//const QString g_homePage = "http://www.macvideostudio.com/video-editor-free-editing-software.html";
//const QString g_buynowPage = "http://www.macvideostudio.com/purchase/moviemator-video-editor-pro.html";
const QString g_homePage = QObject::tr("http://moviemator.net");
const QString g_buynowPage = QObject::tr("http://moviemator.net/buynow.html");
#else
const QString g_homePage = "http://www.macvideostudio.com/video-editor-free-editing-software.html";
const QString g_buynowPage = "http://www.macvideostudio.com/video-editor-free-editing-software.html";
#endif

#else

#if MOVIEMATOR_PRO
const QString g_homePage = "http://www.macvideostudio.com/video-editor-free-editing-software.html";
const QString g_buynowPage = "http://www.macvideostudio.com/video-editor-free-editing-software.html";
#else
const QString g_homePage = "http://www.macvideostudio.com/video-editor-free-editing-software.html";
const QString g_buynowPage = "http://www.macvideostudio.com/video-editor-free-editing-software.html";
#endif

#endif

#else //mac

#if SHARE_VERSION

#if MOVIEMATOR_PRO
const QString g_homePage = "http://www.macvideostudio.com/mac-video-editor-moviemator-pro.html";
const QString g_buynowPage = "http://www.macvideostudio.com/purchase/buy-video-editor-moviemator-pro.html";

#else
const QString g_homePage = "http://www.macvideostudio.com/moviemator-free-mac-video-editor.html";
const QString g_buynowPage = "http://www.macvideostudio.com/purchase/buy-video-editor-moviemator-pro.html";
#endif

#else

#if MOVIEMATOR_PRO
const QString g_homePage = "http://www.macvideostudio.com/video-editor-moviemator-appstore.html";
const QString g_buynowPage = "http://www.macvideostudio.com/purchase/buy-video-editor-moviemator-pro.html";
#else
const QString g_homePage = "http://www.macvideostudio.com/video-editor-moviemator-appstore.html";
const QString g_buynowPage = "http://www.macvideostudio.com/purchase/buy-video-editor-moviemator-pro.html";
#endif

#endif

#endif

static bool eventDebugCallback(void **data)
{
    QEvent *event = reinterpret_cast<QEvent*>(data[1]);
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        QObject *receiver = reinterpret_cast<QObject*>(data[0]);
        LOG_DEBUG() << event << "->" << receiver;
    }
    else if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
        QObject *receiver = reinterpret_cast<QObject*>(data[0]);
        LOG_DEBUG() << event << "->" << receiver;
    }
    return false;
}

static const int AUTOSAVE_TIMEOUT_MS = 10000;

MainWindow::MainWindow()
    : QMainWindow(nullptr)
    , ui(new Ui::MainWindow)
    , m_isKKeyPressed(false)
//    , m_meltedServerDock(nullptr)
//    , m_meltedPlaylistDock(nullptr)
    , m_keyerGroup(nullptr)
    , m_keyerMenu(nullptr)
    , m_isPlaylistLoaded(false)
//    , m_htmlEditor(0)
    , m_autosaveFile(nullptr)
    , m_exitCode(EXIT_SUCCESS)
    , m_navigationPosition(0)
    , m_mainController(MAINCONTROLLER)
{

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    QLibrary libJack("libjack.so.0");
    if (!libJack.load()) {
        QMessageBox::critical(this, qApp->applicationName(),
            tr("Error: This program requires the JACK 1 library.\n\nPlease install it using your package manager. It may be named libjack0, jack-audio-connection-kit, jack, or similar."));
        ::exit(EXIT_FAILURE);
    } else {
        libJack.unload();
    }
    QLibrary libSDL("libSDL-1.2.so.0");
    if (!libSDL.load()) {
        QMessageBox::critical(this, qApp->applicationName(),
            tr("Error: This program requires the SDL 1.2 library.\n\nPlease install it using your package manager. It may be named libsdl1.2debian, SDL, or similar."));
        ::exit(EXIT_FAILURE);
    } else {
        libSDL.unload();
    }
#endif
     LOG_DEBUG() << "MainWindow 1";
    if (!qgetenv("OBSERVE_FOCUS").isEmpty()) {
        connect(qApp, &QApplication::focusChanged,
                this, &MainWindow::onFocusChanged);
        connect(qApp, &QGuiApplication::focusObjectChanged,
                this, &MainWindow::onFocusObjectChanged);
        connect(qApp, &QGuiApplication::focusWindowChanged,
                this, &MainWindow::onFocusWindowChanged);
    }
    LOG_DEBUG() << "MainWindow 2";
    if (!qgetenv("EVENT_DEBUG").isEmpty())
        QInternal::registerCallback(QInternal::EventNotifyCallback, eventDebugCallback);

    LOG_DEBUG() << "begin";
//#ifndef Q_OS_WIN
//    new GLTestWidget(this);
//#endif
    Database::singleton(this);
    m_autosaveTimer.setSingleShot(true);
    m_autosaveTimer.setInterval(AUTOSAVE_TIMEOUT_MS);
    connect(&m_autosaveTimer, SIGNAL(timeout()), this, SLOT(onAutosaveTimeout()));

    // Initialize all QML types
    MMQmlUtilities::registerCommonTypes();

    LOG_DEBUG() << "setup ui";
    // Create the UI.
    ui->setupUi(this);
    LOG_DEBUG() << "setup ui end";

    configureUI();

    setDockNestingEnabled(true);
    ui->statusBar->hide();
    setDockNestingEnabled(false);

    setMouseTracking(true);


    m_bLeftMouseBtnDown = false;

    LOG_DEBUG() << "Connect UI signals";
    // Connect UI signals.
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openVideo()));
//    connect(ui->actionRemove, SIGNAL(triggered()), this, SLOT(removeVideo()));

    if (ui->actionFullscreen)
        connect(ui->actionFullscreen, SIGNAL(triggered()), this, SLOT(on_actionEnter_Full_Screen_triggered()));
//    connect(ui->mainToolBar, SIGNAL(visibilityChanged(bool)), SLOT(onToolbarVisibilityChanged(bool)));
//    connect(ui->actionETinySoftForum, SIGNAL(triggered()), this, SLOT(onETinySoftForum_triggered()));
    connect(ui->actionEmail, SIGNAL(triggered()), this, SLOT(onEmail_triggered()));

    // Accept drag-n-drop of files.
    this->setAcceptDrops(true);

    LOG_DEBUG() << "Set up Undo Stack";
    // Setup the undo stack.
    m_undoStack = new QUndoStack(this);
    QAction *undoAction = m_undoStack->createUndoAction(this);
    QAction *redoAction = m_undoStack->createRedoAction(this);
    undoAction->setIcon(QIcon(":/icons/light/32x32/edit-undo.png"));
    redoAction->setIcon(QIcon(":/icons/light/32x32/edit-redo.png"));
    undoAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+Z", nullptr));
    redoAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+Z", nullptr));
    ui->menuEdit->insertAction(ui->actionCut, undoAction);
    ui->menuEdit->insertAction(ui->actionCut, redoAction);
    ui->menuEdit->insertSeparator(ui->actionCut);
    ui->actionUndo->setIcon(undoAction->icon());
    ui->actionRedo->setIcon(redoAction->icon());
    ui->actionUndo->setToolTip(undoAction->toolTip());
    ui->actionRedo->setToolTip(redoAction->toolTip());

    connect(m_undoStack, SIGNAL(canUndoChanged(bool)), ui->actionUndo, SLOT(setEnabled(bool)));
    connect(m_undoStack, SIGNAL(canRedoChanged(bool)), ui->actionRedo, SLOT(setEnabled(bool)));


    LOG_DEBUG() << "setStyleSheet(\"QMainWindow::separator {width: 6px; background: '#1D1E1F'}\");";
    setStyleSheet("QMainWindow::separator {width: 6px; background: '#1D1E1F'}");

    LOG_DEBUG() << "customize toolbar";
    this->customizeToolbar();

    // Add the player widget.
    LOG_DEBUG() << "Add the player widget";
    m_player = new Player();

    MLT.videoWidget()->installEventFilter(this);

    //      // Add the player widget.
    //      m_player = new Player;
    //      m_player->setAutoFillBackground(true);

    //      QPalette palette;
    //      palette.setColor(QPalette::Background, QColor(0,0,0));
    //      //palette.setBrush(QPalette::Background, QBrush(QPixmap(":/background.png")));
    //      m_player->setPalette(palette);
    //      MLT.videoWidget()->installEventFilter(this);
    //        rightDockWidget->setWidget(m_player);

     ui->centralWidget->layout()->addWidget(m_player);

     ui->centralWidget->layout()->setContentsMargins(0,0,0,0);


      //    ui->horizontalLayout->addWidget(m_tabForResource);
       //   ui->horizontalLayout->addWidget(m_player);
          connect(this, SIGNAL(producerOpened()), m_player, SLOT(onProducerOpened()));
          connect(m_player, SIGNAL(showStatusMessage(QString)), this, SLOT(showStatusMessage(QString)));
          connect(m_player, SIGNAL(inChanged(int)), this, SLOT(onCutModified()));
          connect(m_player, SIGNAL(outChanged(int)), this, SLOT(onCutModified()));
          connect(m_player, SIGNAL(tabIndexChanged(int)), SLOT(onPlayerTabIndexChanged(int)));
          connect(MLT.videoWidget(), SIGNAL(started()), SLOT(processMultipleFiles()));
          connect(MLT.videoWidget(), SIGNAL(paused()), m_player, SLOT(showPaused()));
          connect(MLT.videoWidget(), SIGNAL(playing()), m_player, SLOT(showPlaying()));

          setupSettingsMenu();

          readPlayerSettings();

          configureVideoWidget();


    // Add the docks.
    LOG_DEBUG() << "Add the docks";
//    m_scopeController = new ScopeController(this, ui->menuView);
    QDockWidget* audioMeterDock = findChild<QDockWidget*>("AudioPeakMeterDock");
    if (audioMeterDock) {
        connect(ui->actionAudioMeter, SIGNAL(triggered()), audioMeterDock->toggleViewAction(), SLOT(trigger()));
    }

   // setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::CustomizeWindowHint);
    //setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint);


////    LOG_DEBUG() << "PlaylistDock";
//    m_playlistDock = new PlaylistDock(this, this);
//    m_playlistDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
//    m_playlistDock->setTitleBarWidget(new QWidget());
//    m_playlistDock->setMinimumSize(520,287);
//    m_playlistDock->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

// //   m_playlistDock->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

//    connect(ui->actionPlaylist, SIGNAL(triggered()), this, SLOT(onPlaylistDockTriggered()));
//    connect(m_playlistDock, SIGNAL(clipOpened(Mlt::Producer*)), this, SLOT(openCut(Mlt::Producer*)));
//    connect(m_playlistDock, SIGNAL(itemActivated(int)), this, SLOT(seekPlaylist(int)));
//    connect(m_playlistDock, SIGNAL(showStatusMessage(QString)), this, SLOT(showStatusMessage(QString)));
//    connect(m_playlistDock->model(), SIGNAL(created()), this, SLOT(onPlaylistCreated()));
//    connect(m_playlistDock->model(), SIGNAL(cleared()), this, SLOT(onPlaylistCleared()));
//    connect(m_playlistDock->model(), SIGNAL(cleared()), this, SLOT(updateAutoSave()));
//    connect(m_playlistDock->model(), SIGNAL(closed()), this, SLOT(onPlaylistClosed()));
//    connect(m_playlistDock->model(), SIGNAL(modified()), this, SLOT(onPlaylistModified()));
//    connect(m_playlistDock->model(), SIGNAL(modified()), this, SLOT(updateAutoSave()));
//    connect(m_playlistDock->model(), SIGNAL(loaded()), this, SLOT(onPlaylistLoaded()));
//    if (!Settings.playerGPU())
//        connect(m_playlistDock->model(), SIGNAL(loaded()), this, SLOT(updateThumbnails()));

//    connect(m_playlistDock, SIGNAL(pushCommand(QUndoCommand *)), this, SLOT(pushCommand(QUndoCommand *)));
//    connect(m_playlistDock, SIGNAL(openVideo()), this, SLOT(openVideo()));
//    connect(m_playlistDock, SIGNAL(setPauseAfterOpen(bool)), this, SLOT(setPauseAfterOpen(bool)));
//    connect(m_playlistDock, SIGNAL(openFiles(const QStringList &)), this, SLOT(openFiles(const QStringList &)));
//    connect(m_playlistDock, SIGNAL(loadProducerWidget(Mlt::Producer* )), this, SLOT(loadProducerWidget(Mlt::Producer* )));
//    connect(m_playlistDock, SIGNAL(propertiesDockTriggered()), this, SLOT(onPropertiesDockTriggered()));
//    //取代Playlist::ClearCommand调用MAIN.open做的一个特殊处理
//    connect(m_playlistDock->model(), SIGNAL(openProducer(Mlt::Producer *)), this, SLOT(open(Mlt::Producer *)));
//    connect(m_playlistDock->model(), SIGNAL(seekPlaylist(int)), this, SLOT(seekPlaylist(int)));


    LOG_DEBUG() << "timelinedock";
    m_timelineDock = new TimelineDock(this);
    m_timelineDock->setExtraQmlContextProperty("mainwindow", this);

    QWidget* titleWidget = new QWidget(this); /* where this a QMainWindow object */
    m_timelineDock->setTitleBarWidget( titleWidget );

    m_timelineDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_timelineDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    addDockWidget(Qt::BottomDockWidgetArea, m_timelineDock);


    connect(ui->actionTimeline, SIGNAL(triggered()), SLOT(onTimelineDockTriggered()));
    connect(m_player, SIGNAL(seeked(int)), m_timelineDock, SLOT(onSeeked(int)));
    connect(m_timelineDock, SIGNAL(seeked(int)), SLOT(seekTimeline(int)));
    connect(m_timelineDock, SIGNAL(clipClicked()), SLOT(onTimelineClipSelected()));
    connect(m_timelineDock, SIGNAL(showStatusMessage(QString)), this, SLOT(showStatusMessage(QString)));
    connect(m_timelineDock->model(), SIGNAL(showStatusMessage(QString)), this, SLOT(showStatusMessage(QString)));
    connect(m_timelineDock->model(), SIGNAL(created()), SLOT(onMultitrackCreated()));
    connect(m_timelineDock->model(), SIGNAL(closed()), SLOT(onMultitrackClosed()));
    //sll：modify放在一个地方建立连接，保证数据操作及更新都在界面更新之前
    connect(m_timelineDock->model(), SIGNAL(modified()), m_timelineDock, SLOT(clearSelectionIfInvalid()));
    connect(m_timelineDock->model(), SIGNAL(modified()), m_timelineDock->model(), SLOT(adjustBackgroundDuration()));
    connect(m_timelineDock->model(), SIGNAL(modified()), SLOT(onMultitrackModified()));
    connect(m_timelineDock->model(), SIGNAL(modified()), SLOT(updateAutoSave()));
    connect(m_timelineDock->model(), SIGNAL(durationChanged()), SLOT(onMultitrackDurationChanged()));
    connect(m_timelineDock, SIGNAL(clipOpened(Mlt::Producer*)), SLOT(openCut(Mlt::Producer*)));
    connect(m_timelineDock->model(), SIGNAL(seeked(int)), SLOT(seekTimeline(int)));
    connect(m_timelineDock, SIGNAL(selected(Mlt::Producer*)), SLOT(loadProducerWidget(Mlt::Producer*)));
    connect(m_timelineDock, SIGNAL(selectionChanged()), SLOT(onTimelineSelectionChanged()));
    connect(m_timelineDock, SIGNAL(clipCopied()), SLOT(onClipCopied()));
//    connect(m_playlistDock, SIGNAL(addAllTimeline(Mlt::Playlist*)), SLOT(onTimelineDockTriggered()));
//    connect(m_playlistDock, SIGNAL(addAllTimeline(Mlt::Playlist*)), SLOT(onAddAllToTimeline(Mlt::Playlist*)));
    connect(m_player, SIGNAL(previousSought()), m_timelineDock, SLOT(seekPreviousEdit()));
    connect(m_player, SIGNAL(nextSought()), m_timelineDock, SLOT(seekNextEdit()));

    //connect(m_timelineDock, SIGNAL(selected(Mlt::Producer*)), SLOT(loadTemplateInfo(Mlt::Producer*)));


    //模板编辑类
    //m_templateEditor = new TemplateEidtor();


    LOG_DEBUG() << "FilterController";
    m_filterController = new FilterController(this);

    connect(m_filterController,SIGNAL(filtersInfoLoaded()),this,SLOT(onFiltersInfoLoaded()));

    m_propertiesVideoFilterDock = new FiltersDock(m_filterController->metadataModel(), m_filterController->attachedModel(),true, this);
    m_propertiesVideoFilterDock->setExtraQmlContextProperty("mainwindow", this);
    m_propertiesVideoFilterDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_propertiesVideoFilterDock->setTitleBarWidget(new QWidget());
    m_propertiesVideoFilterDock->setMinimumSize(520,272);

    m_propertiesAudioFilterDock = new FiltersDock(m_filterController->metadataModel(), m_filterController->attachedModel(),false, this);
    m_propertiesAudioFilterDock->setExtraQmlContextProperty("mainwindow", this);
    m_propertiesAudioFilterDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_propertiesAudioFilterDock->setTitleBarWidget(new QWidget());
    m_propertiesAudioFilterDock->setMinimumSize(520,272);





    connect(m_propertiesVideoFilterDock, SIGNAL(currentFilterRequested(int)), m_filterController, SLOT(setCurrentFilter(int)), Qt::QueuedConnection);
    connect(m_propertiesAudioFilterDock, SIGNAL(currentFilterRequested(int)), m_filterController, SLOT(setCurrentFilter(int)), Qt::QueuedConnection);

    connect(ui->actionFilters, SIGNAL(triggered()), this, SLOT(onFiltersDockTriggered()));
//    connect(m_filterController, SIGNAL(currentFilterChanged(QObject*, QmlMetadata*, int)), m_filtersDock, SLOT(setCurrentFilter(QObject*, QmlMetadata*, int)), Qt::QueuedConnection);
     connect(m_filterController, SIGNAL(currentFilterChanged(QObject*, QmlMetadata*, int)), m_propertiesVideoFilterDock, SLOT(setCurrentFilter(QObject*, QmlMetadata*, int)));
     connect(m_filterController, SIGNAL(currentFilterChanged(QObject*, QmlMetadata*, int)), m_propertiesAudioFilterDock, SLOT(setCurrentFilter(QObject*, QmlMetadata*, int)));
    //MovieMator Pro
//#ifdef MOVIEMATOR_PRO
//     connect(m_filterController, SIGNAL(currentFilterChanged(QObject*, QmlMetadata*, int)), m_timelineDock, SLOT(setCurrentFilter(QObject*, QmlMetadata*, int)), Qt::QueuedConnection);
    connect(m_filterController, SIGNAL(currentFilterChanged(QObject*, QmlMetadata*, int)), m_timelineDock, SLOT(setCurrentFilter(QObject*, QmlMetadata*, int)));
//#endif
    //End
    connect(m_filterController, SIGNAL(currentFilterAboutToChange(int)), m_propertiesVideoFilterDock, SLOT(clearCurrentFilter(int)));
    connect(m_filterController, SIGNAL(currentFilterAboutToChange(int)), m_propertiesAudioFilterDock, SLOT(clearCurrentFilter(int)));
    connect(this, SIGNAL(producerOpened()), m_filterController, SLOT(setProducer()));
    connect(this, SIGNAL(producerOpened()), this, SLOT(onProducerOpened()));

    connect(m_filterController->attachedModel(), SIGNAL(changed()), SLOT(onFilterModelChanged()));
    connect(m_propertiesVideoFilterDock, SIGNAL(filterParameterChanged()), SLOT(onFilterModelChanged()));
    connect(m_propertiesAudioFilterDock, SIGNAL(filterParameterChanged()), SLOT(onFilterModelChanged()));
    connect(m_filterController, SIGNAL(statusChanged(QString)), this, SLOT(showStatusMessage(QString)));
    connect(m_timelineDock, SIGNAL(fadeInChanged(int)), m_propertiesVideoFilterDock, SLOT(setFadeInDuration(int)));
    connect(m_timelineDock, SIGNAL(fadeInChanged(int)), m_propertiesAudioFilterDock, SLOT(setFadeInDuration(int)));
    connect(m_timelineDock, SIGNAL(fadeOutChanged(int)), m_propertiesVideoFilterDock, SLOT(setFadeOutDuration(int)));
    connect(m_timelineDock, SIGNAL(fadeOutChanged(int)), m_propertiesAudioFilterDock, SLOT(setFadeOutDuration(int)));
    connect(m_timelineDock, SIGNAL(selected(Mlt::Producer*)), m_filterController, SLOT(setProducer(Mlt::Producer*)));

    connect(m_timelineDock, SIGNAL(addPositionAndSize()), m_filterController, SLOT(addPositionAndSizeFilter()));

    connect(m_timelineDock, SIGNAL(addRotate()), m_filterController, SLOT(addRotateFilter()));
    connect(m_timelineDock, SIGNAL(addCrop()), m_filterController, SLOT(addCropFilter()));

    connect(m_timelineDock, SIGNAL(addFadeInAudio()), m_filterController, SLOT(addFadeInAudioFilter()));

    connect(m_timelineDock, SIGNAL(addFadeOutAudio()), m_filterController, SLOT(addFadeOutAudioFilter()));

    connect(m_timelineDock, SIGNAL(addFadeInVideo()), m_filterController, SLOT(addFadeInVideoFilter()));

    connect(m_timelineDock, SIGNAL(addFadeOutVideo()), m_filterController, SLOT(addFadeOutVideoFilter()));

    connect(m_timelineDock, SIGNAL(addVolume()), m_filterController, SLOT(addVolumeFilter()));
    connect(m_timelineDock, SIGNAL(addText()), m_filterController, SLOT(addTextFilter()));
    connect(m_timelineDock, SIGNAL(filterAdded()), this, SLOT(onShowPropertiesVideoFilterDock()));
    connect(m_timelineDock, SIGNAL(filterAdded()), this, SLOT(onShowPropertiesAudioFilterDock()));
    connect(m_timelineDock, SIGNAL(showFilterDock()), this, SLOT(onShowPropertiesVideoFilterDock()));
    connect(m_timelineDock, SIGNAL(showFilterDock()), this, SLOT(onShowPropertiesAudioFilterDock()));


//#ifdef MOVIEMATOR_PRO
    connect(m_propertiesVideoFilterDock, SIGNAL(changePosition(int)), m_timelineDock, SLOT(changePostionFromFilter(int)));
    connect(m_propertiesAudioFilterDock, SIGNAL(changePosition(int)), m_timelineDock, SLOT(changePostionFromFilter(int)));

    connect(m_timelineDock,SIGNAL(positionChangedForKeyFrame(int)), m_propertiesVideoFilterDock,SLOT(setCurrentClipFrameFromTimeline(int)));
    connect(m_timelineDock,SIGNAL(positionChangedForKeyFrame(int)), m_propertiesAudioFilterDock,SLOT(setCurrentClipFrameFromTimeline(int)));
    connect(m_timelineDock, SIGNAL(positionChanged()), m_propertiesVideoFilterDock, SLOT(onChangePosition()));
    connect(m_timelineDock, SIGNAL(positionChanged()), m_propertiesAudioFilterDock, SLOT(onChangePosition()));
//    connect(m_timelineDock, SIGNAL(positionChanged()), m_filtersDock, SIGNAL(positionChanged()));
//#endif

    LOG_DEBUG() << "history dock";
    m_historyDock = new QDockWidget(tr("History"), this);
    m_historyDock->hide();
    m_historyDock->setObjectName("historyDock");
    m_historyDock->setWindowIcon(ui->actionHistory->icon());

    m_historyDock->setMinimumWidth(150);

    connect(ui->actionHistory, SIGNAL(triggered()), this, SLOT(onHistoryDockTriggered()));
    QUndoView* undoView = new QUndoView(m_undoStack, m_historyDock);
    undoView->setObjectName("historyView");
    undoView->setAlternatingRowColors(true);
    undoView->setSpacing(2);
    m_historyDock->setWidget(undoView);
    ui->actionUndo->setDisabled(true);
    ui->actionRedo->setDisabled(true);




    LOG_DEBUG() << "EncodeDock";
    m_encodeDock = new EncodeDock();//去掉了this，使EncodeDock成为一个独立的窗口
    m_encodeDock->installEventFilter(this);
    m_encodeDock->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    m_encodeDock->setWindowModality(Qt::WindowModal);
    m_encodeDock->hide();


    connect(this, SIGNAL(producerOpened()), m_encodeDock, SLOT(onProducerOpened()));
    connect(ui->actionEncode, SIGNAL(triggered()), this, SLOT(onEncodeTriggered()));
  //  connect(m_encodeDock->toggleViewAction(), SIGNAL(triggered(bool)), this, SLOT(onEncodeTriggered(bool)));
    connect(m_encodeDock, SIGNAL(captureStateChanged(bool)), m_player, SLOT(onCaptureStateChanged(bool)));
    connect(m_encodeDock, SIGNAL(captureStateChanged(bool)), m_propertiesVideoFilterDock, SLOT(setDisabled(bool)));
    connect(m_encodeDock, SIGNAL(captureStateChanged(bool)), m_propertiesAudioFilterDock, SLOT(setDisabled(bool)));
    connect(m_encodeDock, SIGNAL(captureStateChanged(bool)), ui->actionOpen, SLOT(setDisabled(bool)));
    connect(m_encodeDock, SIGNAL(captureStateChanged(bool)), ui->actionOpenOther, SLOT(setDisabled(bool)));
    connect(m_encodeDock, SIGNAL(captureStateChanged(bool)), ui->actionExit, SLOT(setDisabled(bool)));
    connect(m_encodeDock, SIGNAL(captureStateChanged(bool)), this, SLOT(onCaptureStateChanged(bool)));
    connect(m_encodeDock, SIGNAL(captureStateChanged(bool)), m_historyDock, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(profileChanged()), m_encodeDock, SLOT(onProfileChanged()));
    connect(this, SIGNAL(profileChanged()), SLOT(onProfileChanged()));
//    connect(m_playlistDock->model(), SIGNAL(modified()), m_encodeDock, SLOT(onProducerOpened()));
    connect(m_timelineDock, SIGNAL(clipCopied()), m_encodeDock, SLOT(onProducerOpened()));
    m_encodeDock->onProfileChanged();

    m_jobsDock = new JobsDock();//delete this to make jobs dock independent
    m_jobsDock->installEventFilter(this);
    m_jobsDock->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    m_jobsDock->setWindowModality(Qt::WindowModal);
    connect(&JOBS, SIGNAL(jobAdded()), m_jobsDock, SLOT(show()));
    connect(&JOBS, SIGNAL(jobAdded()), m_jobsDock, SLOT(raise()));

    m_tasksDock = new EncodeTaskDock();
    m_tasksDock->installEventFilter(this);
    m_tasksDock->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    m_tasksDock->setWindowModality(Qt::WindowModal);

    connect(&ENCODETASKS, SIGNAL(taskAdded()), m_tasksDock, SLOT(show()));
    connect(&ENCODETASKS, SIGNAL(taskAdded()), m_tasksDock, SLOT(raise()));



//    m_textlistDock = new TextlistDock(m_filterController->attachedModel(),this);
//    m_textlistDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    // connect video widget signals
    Mlt::GLWidget* videoWidget = static_cast<Mlt::GLWidget*>(&(MLT));
//    connect(videoWidget, SIGNAL(dragStarted()), m_playlistDock, SLOT(onPlayerDragStarted()));
    connect(videoWidget, SIGNAL(seekTo(int)), m_player, SLOT(seek(int)));
    connect(videoWidget, SIGNAL(gpuNotSupported()), this, SLOT(onGpuNotSupported()));
//    connect(videoWidget, SIGNAL(frameDisplayed(const SharedFrame&)), m_scopeController, SIGNAL(newFrame(const SharedFrame&)));
 //   connect(videoWidget, SIGNAL(frameDisplayed(const SharedFrame&)), m_scopeController, SLOT(onFrameDisplayed(const SharedFrame&)));

    //connect(m_filterController, SIGNAL(currentFilterChanged(QObject*, QmlMetadata*, int)), videoWidget, SLOT(setCurrentFilter(QObject*, QmlMetadata*)), Qt::QueuedConnection);
//    connect(m_filterController, SIGNAL(currentFilterChanged(QObject*, QmlMetadata*, int)), this, SLOT(setCurrentFilterForVideoWidget(QObject*, QmlMetadata*)), Qt::QueuedConnection);
     connect(m_filterController, SIGNAL(currentFilterChanged(QObject*, QmlMetadata*, int)), this, SLOT(setCurrentFilterForVideoWidget(QObject*, QmlMetadata*)));
    connect(m_filterController, SIGNAL(currentFilterAboutToChange(int)), videoWidget, SLOT(setBlankScene()));

    readWindowSettings();
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    setFocus();
    setCurrentFile("");

    m_timelineDock->setFocusPolicy(Qt::StrongFocus);


    //初始化资源管理Dock
    initParentDockForResourceDock();
//    addResourceDock(m_filtersDock);
//    addDockWidget(Qt::RightDockWidgetArea, m_filtersDock);
//    m_filtersDock->show();

    initParentDockForPropteriesDock();

    LOG_DEBUG() << "RecentDock";
    m_resourceRecentDock = RDG_CreateRecentDock(&MainInterface::singleton());
    addResourceDock(m_resourceRecentDock, tr("File"), QIcon(":/icons/light/32x32/file.png"), QIcon(":/icons/light/32x32/file-highlight.png"));

    LOG_DEBUG() << "VideoFilterDock";
    m_resourceVideoFilterDock = RDG_CreateVideoFilterDock(&MainInterface::singleton());
    addResourceDock(m_resourceVideoFilterDock, tr("Video Filter"), QIcon(":/icons/light/32x32/video_filter.png"), QIcon(":/icons/light/32x32/video_filter_on.png"));
//    RDG_SetVideoFiltersInfo( m_filterController->getVideoFiltersInfo());

    LOG_DEBUG() << "AudioFilterDock";
    m_resourceAudioFilterDock = RDG_CreateAudioFilterDock(&MainInterface::singleton());
    addResourceDock(m_resourceAudioFilterDock, tr("Audio Filter"), QIcon(":/icons/light/32x32/audio_filter.png"), QIcon(":/icons/light/32x32/audio_filter_on.png"));
//    RDG_SetAudioFiltersInfo(m_filterController->getAudioFiltersInfo());

    LOG_DEBUG() << "TextDock";
    m_resourceTextDock = RDG_CreateTextDock(&MainInterface::singleton());
    addResourceDock(m_resourceTextDock, tr("Text"), QIcon(":/icons/light/32x32/text.png"), QIcon(":/icons/light/32x32/text-highlight.png"));

    LOG_DEBUG() << "StickersDock";
    m_resourceStickerDock = RDG_CreateStickerDock(&MainInterface::singleton());
    addResourceDock(m_resourceStickerDock, tr("Stickers"), QIcon(":/icons/light/32x32/anim-stickers.png"), QIcon(":/icons/light/32x32/anim-stickers-highlight.png"));

    m_propertiesDock = new QDockWidget(tr("Properties"));//, this);
    m_propertiesDock->installEventFilter(this);
    m_propertiesDock->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    m_propertiesDock->setWindowModality(Qt::WindowModal);
    m_propertiesDock->setObjectName("propertiesDock");
    m_propertiesDock->setWindowIcon(ui->actionProperties->icon());
    m_propertiesDock->setMinimumWidth(410);
    m_propertiesDock->setStyleSheet(".QWidget {background-color: rgb(53,53,53)}");
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    m_propertiesDock->setWidget(scroll);
    connect(m_encodeDock, SIGNAL(captureStateChanged(bool)), m_propertiesDock, SLOT(setDisabled(bool)));
    //m_propertiesDock->setVisible(false);

    m_simplePropertiesDock = new QDockWidget(tr("Simple Properties"));
    QScrollArea* scroll1 = new QScrollArea;
    scroll1->setWidgetResizable(true);
    scroll1->setFrameShape(QFrame::NoFrame);
    m_simplePropertiesDock->setWidget(scroll1);
    addPropertiesDock(m_simplePropertiesDock, tr("Properties"), QIcon(":/icons/light/32x32/show-properties.png"), QIcon(":/icons/light/32x32/show-properties-highlight.png"));


    m_propertiesVideoFilterDock->setExtraQmlContextProperty("propertiesContainer", m_propertiesDockContainer);
    addPropertiesDock(m_propertiesVideoFilterDock, tr("Video Filter"), QIcon(":/icons/light/32x32/video_filter.png"), QIcon(":/icons/light/32x32/video_filter_on.png"));
    m_propertiesAudioFilterDock->setExtraQmlContextProperty("propertiesContainer", m_propertiesDockContainer);
    addPropertiesDock(m_propertiesAudioFilterDock, tr("Audio Filter"), QIcon(":/icons/light/32x32/audio_filter.png"), QIcon(":/icons/light/32x32/audio_filter_on.png"));

    //替换模板文件界面Dock
    //m_templateEditorDock = TemplateEditorDock_initModule(&MainInterface::singleton());
    //addPropertiesDock(m_templateEditorDock, tr("Template"), QIcon(":/icons/light/32x32/show-filters.png"), QIcon(":/icons/light/32x32/show-filters-highlight.png"));



    m_registrationTipsDialog = new RegistrationTipsDialog();
    m_registrationDialog = new RegistrationDialog();


    m_proFeaturePromptDialog = new ProFeaturePromptDialog();
    m_proFeaturePromptDialog->setWindowModality(QmlApplication::dialogModality());
    m_upgradeToProPromptDialog = new UpgradeToProPromptDialog();
    m_upgradeToProPromptDialog->setWindowModality(QmlApplication::dialogModality());

#ifdef MOVIEMATOR_PRO
#ifndef SHARE_VERSION
    m_invalidProjectDiaog = new InvalidProjectDialog();
    m_invalidProjectDiaog->setWindowModality(QmlApplication::dialogModality());
#endif
#endif

//    QDesktopWidget *desktop = QApplication::desktop();
//    QRect screen = desktop->screenGeometry();
//    int screenWidth = screen.width();
//    int screenHeight = screen.height();

//    int left = (screenWidth-1147)/2;
//    int top = (screenHeight - 654)/2;
//    setGeometry(left,0,1147,654);


    LOG_DEBUG() << "setWindowTitle";

#if SHARE_VERSION

#if MOVIEMATOR_PRO
    if (Registration.registrationType() == Registration_None)
        setWindowTitle(tr("MovieMator Video Editor Pro (Unregistered)"));
    else if (Registration.registrationType() == Registration_Personal)
        setWindowTitle(tr("MovieMator Video Editor Pro (Personal)"));
    else if (Registration.registrationType() == Registration_Family)
        setWindowTitle(tr("MovieMator Video Editor Pro (Business)"));
    else if (Registration.registrationType() == Registration_Commercial)
        setWindowTitle(tr("MovieMator Video Editor Pro"));
    else
        setWindowTitle(tr("MovieMator Video Editor Pro"));
#else
    setWindowTitle(tr("MovieMator Video Editor"));
#endif

#else
#if MOVIEMATOR_PRO
    setWindowTitle(tr("MovieMator Video Editor Pro"));
#else
    setWindowTitle(tr("MovieMator Video Editor"));
#endif
#endif

//    LOG_DEBUG() << "init pythonqt";
//    m_mainController.initPythonQt();

    LOG_DEBUG() << "end";
}

void MainWindow::configureUI()
{
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
 //   ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
#endif
#ifdef Q_OS_MAC
    // Qt 5 on OS X supports the standard Full Screen window widget.
//    ui->mainToolBar->removeAction(ui->actionFullscreen);
    // OS X has a standard Full Screen shortcut we should use.
    ui->actionEnter_Full_Screen->setShortcut(QKeySequence((Qt::CTRL + Qt::META + Qt::Key_F)));
#endif
#ifdef Q_OS_WIN
    // Fullscreen on Windows is not allowing popups and other app windows to appear.
    delete ui->actionFullscreen;
    ui->actionFullscreen = nullptr;
    delete ui->actionEnter_Full_Screen;
    ui->actionEnter_Full_Screen = nullptr;
#endif


#ifndef SHARE_VERSION
    ui->actionBuy_a_License_Code->setVisible(false);
    ui->actionEnter_License_Code->setVisible(false);
#endif

#ifndef MOVIEMATOR_PRO
    ui->actionBuy_a_License_Code->setVisible(false);
    ui->actionEnter_License_Code->setVisible(false);
#endif

#if STEAM
    ui->actionBuy_a_License_Code->setVisible(false);
    ui->actionEnter_License_Code->setVisible(false);
#endif

#if MOVIEMATOR_PRO
    ui->actionAbout_TVE->setText(tr("About MovieMator Video Editor Pro"));
#else
    ui->actionAbout_TVE->setText(tr("About MovieMator Video Editor"));
#endif

#if defined(Q_OS_WIN)
    ui->actionGet_Total_Video_Converter_Pro->setVisible(false);
    ui->actionGet_Total_Video_Player->setVisible(false);
#endif

}

void MainWindow::onFocusWindowChanged(QWindow *) const
{
    LOG_DEBUG() << "Focuswindow changed";
    LOG_DEBUG() << "Current focusWidget:" << QApplication::focusWidget();
    LOG_DEBUG() << "Current focusObject:" << QApplication::focusObject();
    LOG_DEBUG() << "Current focusWindow:" << QApplication::focusWindow();
}

void MainWindow::onFocusObjectChanged(QObject *) const
{
    LOG_DEBUG() << "Focusobject changed";
    LOG_DEBUG() << "Current focusWidget:" << QApplication::focusWidget();
    LOG_DEBUG() << "Current focusObject:" << QApplication::focusObject();
    LOG_DEBUG() << "Current focusWindow:" << QApplication::focusWindow();
}

void MainWindow::onTimelineClipSelected()
{
    // Synchronize navigation position with timeline selection.
    TimelineDock * t = m_timelineDock;

    Q_ASSERT(t);
    if (!t) {
        return;
    }

    if (t->selection().isEmpty())
        return;

    m_navigationPosition = t->centerOfClip(t->currentTrack(), t->selection().first());

}

void MainWindow::onAddAllToTimeline(Mlt::Playlist* playlist)
{
    // We stop the player because of a bug on Windows that results in some
    // strange memory leak when using Add All To Timeline, more noticeable
    // with (high res?) still image files.
    Q_ASSERT(m_player);
    Q_ASSERT(m_timelineDock);
    if (MLT.isSeekable())
        m_player->pause();
    else
        m_player->stop();
    m_timelineDock->appendFromPlaylist(playlist);
}

MainWindow& MainWindow::singleton()
{
    static MainWindow* instance = new MainWindow;
    return *instance;
}

MainWindow::~MainWindow()
{
    m_autosaveMutex.lock();
    delete m_autosaveFile;
    m_autosaveFile = nullptr;
    m_autosaveMutex.unlock();

//    delete m_htmlEditor;
    delete ui;

    Mlt::Controller::destroy();
}

void MainWindow::setupSettingsMenu()
{
    LOG_DEBUG() << "begin";
    QActionGroup* group = new QActionGroup(this);
    group->addAction(ui->actionOneField);
    group->addAction(ui->actionLinearBlend);
    group->addAction(ui->actionYadifTemporal);
    group->addAction(ui->actionYadifSpatial);
    group = new QActionGroup(this);
    group->addAction(ui->actionNearest);
    group->addAction(ui->actionBilinear);
    group->addAction(ui->actionBicubic);
    group->addAction(ui->actionHyper);
    if (Settings.playerGPU()) {
        group = new QActionGroup(this);
        group->addAction(ui->actionGammaRec709);
        group->addAction(ui->actionGammaSRGB);
    } else {
        delete ui->menuGamma;
    }
    m_profileGroup = new QActionGroup(this);
    m_profileGroup->addAction(ui->actionProfileAutomatic);
    ui->actionProfileAutomatic->setData(QString());
    ui->menuProfile->addAction(addProfile(m_profileGroup, "HD 720p 50 fps", "atsc_720p_50"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "HD 720p 59.94 fps", "atsc_720p_5994"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "HD 720p 60 fps", "atsc_720p_60"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "HD 1080i 25 fps", "atsc_1080i_50"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "HD 1080i 29.97 fps", "atsc_1080i_5994"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "HD 1080p 23.98 fps", "atsc_1080p_2398"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "HD 1080p 24 fps", "atsc_1080p_24"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "HD 1080p 25 fps", "atsc_1080p_25"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "HD 1080p 29.97 fps", "atsc_1080p_2997"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "HD 1080p 30 fps", "atsc_1080p_30"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "SD NTSC", "dv_ntsc"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "SD PAL", "dv_pal"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "UHD 2160p 23.98 fps", "uhd_2160p_2398"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "UHD 2160p 24 fps", "uhd_2160p_24"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "UHD 2160p 25 fps", "uhd_2160p_25"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "UHD 2160p 29.97 fps", "uhd_2160p_2997"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "UHD 2160p 30 fps", "uhd_2160p_30"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "UHD 2160p 50 fps", "uhd_2160p_50"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "UHD 2160p 59.94 fps", "uhd_2160p_5994"));
    ui->menuProfile->addAction(addProfile(m_profileGroup, "UHD 2160p 60 fps", "uhd_2160p_60"));
    QMenu* menu = ui->menuProfile->addMenu(tr("Non-Broadcast"));
    menu->addAction(addProfile(m_profileGroup, "HD 720p 23.98 fps", "atsc_720p_2398"));
    menu->addAction(addProfile(m_profileGroup, "HD 720p 24 fps", "atsc_720p_24"));
    menu->addAction(addProfile(m_profileGroup, "HD 720p 25 fps", "atsc_720p_25"));
    menu->addAction(addProfile(m_profileGroup, "HD 720p 29.97 fps", "atsc_720p_2997"));
    menu->addAction(addProfile(m_profileGroup, "HD 720p 30 fps", "atsc_720p_30"));
    menu->addAction(addProfile(m_profileGroup, "HD 1080i 60 fps", "atsc_1080i_60"));
    menu->addAction(addProfile(m_profileGroup, "HDV 1080i 25 fps", "hdv_1080_50i"));
    menu->addAction(addProfile(m_profileGroup, "HDV 1080i 29.97 fps", "hdv_1080_60i"));
    menu->addAction(addProfile(m_profileGroup, "HDV 1080p 25 fps", "hdv_1080_25p"));
    menu->addAction(addProfile(m_profileGroup, "HDV 1080p 29.97 fps", "hdv_1080_30p"));
    menu->addAction(addProfile(m_profileGroup, tr("DVD Widescreen NTSC"), "dv_ntsc_wide"));
    menu->addAction(addProfile(m_profileGroup, tr("DVD Widescreen PAL"), "dv_pal_wide"));
    menu->addAction(addProfile(m_profileGroup, "640x480 4:3 NTSC", "square_ntsc"));
    menu->addAction(addProfile(m_profileGroup, "768x576 4:3 PAL", "square_pal"));
    menu->addAction(addProfile(m_profileGroup, "854x480 16:9 NTSC", "square_ntsc_wide"));
    menu->addAction(addProfile(m_profileGroup, "1024x576 16:9 PAL", "square_pal_wide"));
    m_customProfileMenu = ui->menuProfile->addMenu(tr("Custom"));
    m_customProfileMenu->addAction(ui->actionAddCustomProfile);
    // Load custom profiles
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
    if (dir.cd("profiles")) {
        QStringList profiles = dir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
        if (profiles.length() > 0)
            m_customProfileMenu->addSeparator();
        foreach (QString name, profiles)
            m_customProfileMenu->addAction(addProfile(m_profileGroup, name, dir.filePath(name)));
    }

    // Add the SDI and HDMI devices to the Settings menu.
    m_externalGroup = new QActionGroup(this);
    ui->actionExternalNone->setData(QString());
    m_externalGroup->addAction(ui->actionExternalNone);

    int n = QApplication::desktop()->screenCount();
    for (int i = 0; n > 1 && i < n; i++) {
        QAction* action = new QAction(tr("Screen %1").arg(i), this);
        action->setCheckable(true);
        action->setData(i);
        m_externalGroup->addAction(action);
    }

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    Mlt::Consumer linsys(MLT.profile(), "sdi");
    if (linsys.is_valid()) {
        QAction* action = new QAction("DVEO VidPort", this);
        action->setCheckable(true);
        action->setData(QString("sdi"));
        m_externalGroup->addAction(action);
    }
#endif

    Mlt::Profile profile;
    Mlt::Consumer decklink(profile, "decklink:");
    if (decklink.is_valid()) {
        decklink.set("list_devices", 1);
        int n = decklink.get_int("devices");
        for (int i = 0; i < n; ++i) {
            QString device(decklink.get(QString("device.%1").arg(i).toLatin1().constData()));
            if (!device.isEmpty()) {
                QAction* action = new QAction(device, this);
                action->setCheckable(true);
                action->setData(QString("decklink:%1").arg(i));
                m_externalGroup->addAction(action);

                if (!m_keyerGroup) {
                    m_keyerGroup = new QActionGroup(this);
                    action = new QAction(tr("Off"), m_keyerGroup);
                    action->setData(QVariant(0));
                    action->setCheckable(true);
                    action = new QAction(tr("Internal"), m_keyerGroup);
                    action->setData(QVariant(1));
                    action->setCheckable(true);
                    action = new QAction(tr("External"), m_keyerGroup);
                    action->setData(QVariant(2));
                    action->setCheckable(true);
                }
            }
        }
    }
    if (m_externalGroup->actions().count() > 1)
    {
        ui->menuExternal->addActions(m_externalGroup->actions());
        ui->menuExternal->setHidden(true);
    }
    else {
        delete ui->menuExternal;
        ui->menuExternal = nullptr;
    }
    if (m_keyerGroup) {
        m_keyerMenu = ui->menuExternal->addMenu(tr("DeckLink Keyer"));
        m_keyerMenu->addActions(m_keyerGroup->actions());
        m_keyerMenu->setDisabled(true);
        connect(m_keyerGroup, SIGNAL(triggered(QAction*)), this, SLOT(onKeyerTriggered(QAction*)));
    }
    connect(m_externalGroup, SIGNAL(triggered(QAction*)), this, SLOT(onExternalTriggered(QAction*)));
    connect(m_profileGroup, SIGNAL(triggered(QAction*)), this, SLOT(onProfileTriggered(QAction*)));

    // Setup the language menu actions
    m_languagesGroup = new QActionGroup(this);
//    QAction* a = new QAction(QLocale::languageToString(QLocale::Catalan), m_languagesGroup);
//    a->setCheckable(true);
//    a->setData("ca");
//    ui->menuLanguage->addActions(m_languagesGroup->actions());

    QAction *a = new QAction(QLocale::languageToString(QLocale::English), m_languagesGroup);
    a->setCheckable(true);
    a->setData("en");
//    ui->menuLanguage->addActions(m_languagesGroup->actions());

    a = new QAction(QLocale::languageToString(QLocale::Chinese), m_languagesGroup);
    a->setCheckable(true);
    a->setData("zh_CN");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
 /*   a = new QAction(QLocale::languageToString(QLocale::Czech), m_languagesGroup);
    a->setCheckable(true);
    a->setData("cs");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::Danish), m_languagesGroup);
    a->setCheckable(true);
    a->setData("da");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::Dutch), m_languagesGroup);
    a->setCheckable(true);
    a->setData("nl");

    a = new QAction(QLocale::languageToString(QLocale::Greek), m_languagesGroup);
    a->setCheckable(true);
    a->setData("el");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::French), m_languagesGroup);
    a->setCheckable(true);
    a->setData("fr");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::German), m_languagesGroup);
    a->setCheckable(true);
    a->setData("de");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::Italian), m_languagesGroup);
    a->setCheckable(true);
    a->setData("it");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::Polish), m_languagesGroup);
    a->setCheckable(true);
    a->setData("pl");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::Portuguese).append(" (Brazil)"), m_languagesGroup);
    a->setCheckable(true);
    a->setData("pt_BR");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::Portuguese).append(" (Portugal)"), m_languagesGroup);
    a->setCheckable(true);
    a->setData("pt_PT");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::Slovak), m_languagesGroup);
    a->setCheckable(true);
    a->setData("sk");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::Spanish), m_languagesGroup);
    a->setCheckable(true);
    a->setData("es");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::Russian), m_languagesGroup);
    a->setCheckable(true);
    a->setData("ru");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
    a = new QAction(QLocale::languageToString(QLocale::Ukrainian), m_languagesGroup);
    a->setCheckable(true);
    a->setData("uk");
    ui->menuLanguage->addActions(m_languagesGroup->actions());
*/
    const QString locale = Settings.language();
    foreach (QAction* action, m_languagesGroup->actions()) {
        if (action->data().toString().startsWith(locale)) {
            action->setChecked(true);
            break;
        }
    }
    connect(m_languagesGroup, SIGNAL(triggered(QAction*)), this, SLOT(onLanguageTriggered(QAction*)));

    // Setup the themes actions
    group = new QActionGroup(this);
    group->addAction(ui->actionSystemTheme);
    group->addAction(ui->actionFusionDark);
    group->addAction(ui->actionFusionLight);
    if (Settings.theme() == "dark")
        ui->actionFusionDark->setChecked(true);
    else if (Settings.theme() == "light")
        ui->actionFusionLight->setChecked(true);
    else
        ui->actionSystemTheme->setChecked(true);

#ifdef Q_OS_WIN
    // On Windows, if there is no JACK or it is not running
    // then Shotcut crashes inside MLT's call to jack_client_open().
    // Therefore, the JACK option for Shotcut is banned on Windows.
    delete ui->actionJack;
    ui->actionJack = nullptr;

    // Setup the display method actions.
    if (!Settings.playerGPU()) {
        group = new QActionGroup(this);
        ui->actionDrawingAutomatic->setData(0);
        group->addAction(ui->actionDrawingAutomatic);
        ui->actionDrawingDirectX->setData(Qt::AA_UseOpenGLES);
        group->addAction(ui->actionDrawingDirectX);
        ui->actionDrawingOpenGL->setData(Qt::AA_UseDesktopOpenGL);
        group->addAction(ui->actionDrawingOpenGL);
        // Software rendering is not currently working.
        delete ui->actionDrawingSoftware;
        // ui->actionDrawingSoftware->setData(Qt::AA_UseSoftwareOpenGL);
        // group->addAction(ui->actionDrawingSoftware);
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(onDrawingMethodTriggered(QAction*)));
        switch (Settings.drawMethod()) {
        case Qt::AA_UseDesktopOpenGL:
            ui->actionDrawingOpenGL->setChecked(true);
            break;
        case Qt::AA_UseOpenGLES:
            delete ui->actionGPU;
            ui->actionGPU = nullptr;
            ui->actionDrawingDirectX->setChecked(true);
            break;
        case Qt::AA_UseSoftwareOpenGL:
            delete ui->actionGPU;
            ui->actionGPU = nullptr;
            ui->actionDrawingSoftware->setChecked(true);
            break;
        default:
            ui->actionDrawingAutomatic->setChecked(true);
            break;
        }
    } else {
        // GPU mode only works with OpenGL.
        delete ui->menuDrawingMethod;
        ui->menuDrawingMethod = nullptr;
    }
#else
    delete ui->menuDrawingMethod;
    ui->menuDrawingMethod = nullptr;
#endif
    //隐藏原profile菜单
    ui->menuProfile->menuAction()->setVisible(false);
    LOG_DEBUG() << "end";
}

QAction* MainWindow::addProfile(QActionGroup* actionGroup, const QString& desc, const QString& name)
{
    Q_ASSERT(actionGroup);
    QAction* action = new QAction(desc, this);
    Q_ASSERT(action);
    action->setCheckable(true);
    action->setData(name);
    actionGroup->addAction(action);
    return action;
}

void MainWindow::open(Mlt::Producer* producer)
{
    Q_ASSERT(producer);
    if (!producer) {
        return;
    }

    if (!producer->is_valid())
        showStatusMessage(tr("Failed to open "));
    else if (producer->get_int("error"))
        showStatusMessage(tr("Failed to open ") + producer->get("resource"));

    bool ok = false;
    int screen = Settings.playerExternal().toInt(&ok);
    if (ok && screen != QApplication::desktop()->screenNumber(this))
        m_player->moveVideoToScreen(screen);

    // no else here because open() will delete the producer if open fails
    if (!MLT.setProducer(producer)) {
        emit producerOpened();
        if (!MLT.profile().is_explicit() || MLT.isMultitrack() || MLT.isPlaylist())
            emit profileChanged();
    }
//    m_player->setFocus();
//    m_playlistDock->setUpdateButtonEnabled(false);

    // Needed on Windows. Upon first file open, window is deactivated, perhaps OpenGL-related.
    activateWindow();
}

bool MainWindow::isCompatibleWithGpuMode(MltXmlChecker& checker)
{
    if (checker.needsGPU() && !Settings.playerGPU()) {
        LOG_INFO() << "file uses GPU but GPU not enabled";
        QMessageBox dialog(QMessageBox::Question,
           qApp->applicationName(),
           tr("The file you opened uses GPU effects, but GPU processing is not enabled.\n"
              "Do you want to enable GPU processing and restart?"),
           QMessageBox::No |
           QMessageBox::Yes,
           this);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
        dialog.setWindowModality(QmlApplication::dialogModality());
        dialog.setDefaultButton(QMessageBox::Yes);
        dialog.setEscapeButton(QMessageBox::No);
        int r = dialog.exec();
        if (r == QMessageBox::Yes) {
            Settings.setPlayerGPU(true);
            m_exitCode = EXIT_RESTART;
            QApplication::closeAllWindows();
        }
        return false;
    }
    return true;
}

bool MainWindow::saveRepairedXmlFile(MltXmlChecker& checker, QString& fileName)
{
    QFileInfo fi(fileName);
    QFile repaired(fileName);//QString("%1/%2 - %3.%4").arg(fi.path())
        //.arg(fi.completeBaseName()).arg(tr("Repaired")).arg(fi.suffix()));
    repaired.resize(0);
    repaired.open(QIODevice::WriteOnly);

    LOG_INFO() << "repaired MLT XML file name" << repaired.fileName();
    QFile temp(checker.tempFileName());
    if (temp.exists() && repaired.exists()) {
        temp.open(QIODevice::ReadOnly);
        QByteArray xml = temp.readAll();
        temp.close();

        qint64 n = repaired.write(xml);
        while (n > 0 && n < xml.size()) {
            qint64 x = repaired.write(xml.right(int(xml.size() - n)));
            if (x > 0)
                n += x;
            else
                n = x;
        }
        repaired.close();
        if (n == xml.size()) {
            fileName = repaired.fileName();
            return true;
        }
    }
    QMessageBox::warning(this, qApp->applicationName(), tr("Repairing the project failed."));
    LOG_WARNING() << "repairing failed";
    return false;
}

bool MainWindow::isXmlRepaired(MltXmlChecker& checker, QString& fileName)
{
    bool result = true;
    if (checker.isCorrected()) {
        LOG_WARNING() << fileName;
        QMessageBox dialog(QMessageBox::Question,
           qApp->applicationName(),
           tr("MovieMator noticed some problems in your project.\n"
              "Do you want MovieMator to try to repair it?\n\n"
              "If you choose Yes, MovieMator will create a copy of your project\n"
              "with \"- Repaired\" in the file name and open it."),
           QMessageBox::No |
           QMessageBox::Yes,
           this);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
        dialog.setWindowModality(QmlApplication::dialogModality());
        dialog.setDefaultButton(QMessageBox::Yes);
        dialog.setEscapeButton(QMessageBox::No);
        int r = dialog.exec();
        if (r == QMessageBox::Yes)
            result = saveRepairedXmlFile(checker, fileName);
    }
    else if (checker.unlinkedFilesModel().rowCount() > 0) {
        UnlinkedFilesDialog dialog(this);
        dialog.setModel(checker.unlinkedFilesModel());
        dialog.setWindowModality(QmlApplication::dialogModality());
        if (dialog.exec() == QDialog::Accepted) {
            if (checker.check(fileName) && checker.isCorrected())
                result = saveRepairedXmlFile(checker, fileName);
        } else {
            result = false;
        }
    }
    return result;
}

bool MainWindow::checkAutoSave(QString &url)
{
    QMutexLocker locker(&m_autosaveMutex);

    // check whether autosave files exist:
    AutoSaveFile* stale = AutoSaveFile::getFile(url);
    if (stale) {
        QMessageBox dialog(QMessageBox::Question, qApp->applicationName(),
           tr("Auto-saved files exist. Do you want to recover them now?"),
           QMessageBox::No | QMessageBox::Yes, nullptr);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
        dialog.setWindowModality(QmlApplication::dialogModality());
        dialog.setDefaultButton(QMessageBox::Yes);
        dialog.setEscapeButton(QMessageBox::No);
        int r = dialog.exec();
        if (r == QMessageBox::Yes) {
            if (!stale->open(QIODevice::ReadWrite)) {
                LOG_WARNING() << "failed to recover autosave file" << url;
                delete stale;
            } else {
                delete m_autosaveFile;
                m_autosaveFile = stale;
                url = stale->fileName();
                return true;
            }
        } else {
            // remove the stale file
            delete stale;
        }
    }

    // create new autosave object
    delete m_autosaveFile;
    m_autosaveFile = new AutoSaveFile(url);

    return false;
}

void MainWindow::stepLeftBySeconds(int sec)
{
    Q_ASSERT(m_player);
    m_player->seek(m_player->position() + sec * qRound(MLT.profile().fps()));
}

void MainWindow::doAutosave()
{
    m_autosaveMutex.lock();
    if (m_autosaveFile) {
        if (m_autosaveFile->isOpen() || m_autosaveFile->open(QIODevice::ReadWrite)) {
            saveXML(m_autosaveFile->fileName(), false /* without relative paths */);
        } else {
            LOG_ERROR() << "failed to open autosave file for writing" << m_autosaveFile->fileName();
        }
    }
    m_autosaveMutex.unlock();
}

void MainWindow::setFullScreen(bool isFullScreen)
{
    if (isFullScreen) {
#ifndef Q_OS_WIN
        showFullScreen();
        ui->actionEnter_Full_Screen->setVisible(false);
        ui->actionFullscreen->setVisible(false);
#endif
    }
}

QString MainWindow::untitledFileName() const
{
    QDir dir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
    if (!dir.exists()) dir.mkpath(dir.path());
    return dir.filePath("__untitled__.mmp");
}

QString MainWindow::getFileHash(const QString& path) const
{
    // This routine is intentionally copied from Kdenlive.
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData;
         // 1 MB = 1 second per 450 files (or faster)
         // 10 MB = 9 seconds per 450 files (or faster)
        if (file.size() > 1000000*2) {
            fileData = file.read(1000000);
            if (file.seek(file.size() - 1000000))
                fileData.append(file.readAll());
        } else {
            fileData = file.readAll();
        }
        file.close();
        return QCryptographicHash::hash(fileData, QCryptographicHash::Md5).toHex();
    }
    return QString();
}


void MainWindow::setProfile(const QString &profile_name)
{
    LOG_DEBUG() << profile_name;
    MLT.setProfile(profile_name);
    emit profileChanged();
}

static void autosaveTask(MainWindow* p)
{
    Q_ASSERT(p);
    LOG_DEBUG() << "running";
    p->doAutosave();
}

void MainWindow::onAutosaveTimeout()
{
    if (isWindowModified())
        QtConcurrent::run(autosaveTask, this);
}

void MainWindow::updateAutoSave()
{
    if (!m_autosaveTimer.isActive())
        m_autosaveTimer.start();
}

static void openFileTask(MainWindow *p, QString url, const Mlt::Properties* properties)
{
    Q_ASSERT(p);
    LOG_DEBUG() << "Open project";
    p->open1(url, properties);
    emit p->hideProgressDialog();
}


void MainWindow::open(QString url, const Mlt::Properties* properties)
{
    LOG_DEBUG() << url;

#if SHARE_VERSION
#else
    if (url.endsWith(".vob", Qt::CaseInsensitive) || url.endsWith(".m4p", Qt::CaseInsensitive))
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

    bool modified = false;
    if ( url.endsWith(".mmp")) {//xjp

        MltXmlChecker checker;
        if (checker.check(url)) {
            if (!isCompatibleWithGpuMode(checker))
                return;
        }
#ifdef MOVIEMATOR_PRO
#ifndef SHARE_VERSION
        if(checker.hasUnbookmarkedFile())
        {
            showInvalidProjectDialog();
            return;
        }
#endif
#endif

        // only check for a modified project when loading a project, not a simple producer
        if (!continueModified())
            return;
        // close existing project
        //if (playlist())
        //    m_playlistDock->model()->close();
        if (multitrack())
            m_timelineDock->model()->close();
        if (!isXmlRepaired(checker, url))
            return;
        modified = checkAutoSave(url);
        // let the new project change the profile
        if (modified || QFile::exists(url)) {
            MLT.profile().set_explicit(false);
            setWindowModified(modified);
        }
    }

    if (!playlist() && !multitrack()) {
        if (!modified && !continueModified())
            return;
        setCurrentFile("");
        setWindowModified(modified);
        MLT.resetURL();
        // Return to automatic video mode if selected.
        if (m_profileGroup->checkedAction()->data().toString().isEmpty())
            MLT.profile().set_explicit(false);
    }

    if ( url.endsWith(".mmp") && (url != untitledFileName()) )
    {

        QProgressDialog progressDialog( QStringLiteral("Loading project..."), "",0,0,this, Qt::Sheet);
        progressDialog.setCancelButton(nullptr);
        progressDialog.resize(progressDialog.size() + QSize(200,20));
        progressDialog.reset();
        connect(this, SIGNAL(hideProgressDialog()), &progressDialog, SLOT(cancel()));

        //progressDialog.exec();
        progressDialog.setModal(false);                        // 指定进度条的模态:true(模态),false(非模态)
        progressDialog.show();
        qApp->processEvents();
        openFileTask(this, url, properties);
        //QFuture<void> fut1 = QtConcurrent::run(openFileTask, this, url, properties);
        //fut1.waitForFinished();
        //showLoadProgress();
    }
//    else if (url.endsWith(".mlt"))
//    {
//        //拷贝到模板目录,并添加到模板管理界面
//        if (!MLT.openXML(url)) {
//            open(MLT.producer());
//            LOG_INFO() << url;
//        }
//        else {
//            showStatusMessage(tr("Failed to open ") + url);
//        }
//    }
    else
    {
        open1(url, properties);
    }
}

void MainWindow::open1(QString url, const Mlt::Properties *properties)
{
    if (!MLT.open(url)) {
        Mlt::Properties* props = const_cast<Mlt::Properties*>(properties);
//        Q_ASSERT(props);
//        Q_ASSERT(props->is_valid());
        if (props && props->is_valid())
            mlt_properties_inherit(MLT.producer()->get_properties(), props->get_properties());
        m_player->setPauseAfterOpen(!MLT.isClip());
        open(MLT.producer());
        if (url.startsWith(AutoSaveFile::path())) {
            if (m_autosaveFile && m_autosaveFile->managedFileName() != untitledFileName()) {
                onFileOpened(m_autosaveFile->managedFileName());
                LOG_INFO() << m_autosaveFile->managedFileName();
            }
        } else {
            onFileOpened(url);
            LOG_INFO() << url;
        }

     //   MAIN.pushCommand(new Playlist::AppendCommand(*(m_playlistDock->model()), MLT.XML()));
    //    MLT.producer()->set(kPlaylistIndexProperty, m_playlistDock->model()->playlist()->count());
    }
    else {
//        showStatusMessage(tr("Failed to open ") + url);
        MLT.setProducer(new Mlt::Producer(*multitrack()));//设置producer，打开文件失败后，还可以获取时间值
        if (url != untitledFileName())
           onOpenFailed(url);
    }
}


//class AppendTask : public QRunnable
//{
//public:
//    AppendTask(PlaylistModel* model, const QStringList& filenames)
//        : QRunnable()
//        , model(model)
//        , filenames(filenames)
//    {
//    }
//    void run()
//    {
//        foreach (QString filename, filenames) {
//            QStringList splitList = filename.split(".");
//            QString lastStr = splitList.last();

//#if SHARE_VERSION
//#else
//            if (filename.endsWith(".vob", Qt::CaseInsensitive) || filename.endsWith(".m4p", Qt::CaseInsensitive))
//            {
////                QMessageBox dialog(QMessageBox::NoIcon,
////                                   qApp->applicationName(),
////                                   tr("For reasons of copyright protection, you can not import vob or m4p files"),
////                                   QMessageBox::Ok,
////                                   this);
////        #if MOVIEMATOR_PRO
////                dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
////        #else
////                dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
////        #endif
////                dialog.setWindowModality(QmlApplication::dialogModality());
////                dialog.setDefaultButton(QMessageBox::Ok);
////                int r = dialog.exec();
//                continue;
//            }
//#endif

//            if(lastStr.compare("mmp") )
//            {
//            Mlt::Producer p(MLT.profile(), filename.toUtf8().constData());
//            if (p.is_valid()) {
//                // Convert avformat to avformat-novalidate so that XML loads faster.
//                if (!qstrcmp(p.get("mlt_service"), "avformat")) {
//                    p.set("mlt_service", "avformat-novalidate");
//                    p.set("mute_on_pause", 0);
//                }
//                MLT.setImageDurationFromDefault(&p);
//                MLT.getHash(p);
//                MAIN.pushCommand(new Playlist::AppendCommand(*model, MLT.XML(&p)));
//            }
//            }
//        }
//        emit MAIN.hideProgressDialog();
//    }
//private:
//    PlaylistModel* model;
//    const QStringList& filenames;
//};



void MainWindow::openVideo()
{

    QString path = Settings.openPath();
#ifdef Q_OS_MAC
    path.append("/*");
#endif
    QString supportedFormats = tr("All supported files(*.mmp *.png *.bmp *.tiff *.jpeg *.gif *.jpg *.3gp *.3gp2 *.3g2 *.3gpp *.a52  *.avi *.asf  *.ac3 *.aac *.au *.aiff *.aif *.ape *.amr *.caf *.cda *.dat *.divx *.dsm *.dv *.dts *.dvr-ms *.flac  *.fli  *.flc *.flv *.f4v *.gif *.ifo *.m4r *.mp2 *.mp3 *.mmf *.mp4 *.mov *.mpg *.mpeg *.m1v *.mpeg1 *.mpeg2 *.m2a *.mp1  *.m2p *.m2v *.mpv *.mpa *.mpeg4*.mod *.m2ts *.mts *.m4v *.m4a *.mxf *.mkv *.mka *.ogg *.ogv*.ogm *.oga *.qt *.rmvb *.rm *.ra *.ram *.tod  *.ts *.vob *.wma *.wav *.wmv *.webm *.wtv)");
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open File"), path, supportedFormats);
//    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open File"), path);
    if (filenames.length() > 0) {

//        m_resourceBtnDock->on_showPlaylistDock_clicked();
//        m_resourceBtnDock->on_showRecentDock_clicked();
        Settings.setOpenPath(QFileInfo(filenames.first()).path());
        activateWindow();
        if (filenames.length() > 1)// 去掉限制添加所有的文件到playlist
            m_multipleFiles = filenames;

        open(filenames.first());

//        //添加所有文件到播放器列表
//        PlaylistModel* model = m_playlistDock->model();
//        QThreadPool::globalInstance()->start(new AppendTask(model, m_multipleFiles));
    }
    else {
        // If file invalid, then on some platforms the dialog messes up SDL.
        MLT.onWindowResize();
        activateWindow();
    }
}

void MainWindow::openFiles(const QStringList &list)
{
    if (list.length() > 1)
        m_multipleFiles = list;
    open(list.first());
//    //添加所有文件到播放器列表
//    PlaylistModel* model = m_playlistDock->model();
//    QThreadPool::globalInstance()->start(new AppendTask(model, m_multipleFiles));
}

void MainWindow::openCut(Mlt::Producer* producer)
{
    Q_ASSERT(producer);
    //播放列表双击播放文件和右键播放，需要把这一句屏蔽
//    m_player->setPauseAfterOpen(true);
    open(producer);
    MLT.seek(producer->get_in());
}

void MainWindow::showStatusMessage(QAction* action, int timeoutSeconds)
{
    Q_ASSERT(action);
    Q_ASSERT(m_player);
    // This object takes ownership of the passed action.
    // This version does not currently log its message.
    m_statusBarAction.reset(action);
    action->setParent(nullptr);
    m_player->setStatusLabel(action->text(), timeoutSeconds, action);
}

void MainWindow::removeVideo()
{
//   m_textManagerWidget->addTextToTimeline(NULL);
//   m_textlistDock->addTextToTimeline(NULL);
//   m_playlistDock->on_removeButton_clicked();
//    RecentDock_removeSelectedItem();
}



void MainWindow::showStatusMessage(const QString& message, int timeoutSeconds)
{
    LOG_INFO() << message;
    m_player->setStatusLabel(message, timeoutSeconds, nullptr /* QAction */);
    m_statusBarAction.reset();
}

void MainWindow::seekPlaylist(int start)
{
    LOG_DEBUG() << "begin";
    if (!playlist()) return;
    // we bypass this->open() to prevent sending producerOpened signal to self, which causes to reload playlist
    if (!MLT.producer() || static_cast<void*>(MLT.producer()->get_producer()) != static_cast<void*>(playlist()->get_playlist()))
        MLT.setProducer(new Mlt::Producer(*playlist()));
    m_player->setIn(-1);
    m_player->setOut(-1);
    // since we do not emit producerOpened, these components need updating
    on_actionJack_triggered(ui->actionJack && ui->actionJack->isChecked());
    m_player->onProducerOpened(false);
    m_encodeDock->onProducerOpened();
    m_filterController->setProducer();
    updateMarkers();
    MLT.seek(start);
    m_player->setFocus();

    m_player->switchToTab(Player::ProjectTabIndex);
    m_player->togglePlayPaused();
}

void MainWindow::seekTimeline(int position)
{
    LOG_DEBUG() << "begin";
    if (!multitrack()) return;
    setMultitrackAsCurrentProducer();

    if (MLT.producer())
    {
        m_player->seek(position);
    }

}

void MainWindow::setMultitrackAsCurrentProducer()
{
    if (!multitrack()) return;
    // we bypass this->open() to prevent sending producerOpened signal to self, which causes to reload playlist
    if (MLT.producer() && static_cast<void*>(MLT.producer()->get_producer()) != static_cast<void*>(multitrack()->get_producer())) {
        MLT.setProducer(new Mlt::Producer(*multitrack()));
        m_player->setIn(-1);
        m_player->setOut(-1);
        // since we do not emit producerOpened, these components need updating
        m_player->onProducerOpened(false);
        m_encodeDock->onProducerOpened();
        m_filterController->setProducer();
        updateMarkers();
        m_player->setFocus();
        m_player->switchToTab(Player::ProjectTabIndex);
        m_timelineDock->emitSelectedFromSelection();
    }
}

void MainWindow::readPlayerSettings()
{
    LOG_DEBUG() << "begin";
    ui->actionRealtime->setChecked(Settings.playerRealtime());
    ui->actionProgressive->setChecked(Settings.playerProgressive());
    ui->actionScrubAudio->setChecked(Settings.playerScrubAudio());
    if (ui->actionJack)
        ui->actionJack->setChecked(Settings.playerJACK());
    if (ui->actionGPU) {
        ui->actionGPU->setChecked(Settings.playerGPU());
        MLT.videoWidget()->setProperty("gpu", ui->actionGPU->isChecked());
    }
    QString deinterlacer = Settings.playerDeinterlacer();
    QString interpolation = Settings.playerInterpolation();

    if (deinterlacer == "onefield")
        ui->actionOneField->setChecked(true);
    else if (deinterlacer == "linearblend")
        ui->actionLinearBlend->setChecked(true);
    else if (deinterlacer == "yadif-nospatial")
        ui->actionYadifTemporal->setChecked(true);
    else
        ui->actionYadifSpatial->setChecked(true);

    if (interpolation == "nearest")
        ui->actionNearest->setChecked(true);
    else if (interpolation == "bilinear")
        ui->actionBilinear->setChecked(true);
    else if (interpolation == "bicubic")
        ui->actionBicubic->setChecked(true);
    else
        ui->actionHyper->setChecked(true);

    QString external = Settings.playerExternal();
    bool ok = false;
    external.toInt(&ok);
    foreach (QAction* a, m_externalGroup->actions()) {
        if (a->data() == external) {
            a->setChecked(true);
            if (a->data().toString().startsWith("decklink") && m_keyerMenu)
                m_keyerMenu->setEnabled(true);
            break;
        }
    }

    if (m_keyerGroup) {
        int keyer = Settings.playerKeyerMode();
        foreach (QAction* a, m_keyerGroup->actions()) {
            if (a->data() == keyer) {
                a->setChecked(true);
                break;
            }
        }
    }

    QString profile = Settings.playerProfile();
    // Automatic not permitted for SDI/HDMI
    if (!external.isEmpty() && !ok && profile.isEmpty())
        profile = "atsc_720p_50";
    foreach (QAction* a, m_profileGroup->actions()) {
        // Automatic not permitted for SDI/HDMI
        if (a->data().toString().isEmpty() && !external.isEmpty() && !ok)
            a->setDisabled(true);
        if (a->data().toString() == profile) {
            a->setChecked(true);
            break;
        }
    }

    QString gamma = Settings.playerGamma();
    if (gamma == "bt709")
        ui->actionGammaRec709->setChecked(true);
    else
        ui->actionGammaSRGB->setChecked(true);

    LOG_DEBUG() << "end";
}

void MainWindow::readWindowSettings()
{
    LOG_DEBUG() << "begin";
    Settings.setWindowGeometryDefault(saveGeometry());
    Settings.setWindowStateDefault(saveState());
    Settings.sync();
    restoreGeometry(Settings.windowGeometry());
    restoreState(Settings.windowState()); //不读取上一次的toolbar及dockwidget状态
    LOG_DEBUG() << "end";
}

void MainWindow::writeSettings()
{
#ifndef Q_OS_MAC
    if (isFullScreen())
        showNormal();
#endif
    Settings.setWindowGeometry(saveGeometry());
    Settings.setWindowState(saveState());//不保存关闭时的toolbar及dockwidget状态，使窗口始终处于初始状态
    Settings.sync();
}

void MainWindow::configureVideoWidget()
{
    LOG_DEBUG() << "begin";
    setProfile(m_profileGroup->checkedAction()->data().toString());
    MLT.videoWidget()->setProperty("realtime", ui->actionRealtime->isChecked());
    bool ok = false;
    m_externalGroup->checkedAction()->data().toInt(&ok);
    if (!ui->menuExternal || m_externalGroup->checkedAction()->data().toString().isEmpty() || ok) {
        MLT.videoWidget()->setProperty("progressive", ui->actionProgressive->isChecked());
    } else {
        MLT.videoWidget()->setProperty("mlt_service", m_externalGroup->checkedAction()->data());
        MLT.videoWidget()->setProperty("progressive", MLT.profile().progressive());
        ui->actionProgressive->setEnabled(false);
    }
    if (ui->actionOneField->isChecked())
        MLT.videoWidget()->setProperty("deinterlace_method", "onefield");
    else if (ui->actionLinearBlend->isChecked())
        MLT.videoWidget()->setProperty("deinterlace_method", "linearblend");
    else if (ui->actionYadifTemporal->isChecked())
        MLT.videoWidget()->setProperty("deinterlace_method", "yadif-nospatial");
    else
        MLT.videoWidget()->setProperty("deinterlace_method", "yadif");
    if (ui->actionNearest->isChecked())
        MLT.videoWidget()->setProperty("rescale", "nearest");
    else if (ui->actionBilinear->isChecked())
        MLT.videoWidget()->setProperty("rescale", "bilinear");
    else if (ui->actionBicubic->isChecked())
        MLT.videoWidget()->setProperty("rescale", "bicubic");
    else
        MLT.videoWidget()->setProperty("rescale", "hyper");
    if (m_keyerGroup)
        MLT.videoWidget()->setProperty("keyer", m_keyerGroup->checkedAction()->data());
    LOG_DEBUG() << "end";
}

void MainWindow::setCurrentFile(const QString &filename)
{
    QString shownName = "Untitled";
    if (filename == untitledFileName())
        m_currentFile.clear();
    else
        m_currentFile = filename;
    if (!m_currentFile.isEmpty())
        shownName = QFileInfo(m_currentFile).fileName();
#if SHARE_VERSION
#if MOVIEMATOR_PRO
    if (Registration.registrationType() == Registration_None)
        setWindowTitle(tr("%1[*] - %2").arg(shownName).arg("MovieMator Video Editor Pro (Unregistered)"));
    else if (Registration.registrationType() == Registration_Personal)
        setWindowTitle(tr("%1[*] - %2").arg(shownName).arg("MovieMator Video Editor Pro (Personal)"));
    else if (Registration.registrationType() == Registration_Family)
        setWindowTitle(tr("%1[*] - %2").arg(shownName).arg("MovieMator Video Editor Pro (Business)"));
    else if (Registration.registrationType() == Registration_Commercial)
        setWindowTitle(tr("%1[*] - %2").arg(shownName).arg("MovieMator Video Editor Pro"));
    else
        setWindowTitle(tr("%1[*] - %2").arg(shownName).arg("MovieMator Video Editor Pro"));
#else
    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg("MovieMator Video Editor"));
#endif

#else
#if MOVIEMATOR_PRO
    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg("MovieMator Video Editor Pro"));
#else
    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("MovieMator Video Editor")));
#endif
#endif
}

//void MainWindow::on_actionAbout_Shotcut_triggered()
//{
//    QMessageBox::about(this, tr("About Shotcut"),
//             tr("<h1>Shotcut version %1</h1>"
//                "<p><a href=\"https://www.shotcut.org/\">Shotcut</a> is a free, open source, cross platform video editor.</p>"
//                "<small><p>Copyright &copy; 2011-2016 <a href=\"https://www.meltytech.com/\">Meltytech</a>, LLC</p>"
//                "<p>Licensed under the <a href=\"http://www.gnu.org/licenses/gpl.html\">GNU General Public License v3.0</a></p>"
//                "<p>This program proudly uses the following projects:<ul>"
//                "<li><a href=\"http://www.qt-project.org/\">Qt</a> application and UI framework</li>"
//                "<li><a href=\"http://www.mltframework.org/\">MLT</a> multimedia authoring framework</li>"
//                "<li><a href=\"http://www.ffmpeg.org/\">FFmpeg</a> multimedia format and codec libraries</li>"
//                "<li><a href=\"http://www.videolan.org/developers/x264.html\">x264</a> H.264 encoder</li>"
//                "<li><a href=\"http://www.webmproject.org/\">WebM</a> VP8 and VP9 encoders</li>"
//                "<li><a href=\"http://lame.sourceforge.net/\">LAME</a> MP3 encoder</li>"
//                "<li><a href=\"http://www.dyne.org/software/frei0r/\">Frei0r</a> video plugins</li>"
//                "<li><a href=\"http://www.ladspa.org/\">LADSPA</a> audio plugins</li>"
//                "<li><a href=\"http://www.defaulticon.com/\">DefaultIcon</a> icon collection by <a href=\"http://www.interactivemania.com/\">interactivemania</a></li>"
//                "<li><a href=\"http://www.oxygen-icons.org/\">Oxygen</a> icon collection</li>"
//                "</ul></p>"
//                "<p>The source code used to build this program can be downloaded from "
//                "<a href=\"https://www.shotcut.org/\">shotcut.org</a>.</p>"
//                "This program is distributed in the hope that it will be useful, "
//                "but WITHOUT ANY WARRANTY; without even the implied warranty of "
//                "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.</small>"
//                ).arg(qApp->applicationVersion()));
//}

void MainWindow::on_actionAbout_TVE_triggered()
{
#if SHARE_VERSION

#if MOVIEMATOR_PRO
    QMessageBox::about(this, tr("About MovieMator Video Editor Pro"),
             tr("<h1>MovieMator Video Editor Pro %1</h1>"
                "<small><p>Product Home Page: <a href=%2>MovieMator Video Editor Pro</a></p>"
                "<p />"
                "<p>Copyright &copy; 2016-2019 effectmatrix, Inc</p>"
                "<p>Based on Shotcut v16.06 Copyright &copy; 2011-2016 Meltytech, LLC.</p>"
                "<p>Licensed under the GNU General Public License v3.0</p>"
                "</small>"
                "<small>"
                "<p />"
                "<p><a href=\"http://www.moviemator.net/develop/third-party.html\">Related Information</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:market@effectmatrix.com\">Feedback</a></p>"
                "</small>"
                ).arg(qApp->applicationVersion()).arg(g_homePage));
#else
    QMessageBox::about(this, tr("About MovieMator Video Editor"),
             tr("<h1>MovieMator Video Editor %1</h1>"
                "<small><p>Product Home Page: <a href=%2>MovieMator Video Editor</a></p>"
                "<p />"
                "<p>Copyright &copy; 2016-2019 effectmatrix, Inc</p>"
                "<p>Based on Shotcut v16.06 Copyright &copy; 2011-2016 Meltytech, LLC.</p>"
                "<p>Licensed under the GNU General Public License v3.0</p>"
                "</small>"
                "<small>"
                "<p />"
                "<p><a href=\"http://www.moviemator.net/develop/third-party.html\">Related Information</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:market@effectmatrix.com\">Feedback</a></p>"
                "</small>"
                ).arg(qApp->applicationVersion()).arg(g_homePage));
#endif

#else
#if MOVIEMATOR_PRO
    QMessageBox::about(this, tr("About MovieMator Video Editor Pro"),
             tr("<h1>MovieMator Video Editor Pro %1</h1>"
                "<small><p>Product Home Page: <a href=%2>MovieMator Video Editor Pro</a></p>"
                "<p />"
                "<p>Copyright &copy; 2016-2019 effectmatrix, Inc</p>"
                "<p>Based on Shotcut v16.06 Copyright &copy; 2011-2016 Meltytech, LLC.</p>"
                "<p>Licensed under the GNU General Public License v3.0</p>"
                "</small>"
                "<small>"
                "<p />"
                "<p><a href=\"http://www.moviemator.net/develop/third-party.html\">Related Information</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:market@effectmatrix.com\">Feedback</a></p>"
                "</small>"
                ).arg(qApp->applicationVersion()).arg(g_homePage));
#else
    QMessageBox::about(this, tr("About MovieMator Video Editor"),
             tr("<h1>MovieMator Video Editor %1</h1>"
                "<small><p>Product Home Page: <a href=%2>MovieMator Video Editor</a></p>"
                "<p />"
                "<p>Copyright &copy; 2016-2019 effectmatrix, Inc</p>"
                "<p>Based on Shotcut v16.06 Copyright &copy; 2011-2016 Meltytech, LLC.</p>"
                "<p>Licensed under the GNU General Public License v3.0</p>"
                "</small>"
                "<small>"
                "<p />"
                "<p><a href=\"http://www.moviemator.net/develop/third-party.html\">Related Information</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:market@effectmatrix.com\">Feedback</a></p>"
                "</small>"
                ).arg(qApp->applicationVersion()).arg(g_homePage));
#endif

#endif
}


void MainWindow::keyPressEvent(QKeyEvent* event)
{
    Q_ASSERT(event);

    bool handled = true;


    switch (event->key()) {
    case Qt::Key_Home:
        m_player->seek(0);
        break;
    case Qt::Key_End:
        if (MLT.producer())
            m_player->seek(MLT.producer()->get_length() - 1);
        break;
    case Qt::Key_Left:
        if ((event->modifiers() & Qt::ControlModifier) && m_timelineDock->isVisible()) {
            if (m_timelineDock->selection().isEmpty()) {
                m_timelineDock->selectClipUnderPlayhead();
            } else if (m_timelineDock->selection().size() == 1) {
                int newIndex = m_timelineDock->selection().first() - 1;
                if (newIndex < 0)
                    break;
                m_timelineDock->setSelection(QList<int>() << newIndex, m_timelineDock->currentTrack());
                m_navigationPosition = m_timelineDock->centerOfClip(m_timelineDock->currentTrack(), newIndex);
            }
        } else {
            stepLeftOneFrame();
        }
        break;
    case Qt::Key_Right:
        if ((event->modifiers() & Qt::ControlModifier) && m_timelineDock->isVisible()) {
            if (m_timelineDock->selection().isEmpty()) {
                m_timelineDock->selectClipUnderPlayhead();
            } else if (m_timelineDock->selection().size() == 1) {
                int newIndex = m_timelineDock->selection().first() + 1;
                if (newIndex >= m_timelineDock->clipCount(-1))
                    break;
                m_timelineDock->setSelection(QList<int>() << newIndex, m_timelineDock->currentTrack());
                m_navigationPosition = m_timelineDock->centerOfClip(m_timelineDock->currentTrack(), newIndex);
            }
        } else {
            stepRightOneFrame();
        }
        break;
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
        {
            int directionMultiplier = event->key() == Qt::Key_PageUp ? -1 : 1;
            int seconds = 1;
            if (event->modifiers() & Qt::ControlModifier)
                seconds *= 5;
            if (event->modifiers() & Qt::ShiftModifier)
                seconds *= 2;
            stepLeftBySeconds(seconds * directionMultiplier);
        }
        break;
    case Qt::Key_Space:
#ifdef Q_OS_MAC
        // Spotlight defaults to Cmd+Space, so also accept Ctrl+Space.
        if ((event->modifiers() == Qt::MetaModifier || (event->modifiers() & Qt::ControlModifier)) && m_timelineDock->isVisible())
#else
        if (event->modifiers() == Qt::ControlModifier && m_timelineDock->isVisible())
#endif
            m_timelineDock->selectClipUnderPlayhead();
        else
            handled = false;
        break;
    case Qt::Key_A:
        if (event->modifiers() == Qt::ShiftModifier) {
//            m_playlistDock->show();
//            m_playlistDock->raise();
//            m_playlistDock->on_actionAppendCut_triggered();
        } else {
            m_timelineDock->show();
            m_timelineDock->raise();
            m_timelineDock->append(-1);
        }
        break;
    case Qt::Key_C:
        if (event->modifiers() == Qt::ShiftModifier) {
//            m_playlistDock->show();
//            m_playlistDock->raise();
//            if (m_playlistDock->position() >= 0)
//                m_playlistDock->on_actionOpen_triggered();
        } else {
            m_timelineDock->show();
            m_timelineDock->raise();
            if (!m_timelineDock->selection().isEmpty())
                m_timelineDock->copyClip(m_timelineDock->currentTrack(), m_timelineDock->selection().first());
        }
        break;
    case Qt::Key_D:
        if (event->modifiers() & Qt::ControlModifier)
            m_timelineDock->setSelection(QList<int>(), m_timelineDock->currentTrack());
        else
            handled = false;
        break;
    case Qt::Key_H:
#ifdef Q_OS_MAC
        // OS X uses Cmd+H to hide an app.
        if (event->modifiers() & Qt::MetaModifier && multitrack())
#else
        if (event->modifiers() & Qt::ControlModifier && multitrack())
#endif
            m_timelineDock->toggleTrackHidden(m_timelineDock->currentTrack());
        break;
    case Qt::Key_J:
        if (m_isKKeyPressed)
            m_player->seek(m_player->position() - 1);
        else
            m_player->rewind();
        break;
    case Qt::Key_K:
            m_player->pause();
            m_isKKeyPressed = true;
        break;
    case Qt::Key_L:
#ifdef Q_OS_MAC
        // OS X uses Cmd+H to hide an app and Cmd+M to minimize. Therefore, we force
        // it to be the apple keyboard control key aka meta. Therefore, to be
        // consistent with all track header toggles, we make the lock toggle also use
        // meta.
        if (event->modifiers() & Qt::MetaModifier && multitrack())
#else
        if (event->modifiers() & Qt::ControlModifier && multitrack())
#endif
            m_timelineDock->setTrackLock(m_timelineDock->currentTrack(), !m_timelineDock->isTrackLocked(m_timelineDock->currentTrack()));
        else if (m_isKKeyPressed)
            m_player->seek(m_player->position() + 1);
        else
            m_player->fastForward();
        break;
    case Qt::Key_M:
#ifdef Q_OS_MAC
        // OS X uses Cmd+M to minimize an app.
        if (event->modifiers() & Qt::MetaModifier && multitrack())
#else
        if (event->modifiers() & Qt::ControlModifier && multitrack())
#endif
            m_timelineDock->toggleTrackMute(m_timelineDock->currentTrack());
        break;
    case Qt::Key_I:
        setInToCurrent(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_O:
        setOutToCurrent(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_S:
        if (multitrack())
            m_timelineDock->splitClip();
        break;
    case Qt::Key_U:
        if (event->modifiers() == Qt::ControlModifier) {
            m_timelineDock->show();
            m_timelineDock->raise();
            m_timelineDock->addAudioTrack();
        }
        break;
    case Qt::Key_V: // Avid Splice In
        if (event->modifiers() == Qt::ShiftModifier) {
//            m_playlistDock->show();
//            m_playlistDock->raise();
//            m_playlistDock->on_actionInsertCut_triggered();
        } else {
            m_timelineDock->show();
            m_timelineDock->raise();
//            m_timelineDock->insert(-1);
            m_timelineDock->paste();    // 粘贴
        }
        break;
    case Qt::Key_B: // Avid Overwrite
        if (event->modifiers() == Qt::ShiftModifier) {
//            m_playlistDock->show();
//            m_playlistDock->raise();
//            m_playlistDock->on_actionUpdate_triggered();
        } else {
            m_timelineDock->show();
            m_timelineDock->raise();
            m_timelineDock->overwrite(-1);
        }
        break;
    case Qt::Key_Escape: // Avid Toggle Active Monitor
        if (MLT.isPlaylist()) {
            if (multitrack())
                m_player->onTabBarClicked(Player::ProjectTabIndex);
            else if (MLT.savedProducer())
                m_player->onTabBarClicked(Player::SourceTabIndex);
//            else
//                m_playlistDock->on_actionOpen_triggered();
        } else if (MLT.isMultitrack()) {
            if (MLT.savedProducer())
                m_player->onTabBarClicked(Player::SourceTabIndex);
            // TODO else open clip under playhead of current track if available
        } else {
            if (multitrack() || (playlist() && playlist()->count() > 0))
                m_player->onTabBarClicked(Player::ProjectTabIndex);
        }
        break;
    case Qt::Key_Up:
/*        if (m_playlistDock->isVisible() && event->modifiers() & Qt::AltModifier) {
            m_playlistDock->raise();
            m_playlistDock->decrementIndex();
            m_playlistDock->on_actionOpen_triggered();
        } else */if (multitrack()) {
            int newClipIndex = -1;
            if ((event->modifiers() & Qt::ControlModifier) &&
                    !m_timelineDock->selection().isEmpty() &&
                    m_timelineDock->currentTrack() > 0) {

                newClipIndex = m_timelineDock->clipIndexAtPosition(m_timelineDock->currentTrack() - 1, m_navigationPosition);
            }

            m_timelineDock->selectTrack(-1);

            if (newClipIndex >= 0) {
                newClipIndex = qMin(newClipIndex, m_timelineDock->clipCount(m_timelineDock->currentTrack()) - 1);
                m_timelineDock->setSelection(QList<int>() << newClipIndex, m_timelineDock->currentTrack() - 1);
            }

        } /*else if (m_playlistDock->isVisible()) {
            m_playlistDock->raise();
            if (event->modifiers() & Qt::ControlModifier)
                m_playlistDock->moveClipUp();
            m_playlistDock->decrementIndex();
        }*/
        break;
    case Qt::Key_Down:
        /*if (m_playlistDock->isVisible() && event->modifiers() & Qt::AltModifier) {
            m_playlistDock->raise();
            m_playlistDock->incrementIndex();
            m_playlistDock->on_actionOpen_triggered();
        } else*/ if (multitrack()) {
            int newClipIndex = -1;
            if ((event->modifiers() & Qt::ControlModifier) &&
                    !m_timelineDock->selection().isEmpty() &&
                    m_timelineDock->currentTrack() < m_timelineDock->model()->trackList().count() - 1) {

                newClipIndex = m_timelineDock->clipIndexAtPosition(m_timelineDock->currentTrack() + 1, m_navigationPosition);
            }

            m_timelineDock->selectTrack(1);

            if (newClipIndex >= 0) {
                newClipIndex = qMin(newClipIndex, m_timelineDock->clipCount(m_timelineDock->currentTrack()) - 1);
                m_timelineDock->setSelection(QList<int>() << newClipIndex, m_timelineDock->currentTrack() - 1);
            }

        } /*else if (m_playlistDock->isVisible()) {
            m_playlistDock->raise();
            if (event->modifiers() & Qt::ControlModifier)
                m_playlistDock->moveClipDown();
            m_playlistDock->incrementIndex();
        }*/
        break;
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
//        if (m_playlistDock->isVisible()) {
//            m_playlistDock->raise();
//            m_playlistDock->setIndex(event->key() - Qt::Key_1);
//        }
        break;
    case Qt::Key_0:
/*        if (m_playlistDock->isVisible()) {
            m_playlistDock->raise();
            m_playlistDock->setIndex(9);
        }
        else */if (m_timelineDock->isVisible()) {
            m_timelineDock->resetZoom();
        }
        break;
    case Qt::Key_X: // Avid Extract
        if (event->modifiers() == Qt::ShiftModifier) {
//            m_playlistDock->show();
//            m_playlistDock->raise();
//            m_playlistDock->on_removeButton_clicked();
        } else {
            m_timelineDock->show();
            m_timelineDock->raise();
            m_timelineDock->removeSelection();
        }
        break;
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
        if (multitrack()) {
            m_timelineDock->show();
            m_timelineDock->raise();
            if (event->modifiers() == Qt::ShiftModifier)
                m_timelineDock->removeSelection();
            else
                m_timelineDock->liftSelection();
        } else {
//            m_playlistDock->show();
//            m_playlistDock->raise();
//            m_playlistDock->on_removeButton_clicked();
        }
        break;
    case Qt::Key_Y:
        if (event->modifiers() == Qt::ControlModifier) {
            m_timelineDock->show();
            m_timelineDock->raise();
            m_timelineDock->addVideoTrack();
        }
        break;
    case Qt::Key_Z: // Avid Lift
        if (event->modifiers() == Qt::ShiftModifier) {
//            m_playlistDock->show();
//            m_playlistDock->raise();
//            m_playlistDock->on_removeButton_clicked();
        } else if (multitrack() && event->modifiers() == Qt::NoModifier) {
            m_timelineDock->show();
            m_timelineDock->raise();
            m_timelineDock->liftSelection();
        }
        break;
    case Qt::Key_Minus:
        if (m_timelineDock->isVisible()) {
            if (event->modifiers() & Qt::ControlModifier)
                ;//m_timelineDock->makeTracksShorter();
            else
                m_timelineDock->zoomOut();
        }
        break;
    case Qt::Key_Equal:
    case Qt::Key_Plus:
        if (m_timelineDock->isVisible()) {
            if (event->modifiers() & Qt::ControlModifier)
                ;//m_timelineDock->makeTracksTaller();
            else
                m_timelineDock->zoomIn();
        }
        break;
    case Qt::Key_Enter: // Seek to current playlist item
    case Qt::Key_Return:
        if (multitrack()) {
            if (m_timelineDock->selection().size())
                m_timelineDock->seekInPoint(m_timelineDock->selection().first());
        } /*else if (m_playlistDock->position() >= 0) {
            if (event->modifiers() == Qt::ShiftModifier)
                seekPlaylist(m_playlistDock->position());
            else
                m_playlistDock->on_actionOpen_triggered();
        }*/
        break;
    case Qt::Key_F5:
        m_timelineDock->model()->reload();
        break;
    case Qt::Key_F1:
        LOG_DEBUG() << "Current focusWidget:" << QApplication::focusWidget();
        LOG_DEBUG() << "Current focusObject:" << QApplication::focusObject();
        LOG_DEBUG() << "Current focusWindow:" << QApplication::focusWindow();
        break;
    default:
        break;
    }

    if (!handled)
        QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    Q_ASSERT(event);
    if (event->key() == Qt::Key_K)
        m_isKKeyPressed = false;
    else
        QMainWindow::keyReleaseEvent(event);
}

// Drag-n-drop events

bool MainWindow::eventFilter(QObject* target, QEvent* event)
{
    Q_ASSERT(event);
    if (!event) {
        return false;
    }
    if (event->type() == QEvent::Wheel)
    {
    //    const QMetaObject *mobj = target->metaObject();
    //    qDebug()<<"class name is "<<mobj->className()<<"obj name is "<<target->objectName();
    }

    if (event->type() == QEvent::DragEnter && target == MLT.videoWidget())
    {
        dragEnterEvent(static_cast<QDragEnterEvent*>(event));
        return true;
    }
    else if (event->type() == QEvent::Drop && target == MLT.videoWidget())
    {
        dropEvent(static_cast<QDropEvent*>(event));
        return true;
    }
    else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
    {
        QQuickWidget * focusedQuickWidget = qobject_cast<QQuickWidget*>(qApp->focusWidget());
        if (focusedQuickWidget)
        {
            event->accept();
            focusedQuickWidget->quickWindow()->sendEvent(focusedQuickWidget->quickWindow()->activeFocusItem(), event);
            QWidget * w = focusedQuickWidget->parentWidget();
            if (!event->isAccepted())
                qApp->sendEvent(w, event);
            return true;
        }
    }
    else if (event->type() == QEvent::NonClientAreaMouseButtonDblClick
               && (target == m_encodeDock || target == m_jobsDock || target == m_propertiesDock || target == m_tasksDock))
    {
        return true;
    }

//    else if(target == ui->mainToolBar->widgetForAction(ui->actionOpen))
//    {
//        qDebug()<<"mainwindow eventfilter:target == ui->mainToolBar->widgetForAction(ui->actionOpen)";
//        if(event->type() == QEvent::Enter && event->type() == QEvent::Leave)
//            return true;
//        else
//            return false;

//    }
//    return QMainWindow::eventFilter(target, event);
    return false;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    Q_ASSERT(event);
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    Q_ASSERT(event);
    const QMimeData *mimeData = event->mimeData();
    Q_ASSERT(mimeData);

    if (mimeData->hasFormat("application/x-qabstractitemmodeldatalist")) {
        QByteArray encoded = mimeData->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        QMap<int,  QVariant> roleDataMap;
        while (!stream.atEnd()) {
            int row, col;
            stream >> row >> col >> roleDataMap;
        }
        if (roleDataMap.contains(Qt::ToolTipRole)) {
            // DisplayRole is just basename, ToolTipRole contains full path
            //m_multipleFiles.append(roleDataMap[Qt::ToolTipRole].toString());
            open(roleDataMap[Qt::ToolTipRole].toString());
            event->acceptProposedAction();
        }
    }
    else if (mimeData->hasUrls()) {
        if (mimeData->urls().length() > 1) {
            foreach (QUrl url, mimeData->urls()) {
                QString path = Util::removeFileScheme(url);
                m_multipleFiles.append(path);
            }
        }
        QString path = Util::removeFileScheme(mimeData->urls().first());
        open(path);
        event->acceptProposedAction();
    }
    else if (mimeData->hasFormat(MLT.MltXMLMimeType())) {
//        m_playlistDock->on_actionOpen_triggered();
//        event->acceptProposedAction();
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    Q_ASSERT(event);
    if (continueJobsRunning() && continueModified()) {
//        if (!m_htmlEditor || m_htmlEditor->close())
        {
            writeSettings();
            QThreadPool::globalInstance()->clear();
            AudioLevelsTask::closeAll();
            event->accept();
            //emit aboutToShutDown();
            DB.shutdown();

            //退出程序有时会调用ImageProvider的RequestImage崩溃
            QmlUtilities::sharedEngine()->removeImageProvider(QString("thumbnail"));

            QApplication::exit(m_exitCode);
            return;
        }
    }
    event->ignore();
}

void MainWindow::showEvent(QShowEvent* event)
{
    // This is needed to prevent a crash on windows on startup when timeline
    // is visible and dock title bars are hidden.
    Q_UNUSED(event)
//    ui->actionShowTitleBars->setChecked(Settings.showTitleBars());
//    on_actionShowTitleBars_triggered(Settings.showTitleBars());
//    ui->actionShowToolbar->setChecked(Settings.showToolBar());
//    on_actionShowToolbar_triggered(Settings.showToolBar());

    windowHandle()->installEventFilter(this);

//    QAction* action = new QAction(tr("Click here to check for a new version of Shotcut."), 0);
//    connect(action, SIGNAL(triggered(bool)), SLOT(on_actionUpgrade_triggered()));
//    showStatusMessage(action, 10 /* seconds */);
}

void MainWindow::on_actionOpenOther_triggered()
{
    // these static are used to open dialog with previous configuration
    OpenOtherDialog dialog(this);

    if (MLT.producer())
        dialog.load(MLT.producer());
    if (dialog.exec() == QDialog::Accepted)
        open(dialog.producer(MLT.profile()));
}

void MainWindow::onProducerOpened()
{
    LOG_DEBUG() << "begin";
//    if (m_meltedServerDock)
//        m_meltedServerDock->disconnect(SIGNAL(positionUpdated(int,double,int,int,int,bool)));

    QWidget* w = loadProducerWidget(MLT.producer());
    if (w && !MLT.producer()->get_int(kMultitrackItemProperty)) {
        if (-1 != w->metaObject()->indexOfSignal("producerReopened()"))
            connect(w, SIGNAL(producerReopened()), m_player, SLOT(onProducerOpened()));
    }
//    else if (MLT.isPlaylist()) {
//        m_playlistDock->model()->load();
//        if (playlist()) {
//            m_isPlaylistLoaded = true;
//            m_player->setIn(-1);
//            m_player->setOut(-1);
//            m_playlistDock->setVisible(true);
//            m_playlistDock->raise();
//            m_player->enableTab(Player::ProjectTabIndex);
//            m_player->switchToTab(Player::ProjectTabIndex);
//        }
//    }
    else if (MLT.isMultitrack()) {
        m_timelineDock->model()->load();
        if (multitrack()) {
            m_player->setIn(-1);
            m_player->setOut(-1);
            m_timelineDock->setVisible(true);
            m_timelineDock->raise();
            m_player->enableTab(Player::ProjectTabIndex);
            m_player->switchToTab(Player::ProjectTabIndex);
            if (m_timelineDock->model()->trackList().count() > 0)
                m_timelineDock->setSelection(QList<int>() << 0, m_timelineDock->currentTrack());
            else
                m_timelineDock->selectMultitrack();
        }
    }
    if (MLT.isClip()) {
        m_player->enableTab(Player::SourceTabIndex);
        m_player->switchToTab(Player::SourceTabIndex);
        MLT.getHash(*MLT.producer());
        ui->actionPaste->setEnabled(true);
    }
    if (m_autosaveFile)
        setCurrentFile(m_autosaveFile->managedFileName());
    else if (!MLT.URL().isEmpty())
        setCurrentFile(MLT.URL());
    on_actionJack_triggered(ui->actionJack && ui->actionJack->isChecked());
}

void MainWindow::onProducerChanged()
{
    MLT.refreshConsumer();
}

bool MainWindow::on_actionSave_triggered()
{
    if (m_currentFile.isEmpty()) {
        return on_actionSave_As_triggered();
    } else {
        saveXML(m_currentFile, false);
        setCurrentFile(m_currentFile);
        setWindowModified(false);
        showStatusMessage(tr("Saved %1").arg(m_currentFile));
        m_undoStack->setClean();
        return true;
    }
}

bool MainWindow::on_actionSave_As_triggered()
{
    QString path = Settings.savePath();
    path.append("/untitled.mmp");//xjp
    QString filename = QFileDialog::getSaveFileName(this, tr("Save MMP"), path, tr("MMP (*.mmp)"));
    if (!filename.isEmpty()) {
        QFileInfo fi(filename);
        Settings.setSavePath(fi.path());
        // if (fi.suffix() != "mlt")
        //     filename += ".mlt";
        // if (MLT.producer())
        //     saveXML(filename, false);
        // else
        //     showStatusMessage(tr("Unable to save empty file, but saved its name for future."));
        if (fi.suffix() != "mmp")//xjp
            filename += ".mmp";//xjp
        if (MLT.producer())
            saveXML(filename, false);
        else
            showStatusMessage(tr("Unable to save empty file, but saved its name for future."));
        if (m_autosaveFile)
            m_autosaveFile->changeManagedFile(filename);
        else
            m_autosaveFile = new AutoSaveFile(filename);
        setCurrentFile(filename);
        setWindowModified(false);
        if (MLT.producer())
            showStatusMessage(tr("Saved %1").arg(m_currentFile));
        m_undoStack->setClean();
    }
    return !filename.isEmpty();
}

bool MainWindow::continueModified()
{
    if (isWindowModified()) {
        QMessageBox dialog(QMessageBox::NoIcon,
                                     qApp->applicationName(),
                                     tr("The project has been modified.\n"
                                        "Do you want to save your changes?"),
                                     QMessageBox::Discard |
                                     QMessageBox::Cancel |
                                     QMessageBox::Save,
                                     this);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
        dialog.setWindowModality(QmlApplication::dialogModality());
        dialog.setDefaultButton(QMessageBox::Save);
        dialog.setEscapeButton(QMessageBox::Cancel);
        int r = dialog.exec();
        if (r == QMessageBox::Save || r == QMessageBox::Discard) {
            QMutexLocker locker(&m_autosaveMutex);
            m_autosaveFile->reset();
            if (r == QMessageBox::Save)
                return on_actionSave_triggered();
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::continueJobsRunning()
{
    if (JOBS.hasIncomplete()) {
        QMessageBox dialog(QMessageBox::Warning,
                                     qApp->applicationName(),
                                     tr("There are incomplete jobs.\n"
                                        "Do you want to still want to exit?"),
                                     QMessageBox::No |
                                     QMessageBox::Yes,
                                     this);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
        dialog.setWindowModality(QmlApplication::dialogModality());
        dialog.setDefaultButton(QMessageBox::Yes);
        dialog.setEscapeButton(QMessageBox::No);
        return (dialog.exec() == QMessageBox::Yes);
    }
    return true;
}

QUndoStack* MainWindow::undoStack() const
{
    return m_undoStack;
}

void MainWindow::pushCommand(QUndoCommand *command)
{
    Q_ASSERT(g_isInUndoRedoProcess == false);
    m_undoStack->push(command);
}

void MainWindow::onEncodeTriggered(bool checked)
{
    if (checked) {
    //    m_encodeWidget->show();
        bool bHasClipInTimeline=false;
        MultitrackModel *model = m_timelineDock->model();
        TrackList trackList = model->trackList();
        LOG_DEBUG()<<"trackList count is:"<<trackList.size();
        for(int i=0; i<trackList.size(); i++)
        {
            int clipCount = m_timelineDock->clipCount(i);
            Mlt::ClipInfo *clipInfo = m_timelineDock->getClipInfo(i, 0);

            LOG_DEBUG()<<"clipCount is:"<<clipCount;
            if(clipCount == 1 && (!strcmp(clipInfo->resource,"blank")))
            {
                bHasClipInTimeline = false;

            }
            else
            {
                bHasClipInTimeline = true;
                break;
            }

        }
        if(bHasClipInTimeline)
        {
            m_encodeDock->show();
#if MOVIEMATOR_FREE
            showUpgradeToProPromptDialog();
#endif
        }
        else
        {
            QMessageBox dialog(QMessageBox::NoIcon,
                                         qApp->applicationName(),
                                         tr("To export video, you must add one or more files to timeline."),
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
        Q_UNUSED(r);
        }
 //       m_encodeDock->eventFilter(m_encodeDock, new QEvent(QEvent::NonClientAreaMouseButtonDblClick));
     //   m_encodeDock->raise();

//        m_player->hide();
//        m_playlistDock->hide();
//        m_filtersDock->hide();
//        m_recentDock->hide();
    }
}

void MainWindow::onCaptureStateChanged(bool started)
{
    if (started && (MLT.resource().startsWith("x11grab:") ||
                    MLT.resource().startsWith("gdigrab:") ||
                    MLT.resource().startsWith("avfoundation"))
                && !MLT.producer()->get_int(kBackgroundCaptureProperty))
        showMinimized();
}

void MainWindow::onJobsDockTriggered(bool checked)
{
    if (checked) {
        m_jobsDock->show();
        m_jobsDock->raise();
       // m_jobsDock->highlightCurrentJobs();
    }
}

void MainWindow::on_tasksDockTriggered(bool checked)
{
    if (checked) {
        m_tasksDock->show();
        m_tasksDock->raise();
    }
}

void MainWindow::onRecentDockTriggered(bool checked)
{
    if (checked) {
     //   m_recentDock->show();
     //   m_recentDock->raise();
    }
}

//高级属性的显示和隐藏
void MainWindow::onPropertiesDockTriggered(bool checked)
{
    if (checked)
    {
        m_propertiesDock->show();
        m_propertiesDock->raise();
    }
    else
        m_propertiesDock->hide();
}

//显示右上角简单属性界面
void MainWindow::showPropertiesDock()
{
    m_propertiesDockContainer->showDock(m_simplePropertiesDock);
}

void MainWindow::onPlaylistDockTriggered(bool checked)
{
    if (checked) {
     //   m_playlistDock->show();
     //   m_playlistDock->raise();
    }
}

void MainWindow::onTimelineDockTriggered(bool checked)
{
    if (checked) {
        m_timelineDock->show();
        m_timelineDock->raise();
    }
}

void MainWindow::onHistoryDockTriggered(bool checked)
{
    if (checked) {
   //     m_historyDock->show();
     //   m_historyDock->raise();
    }
}

void MainWindow::onFiltersDockTriggered(bool checked)
{
    if (checked) {
  //      m_filtersDock->show();
   //     m_filtersDock->raise();
    }
}

void MainWindow::onPlaylistCreated()
{
    if (!playlist() || playlist()->count() == 0) return;
    Q_ASSERT(m_player);
    m_player->enableTab(Player::ProjectTabIndex, true);
}

void MainWindow::onPlaylistLoaded()
{
    Q_ASSERT(m_player);
    updateMarkers();
    m_player->enableTab(Player::ProjectTabIndex, true);
}

void MainWindow::onPlaylistCleared()
{
    m_player->setPauseAfterOpen(true);
    open(new Mlt::Producer(MLT.profile(), "color:"));
    m_player->seek(0);
    setWindowModified(true);
}

void MainWindow::onPlaylistClosed()
{
    setProfile(Settings.playerProfile());
    setCurrentFile("");
    setWindowModified(false);
    m_undoStack->clear();
    MLT.resetURL();
    delete m_autosaveFile;
    m_autosaveFile = new AutoSaveFile(untitledFileName());
    if (!multitrack())
        m_player->enableTab(Player::ProjectTabIndex, false);
}

void MainWindow::onPlaylistModified()
{
    setWindowModified(true);
    if (MLT.producer() && playlist() && static_cast<void*>(MLT.producer()->get_producer()) == static_cast<void*>(playlist()->get_playlist()))
        m_player->onDurationChanged();
    updateMarkers();
    m_player->enableTab(Player::ProjectTabIndex, true);
}

void MainWindow::onMultitrackCreated()
{
    m_player->enableTab(Player::ProjectTabIndex, true);
}

void MainWindow::onMultitrackClosed()
{
    LOG_DEBUG() << "begin";
    setProfile(Settings.playerProfile());
    onPlaylistCleared();
    setCurrentFile("");
    setWindowModified(false);
    m_undoStack->clear();
    MLT.resetURL();
    delete m_autosaveFile;
    m_autosaveFile = new AutoSaveFile(untitledFileName());
    if (!playlist() || playlist()->count() == 0)
        m_player->enableTab(Player::ProjectTabIndex, false);

    createMultitrackModelIfNeeded();
    LOG_DEBUG() << "end";
}

void MainWindow::onMultitrackModified()
{
    setWindowModified(true);
}

void MainWindow::onMultitrackDurationChanged()
{
    if (MLT.producer() && static_cast<void*>(MLT.producer()->get_producer()) == static_cast<void*>(multitrack()->get_producer()))
        m_player->onDurationChanged();
}

void MainWindow::onCutModified()
{
    if (!playlist() && !multitrack()) {
        setWindowModified(true);
        updateAutoSave();
    }
//    if (playlist())
//        m_playlistDock->setUpdateButtonEnabled(true);
}

void MainWindow::onFilterModelChanged()
{
    setWindowModified(true);
    updateAutoSave();
//    if (playlist())
//        m_playlistDock->setUpdateButtonEnabled(true);
}

void MainWindow::updateMarkers()
{
    if (playlist() && MLT.isPlaylist()) {
        QList<int> markers;
        int n = playlist()->count();
        for (int i = 0; i < n; i++)
            markers.append(playlist()->clip_start(i));
        m_player->setMarkers(markers);
    }
}

void MainWindow::updateThumbnails()
{
//    if (Settings.playlistThumbnails() != "hidden")
//        m_playlistDock->model()->refreshThumbnails();
}

void MainWindow::on_actionUndo_triggered()
{
    m_undoStack->undo();
//#ifndef NDEBUG
//    if(m_undoStack->count() <= 0)
//    {
//        m_timelineDock->unitTestCommand();
//    }
//#endif
}

void MainWindow::on_actionRedo_triggered()
{
    m_undoStack->redo();
}

void MainWindow::on_actionFAQ_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.macvideostudio.com/moviemator-free-mac-video-editor.html"));
}

void MainWindow::on_actionForum_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.effectmatrix.com/forum/viewforum.php?f=45&sid=c7610a450cafe691f22562e1fe16c817"));
}

void MainWindow::on_actionFacebook_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.facebook.com/TotalVideoConverterPro"));
}

void MainWindow::on_actionHomePage_triggered()
{
//    QDesktopServices::openUrl(QUrl(g_homePage));
    QDesktopServices::openUrl(QUrl(tr("http://moviemator.net")));
}



void MainWindow::saveXML(const QString &filename, bool withRelativePaths)
{
    if (multitrack()) {
        MLT.saveXML(filename, multitrack(), withRelativePaths);
    } else if (playlist()) {
        int in = MLT.producer()->get_in();
        int out = MLT.producer()->get_out();
        MLT.producer()->set_in_and_out(0, MLT.producer()->get_length() - 1);
        MLT.saveXML(filename, playlist(), withRelativePaths);
        MLT.producer()->set_in_and_out(in, out);
    } else {
        MLT.saveXML(filename, nullptr, withRelativePaths);
    }
}

void MainWindow::changeTheme(const QString &theme)
{
    LOG_DEBUG() << "begin";
    if (theme == "dark") {
        QApplication::setStyle("Windows");//Fusion");
        QPalette palette;
        palette.setColor(QPalette::Window, QColor(53,53,53));
        palette.setColor(QPalette::WindowText, QColor(235,235,235));
        //palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(255,0,0));
        palette.setColor(QPalette::Base, QColor(82,82,82));
        palette.setColor(QPalette::AlternateBase, QColor(40,40,40));
        //palette.setColor(QPalette::Highlight, QColor(23,92,118));
        palette.setColor(QPalette::Active, QPalette::Highlight, QColor(192,72,44));
        palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor(82,82,82));
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(192,72,44));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::ToolTipBase, QColor(203,203,203));
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, palette.color(QPalette::WindowText));
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Button, QColor(82,82,82));
        palette.setColor(QPalette::ButtonText, palette.color(QPalette::WindowText));
        palette.setColor(QPalette::Link, palette.color(QPalette::Highlight).lighter());
        palette.setColor(QPalette::LinkVisited, palette.color(QPalette::Highlight));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(82,82,82));

        QApplication::setPalette(palette);
        QIcon::setThemeName("light");
    } else if (theme == "light") {
        QStyle* style = QStyleFactory::create("Fusion");
        QApplication::setStyle(style);
        QApplication::setPalette(style->standardPalette());
        QIcon::setThemeName("light");
    } else {
        QApplication::setStyle(qApp->property("system-style").toString());
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
        QIcon::setThemeName("");
#else
        QIcon::setThemeName("oxygen");
#endif
    }
    emit QmlApplication::singleton().paletteChanged();
    LOG_DEBUG() << "end";
}

Mlt::Playlist* MainWindow::playlist() const
{
    return nullptr;
//    return m_playlistDock->model()->playlist();
}

Mlt::Producer *MainWindow::multitrack() const
{
    return m_timelineDock->model()->tractor();
}

QWidget *MainWindow::loadProducerWidget(Mlt::Producer* producer)
{
    QWidget* w = nullptr;
    //QScrollArea* scrollArea = (QScrollArea*) m_propertiesDock->widget();
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(m_simplePropertiesDock->widget());


    QWidget* advancedW = nullptr; //视频高级属性
    QScrollArea* advancedScrollArea = qobject_cast<QScrollArea*>(m_propertiesDock->widget()); //视频高级属性

//    Q_ASSERT(producer);//会有空的调用
//    Q_ASSERT(producer->is_valid());
    if (!producer || !producer->is_valid()
            || producer->get("moviemator:imageName")) {
        if (scrollArea->widget())
            scrollArea->widget()->deleteLater();
        if (advancedScrollArea->widget())
            scrollArea->widget()->deleteLater();
        return  w;
    }  

    QString service(producer->get("mlt_service"));
    QString resource = QString::fromUtf8(producer->get("resource"));
    QString shotcutProducer(producer->get(kShotcutProducerProperty));

    if (resource.startsWith("video4linux2:"))
        w = new Video4LinuxWidget(this);
    else if (resource.startsWith("pulse:"))
        w = new PulseAudioWidget(this);
    else if (resource.startsWith("jack:"))
        w = new JackProducerWidget(this);
    else if (resource.startsWith("alsa:"))
        w = new AlsaWidget(this);
    else if (resource.startsWith("dshow:"))
        w = new DirectShowVideoWidget(this);
    else if (resource.startsWith("avfoundation:"))
        w = new AvfoundationProducerWidget(this);
    else if (resource.startsWith("x11grab:"))
        w = new X11grabWidget(this);
    else if (resource.startsWith("gdigrab:"))
        w = new GDIgrabWidget(this);
    else if (service.startsWith("avformat") || shotcutProducer == "avformat") {
        w = new AvformatProducerSimpleWidget(this);
        advancedW = new AvformatProducerWidget(this);
    }
    else if (MLT.isImageProducer(producer)) {
        ImageProducerWidget* ipw = new ImageProducerWidget(this);
        connect(m_player, SIGNAL(outChanged(int)), ipw, SLOT(setOutPoint(int)));
        w = ipw;
    }
    else if (service == "decklink" || resource.contains("decklink"))
        w = new DecklinkProducerWidget(this);
    else if (service == "color")
        w = new ColorProducerWidget(this);
    else if (service == "noise")
        w = new NoiseWidget(this);
    else if (service == "frei0r.ising0r")
        w = new IsingWidget(this);
    else if (service == "frei0r.lissajous0r")
        w = new LissajousWidget(this);
    else if (service == "frei0r.plasma")
        w = new PlasmaWidget(this);
    else if (service == "frei0r.test_pat_B")
        w = new ColorBarsWidget(this);
    else if (service.contains("webvfx"))
        w = new WebvfxProducer(this);
    else if (service == "tone")
        w = new ToneProducerWidget(this);
    else if (producer->parent().get(kShotcutTransitionProperty)) {
        int trackIndex = m_timelineDock->selectedTrackIndex();
        int clipIndex = m_timelineDock->selection().at(0);
        w = new LumaMixTransition(producer->parent(), trackIndex, clipIndex, this);
        scrollArea->setWidget(w);
        // 转场就显示属性界面
        showPropertiesDock();
        return w;
    } else if (playlist_type == producer->type()) {
        w = new TrackPropertiesWidget(*producer, this);
        scrollArea->setWidget(w);
        return w;
    } else if (tractor_type == producer->type()) {
        w = new TimelinePropertiesWidget(*producer, this);
        scrollArea->setWidget(w);
        return w;
    }
    if (w) {
        dynamic_cast<AbstractProducerWidget*>(w)->setProducer(producer);
        if (-1 != w->metaObject()->indexOfSignal("producerChanged(Mlt::Producer*)")) {
            connect(w, SIGNAL(producerChanged(Mlt::Producer*)), SLOT(onProducerChanged()));
            connect(w, SIGNAL(producerChanged(Mlt::Producer*)), m_filterController, SLOT(setProducer(Mlt::Producer*)));
            if (producer->get_int(kMultitrackItemProperty))
                connect(w, SIGNAL(producerChanged(Mlt::Producer*)), m_timelineDock, SLOT(onProducerChanged(Mlt::Producer*)));
        }
        scrollArea->setWidget(w);
        onProducerChanged();
    } else if (scrollArea->widget()) {
        scrollArea->widget()->deleteLater();
    }

    //视频高级属性
    if (advancedW) {
        dynamic_cast<AbstractProducerWidget*>(advancedW)->setProducer(producer);
        if (-1 != advancedW->metaObject()->indexOfSignal("producerChanged(Mlt::Producer*)")) {
            connect(advancedW, SIGNAL(producerChanged(Mlt::Producer*)), SLOT(onProducerChanged()));
            connect(advancedW, SIGNAL(producerChanged(Mlt::Producer*)), m_filterController, SLOT(setProducer(Mlt::Producer*)));
            if (producer->get_int(kMultitrackItemProperty))
                connect(advancedW, SIGNAL(producerChanged(Mlt::Producer*)), m_timelineDock, SLOT(onProducerChanged(Mlt::Producer*)));
        }
        advancedScrollArea->setWidget(advancedW);
        onProducerChanged();
    } else if (advancedScrollArea->widget()) {
        advancedScrollArea->widget()->deleteLater();
    }
    return w;
}

void MainWindow::onMeltedUnitOpened()
{
    Mlt::Producer* producer = new Mlt::Producer(MLT.profile(), "color:");
    MLT.setProducer(producer);
    MLT.play(0);
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(m_propertiesDock->widget());
    delete scrollArea->widget();
//    if (m_meltedServerDock && m_meltedPlaylistDock) {
//        m_player->connectTransport(m_meltedPlaylistDock->transportControl());
//        connect(m_meltedServerDock, SIGNAL(positionUpdated(int,double,int,int,int,bool)),
//                m_player, SLOT(onShowFrame(int,double,int,int,int,bool)));
//    }
    onProducerChanged();
}

void MainWindow::onMeltedUnitActivated()
{
//    m_meltedPlaylistDock->setVisible(true);
//    m_meltedPlaylistDock->raise();
}

void MainWindow::on_actionEnter_Full_Screen_triggered()
{
    if (isFullScreen()) {
        showNormal();
        ui->actionEnter_Full_Screen->setText(tr("Enter Full Screen"));
    } else {
        showFullScreen();
        ui->actionEnter_Full_Screen->setText(tr("Exit Full Screen"));
    }
}

void MainWindow::onGpuNotSupported()
{
    Settings.setPlayerGPU(false);
    if (ui->actionGPU) {
        ui->actionGPU->setChecked(false);
        ui->actionGPU->setDisabled(true);
    }
    LOG_WARNING() << "";
    QMessageBox::critical(this, qApp->applicationName(),
        tr("GPU Processing is not supported"));
}

void MainWindow::editHTML(const QString &fileName)
{
    Q_UNUSED(fileName);
//    bool isNew = !m_htmlEditor;
//    if (!m_htmlEditor) {
//        m_htmlEditor = new HtmlEditor;
//        m_htmlEditor->setWindowIcon(windowIcon());
//    }
//    m_htmlEditor->load(fileName);
//    m_htmlEditor->show();
//    m_htmlEditor->raise();
//    if (Settings.playerZoom() >= 1.0f) {
//        m_htmlEditor->changeZoom(100 * m_player->videoSize().width() / MLT.profile().width());
//        m_htmlEditor->resizeWebView(m_player->videoSize().width(), m_player->videoSize().height());
//    } else {
//        m_htmlEditor->changeZoom(100 * MLT.displayWidth() / MLT.profile().width());
//        m_htmlEditor->resizeWebView(MLT.displayWidth(), MLT.displayHeight());
//    }
//    if (isNew) {
//        // Center the new window over the main window.
//        QPoint point = pos();
//        QPoint halfSize(width(), height());
//        halfSize /= 2;
//        point += halfSize;
//        halfSize = QPoint(m_htmlEditor->width(), m_htmlEditor->height());
//        halfSize /= 2;
//        point -= halfSize;
//        m_htmlEditor->move(point);
//    }
}

void MainWindow::stepLeftOneFrame()
{
    m_player->seek(m_player->position() - 1);
}

void MainWindow::stepRightOneFrame()
{
    m_player->seek(m_player->position() + 1);
}

void MainWindow::stepLeftOneSecond()
{
    stepLeftBySeconds(-1);
}

void MainWindow::stepRightOneSecond()
{
    stepLeftBySeconds(1);
}

void MainWindow::setInToCurrent(bool ripple)
{
    if (m_player->tabIndex() == Player::ProjectTabIndex && multitrack()) {
        m_timelineDock->trimClipAtPlayhead(TimelineDock::TrimInPoint, ripple);
    } else if (MLT.isSeekable() && MLT.isClip()) {
        m_player->setIn(m_player->position());
    }
}

void MainWindow::setOutToCurrent(bool ripple)
{
    if (m_player->tabIndex() == Player::ProjectTabIndex && multitrack()) {
        m_timelineDock->trimClipAtPlayhead(TimelineDock::TrimOutPoint, ripple);
    } else if (MLT.isSeekable() && MLT.isClip()) {
        m_player->setOut(m_player->position());
    }
}

void MainWindow::onShuttle(float x)
{
    if (qFuzzyIsNull(x)) {
        m_player->pause();
    } else if (x > 0) {
        m_player->play(10.0 * double(x));
    } else {
        m_player->play(20.0 * double(x));
    }
}

void MainWindow::on_actionRealtime_triggered(bool checked)
{
    Settings.setPlayerRealtime(checked);
    if (Settings.playerGPU())
        MLT.pause();
    if (MLT.consumer()) {
        MLT.restart();
    }

}

void MainWindow::on_actionProgressive_triggered(bool checked)
{
    MLT.videoWidget()->setProperty("progressive", checked);
    if (Settings.playerGPU())
        MLT.pause();
    if (MLT.consumer()) {
        MLT.profile().set_progressive(checked);
        MLT.restart();
    }
    Settings.setPlayerProgressive(checked);
}

void MainWindow::changeDeinterlacer(bool checked, const char* method)
{
    if (checked) {
        MLT.videoWidget()->setProperty("deinterlace_method", method);
        if (MLT.consumer()) {
            MLT.consumer()->set("deinterlace_method", method);
        }
    }
    Settings.setPlayerDeinterlacer(method);
}

void MainWindow::on_actionOneField_triggered(bool checked)
{
    changeDeinterlacer(checked, "onefield");
}

void MainWindow::on_actionLinearBlend_triggered(bool checked)
{
    changeDeinterlacer(checked, "linearblend");
}

void MainWindow::on_actionYadifTemporal_triggered(bool checked)
{
    changeDeinterlacer(checked, "yadif-nospatial");
}

void MainWindow::on_actionYadifSpatial_triggered(bool checked)
{
    changeDeinterlacer(checked, "yadif");
}

void MainWindow::changeInterpolation(bool checked, const char* method)
{
    if (checked) {
        MLT.videoWidget()->setProperty("rescale", method);
        if (MLT.consumer()) {
            MLT.consumer()->set("rescale", method);
        }
    }
    Settings.setPlayerInterpolation(method);
}



void MainWindow::processMultipleFiles()
{
    if (m_multipleFiles.length() > 1) {
//        PlaylistModel* model = m_playlistDock->model();
//        m_playlistDock->show();
//        m_playlistDock->raise();
//        QThreadPool::globalInstance()->start(new AppendTask(model, m_multipleFiles));
        m_multipleFiles.removeFirst();
        foreach (QString filename, m_multipleFiles)
            onFileOpened(filename);
        m_multipleFiles.clear();
    }
    if (m_isPlaylistLoaded && Settings.playerGPU()) {
        updateThumbnails();
        m_isPlaylistLoaded = false;
    }
}

void MainWindow::onLanguageTriggered(QAction* action)
{

    QMessageBox dialog(QMessageBox::Information,
                       qApp->applicationName(),
                       tr("You must restart MovieMator to switch to the new language.\n"
                          "Do you want to change language now?"),
                       QMessageBox::No | QMessageBox::Yes,
                       this);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
    dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
    dialog.setDefaultButton(QMessageBox::Yes);
    dialog.setEscapeButton(QMessageBox::No);
    dialog.setWindowModality(QmlApplication::dialogModality());
    if (dialog.exec() == QMessageBox::Yes) {
        m_exitCode = EXIT_RESTART;
        Settings.setLanguage(action->data().toString());
        QApplication::closeAllWindows();
    }
    else
    {
//        bool checked = action->isChecked();
//        action->setChecked(!checked);

        QList<QAction*> actionList = m_languagesGroup->actions();
        qDebug()<<action->data()<<",checked:"<<action->isChecked();
        QString curLanguage = Settings.language();
        for(int index =0; index < actionList.size(); index++)
        {
            QAction *tempAction = actionList.at(index);
            qDebug()<<tempAction->data()<<",checked:"<<tempAction->isChecked();

            if(tempAction->data() == curLanguage)
            {
                tempAction->setChecked(true);

            }

        }

    }

}

void MainWindow::on_actionNearest_triggered(bool checked)
{
    changeInterpolation(checked, "nearest");
}

void MainWindow::on_actionBilinear_triggered(bool checked)
{
    changeInterpolation(checked, "bilinear");
}

void MainWindow::on_actionBicubic_triggered(bool checked)
{
    changeInterpolation(checked, "bicubic");
}

void MainWindow::on_actionHyper_triggered(bool checked)
{
    changeInterpolation(checked, "hyper");
}

void MainWindow::on_actionJack_triggered(bool checked)
{
    Settings.setPlayerJACK(checked);
    if (!MLT.enableJack(checked)) {
        if (ui->actionJack)
            ui->actionJack->setChecked(false);
        Settings.setPlayerJACK(false);
        QMessageBox::warning(this, qApp->applicationName(),
            tr("Failed to connect to JACK.\nPlease verify that JACK is installed and running."));
    }
}

void MainWindow::on_actionGPU_triggered(bool checked)
{
    Settings.setPlayerGPU(checked);
    QMessageBox dialog(QMessageBox::Information,
                       qApp->applicationName(),
                       tr("You must restart MovieMator to switch using GPU processing.\n"
                          "Do you want to restart now?"),
                       QMessageBox::No | QMessageBox::Yes,
                       this);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
    dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
    dialog.setDefaultButton(QMessageBox::Yes);
    dialog.setEscapeButton(QMessageBox::No);
    dialog.setWindowModality(QmlApplication::dialogModality());
    if (dialog.exec() == QMessageBox::Yes) {
        m_exitCode = EXIT_RESTART;
        QApplication::closeAllWindows();
    }
}

void MainWindow::onExternalTriggered(QAction *action)
{
    Q_ASSERT(action);
    LOG_DEBUG() << action->data().toString();
    bool isExternal = !action->data().toString().isEmpty();
    Settings.setPlayerExternal(action->data().toString());

    bool ok = false;
    int screen = action->data().toInt(&ok);
    if (ok || action->data().toString().isEmpty()) {
        m_player->moveVideoToScreen(ok? screen : -2);
        isExternal = false;
        MLT.videoWidget()->setProperty("mlt_service", QVariant());
    } else {
        MLT.videoWidget()->setProperty("mlt_service", action->data());
    }

    QString profile = Settings.playerProfile();
    // Automatic not permitted for SDI/HDMI
    if (isExternal && profile.isEmpty()) {
        profile = "atsc_720p_50";
        Settings.setPlayerProfile(profile);
        setProfile(profile);
        MLT.restart();
        foreach (QAction* a, m_profileGroup->actions()) {
            if (a->data() == profile) {
                a->setChecked(true);
                break;
            }
        }
    }
    else {
        MLT.consumerChanged();
    }
    // Automatic not permitted for SDI/HDMI
    m_profileGroup->actions().at(0)->setEnabled(!isExternal);

    // Disable progressive option when SDI/HDMI
    ui->actionProgressive->setEnabled(!isExternal);
    bool isProgressive = isExternal
            ? MLT.profile().progressive()
            : ui->actionProgressive->isChecked();
    MLT.videoWidget()->setProperty("progressive", isProgressive);
    if (MLT.consumer()) {
        MLT.consumer()->set("progressive", isProgressive);
        MLT.restart();
    }
    if (m_keyerMenu)
        m_keyerMenu->setEnabled(action->data().toString().startsWith("decklink"));
}

void MainWindow::onKeyerTriggered(QAction *action)
{
    Q_ASSERT(action);
    LOG_DEBUG() << action->data().toString();
    MLT.videoWidget()->setProperty("keyer", action->data());
    MLT.consumerChanged();
    Settings.setPlayerKeyerMode(action->data().toInt());
}

void MainWindow::changeProfile(QString strProfileName)
{
    //检测是否和当前profile相等
    QString strCurrentPrefileName = Settings.playerProfile();
    if (strProfileName == strCurrentPrefileName)
    {
        return;
    }

    //need restart
    QMessageBox dialog(QMessageBox::Information,
                       qApp->applicationName(),
                       tr("You must restart MovieMator to change the video mode.\n"
                          "Do you want to change video mode now?"),
                       QMessageBox::No | QMessageBox::Yes,
                       this);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
    dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif

    dialog.setDefaultButton(QMessageBox::Yes);
    dialog.setEscapeButton(QMessageBox::No);
    dialog.setWindowModality(QmlApplication::dialogModality());

    if (dialog.exec() == QMessageBox::Yes)
    {
        if (continueModified())
        {
            m_exitCode = EXIT_RESTART;
            //close project
            if (multitrack())
            {
                m_timelineDock->model()->close();
            }

            //set profile
            Settings.setPlayerProfile(strProfileName);
            QApplication::closeAllWindows();
            return;
        }
    }
}

void MainWindow::onProfileTriggered(QAction *action)
{
    Q_ASSERT(action);

    //need restart
    QMessageBox dialog(QMessageBox::Information,
                       qApp->applicationName(),
                       tr("You must restart MovieMator to change the video mode.\n"
                          "Do you want to change video mode now?"),
                       QMessageBox::No | QMessageBox::Yes,
                       this);
#if MOVIEMATOR_PRO
        dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
    dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
    dialog.setDefaultButton(QMessageBox::Yes);
    dialog.setEscapeButton(QMessageBox::No);
    dialog.setWindowModality(QmlApplication::dialogModality());
    if (dialog.exec() == QMessageBox::Yes) {
        if (continueModified())
        {
            m_exitCode = EXIT_RESTART;
            //close project
            if (multitrack())
                m_timelineDock->model()->close();
            //set profile
            Settings.setPlayerProfile(action->data().toString());
            QApplication::closeAllWindows();
            return;
        }
    }


    //discard changes
    {
        QList<QAction*> actionList = m_profileGroup->actions();
        //qDebug()<<action->data()<<",checked:"<<action->isChecked();
        QString curProfile = Settings.playerProfile();
        for(int index =0; index < actionList.size(); index++)
        {
            QAction *tempAction = actionList.at(index);
            //qDebug()<<tempAction->data()<<",checked:"<<tempAction->isChecked();

            if(tempAction->data() == curProfile)
            {
                tempAction->setChecked(true);
            }
        }
    }
}

void MainWindow::onProfileChanged()
{
    if (multitrack() && MLT.isMultitrack() &&
       (m_timelineDock->selection().isEmpty() || m_timelineDock->currentTrack() == -1)) {
        emit m_timelineDock->selected(multitrack());
    }
}

void MainWindow::on_actionAddCustomProfile_triggered()
{
    CustomProfileDialog dialog(this);
    dialog.setWindowModality(QmlApplication::dialogModality());
    if (dialog.exec() == QDialog::Accepted) {
        QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
        if (dir.cd("profiles")) {
            QString name = dialog.profileName();
            QStringList profiles = dir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
            if (profiles.length() == 1)
                m_customProfileMenu->addSeparator();
            QAction* action = addProfile(m_profileGroup, name, dir.filePath(name));
            action->setChecked(true);
            m_customProfileMenu->addAction(action);
        }
    }
}

void MainWindow::on_actionSystemTheme_triggered()
{
    changeTheme("system");
    QApplication::setPalette(QApplication::style()->standardPalette());
    Settings.setTheme("system");
}

void MainWindow::on_actionFusionDark_triggered()
{
    changeTheme("dark");
    Settings.setTheme("dark");
}

void MainWindow::on_actionFusionLight_triggered()
{
    changeTheme("light");
    Settings.setTheme("light");
}

void MainWindow::on_actionTutorials_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.etinysoft.com"));
}

void MainWindow::on_actionRestoreLayout_triggered()
{
    restoreGeometry(Settings.windowGeometryDefault());
    restoreState(Settings.windowStateDefault());
}

void MainWindow::on_actionShowTitleBars_triggered(bool checked)
{
    QList <QDockWidget *> docks = findChildren<QDockWidget *>();
    for (int i = 0; i < docks.count(); i++) {
        QDockWidget* dock = docks.at(i);
        if (checked) {
            dock->setTitleBarWidget(nullptr);
        } else {
            if (!dock->isFloating()) {
                dock->setTitleBarWidget(new QWidget);
            }
        }
    }
    Settings.setShowTitleBars(checked);
}

void MainWindow::on_actionShowToolbar_triggered(bool checked)
{
    Q_UNUSED(checked);
//    ui->mainToolBar->setVisible(checked);
}

void MainWindow::onToolbarVisibilityChanged(bool visible)
{
    ui->actionShowToolbar->setChecked(visible);
    Settings.setShowToolBar(visible);
}

void MainWindow::on_menuExternal_aboutToShow()
{
    foreach (QAction* action, m_externalGroup->actions()) {
        bool ok = false;
        int i = action->data().toInt(&ok);
        if (ok) {
            if (i == QApplication::desktop()->screenNumber(this)) {
                if (action->isChecked()) {
                    m_externalGroup->actions().first()->setChecked(true);
                    Settings.setPlayerExternal(QString());
                }
                action->setDisabled(true);
            }  else {
                action->setEnabled(true);
            }
        }
    }
}

void MainWindow::on_actionUpgrade_triggered()
{
    // showStatusMessage("Checking for upgrade...");
    // m_network.get(QNetworkRequest(QUrl("https://shotcut.org/version.json")));
    QDesktopServices::openUrl(QUrl("http://www.etinysoft.com"));
}

void MainWindow::on_actionOpenXML_triggered()
{
    QString path = Settings.openPath();
#ifdef Q_OS_MAC
    path.append("/*");
#endif
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open File"), path,
        tr("MMP (*.mmp);;All Files (*)"));//xjp
    if (filenames.length() > 0) {
        QString url = filenames.first();
        MltXmlChecker checker;
        if (checker.check(url)) {
            if (!isCompatibleWithGpuMode(checker))
                return;
            isXmlRepaired(checker, url);
        }
        Settings.setOpenPath(QFileInfo(url).path());
        activateWindow();
        if (filenames.length() > 1)
            m_multipleFiles = filenames;
        if (!MLT.openXML(url)) {
            open(MLT.producer());
            onFileOpened(url);
            LOG_INFO() << url;
        }
        else {
            showStatusMessage(tr("Failed to open ") + url);
            onOpenFailed(url);
        }
    }
}

void MainWindow::on_actionGammaSRGB_triggered(bool checked)
{
    Q_UNUSED(checked)
    Settings.setPlayerGamma("iec61966_2_1");
    MLT.restart();
    MLT.refreshConsumer();
}

void MainWindow::on_actionGammaRec709_triggered(bool checked)
{
    Q_UNUSED(checked)
    Settings.setPlayerGamma("bt709");
    MLT.restart();
    MLT.refreshConsumer();
}

void MainWindow::onFocusChanged(QWidget *, QWidget * ) const
{
    LOG_DEBUG() << "Focuswidget changed";
    LOG_DEBUG() << "Current focusWidget:" << QApplication::focusWidget();
    LOG_DEBUG() << "Current focusObject:" << QApplication::focusObject();
    LOG_DEBUG() << "Current focusWindow:" << QApplication::focusWindow();
}

void MainWindow::on_actionScrubAudio_triggered(bool checked)
{
    Settings.setPlayerScrubAudio(checked);
}

#ifdef Q_OS_WIN
void MainWindow::onDrawingMethodTriggered(QAction *action)
{
    Settings.setDrawMethod(action->data().toInt());
    QMessageBox dialog(QMessageBox::Information,
                       qApp->applicationName(),
                       tr("You must restart MovieMator to change the display method.\n"
                          "Do you want to restart now?"),
                       QMessageBox::No | QMessageBox::Yes,
                       this);
    dialog.setDefaultButton(QMessageBox::Yes);
    dialog.setEscapeButton(QMessageBox::No);
    dialog.setWindowModality(QmlApplication::dialogModality());
    if (dialog.exec() == QMessageBox::Yes) {
        m_exitCode = EXIT_RESTART;
        QApplication::closeAllWindows();
    }
}
#endif

void MainWindow::on_actionApplicationLog_triggered()
{
    TextViewerDialog dialog(this);
    QDir dir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
    QFile logFile(dir.filePath("moviemator-log.txt"));
    logFile.open(QIODevice::ReadOnly | QIODevice::Text);
    dialog.setText(logFile.readAll());
    logFile.close();
    dialog.setWindowTitle(tr("Application Log"));
    dialog.exec();
}

void MainWindow::on_actionClose_triggered()
{
    if (MAIN.continueModified()) {
        LOG_DEBUG() << "";

//        if (playlist())
//            m_playlistDock->model()->close();
        if (multitrack())
            m_timelineDock->model()->close();
        else
            onMultitrackClosed();
    }
}

void MainWindow::onPlayerTabIndexChanged(int index)
{
    Q_UNUSED(index);
}

void MainWindow::onUpgradeCheckFinished(QNetworkReply* reply)
{
    Q_ASSERT(reply);
    if (!reply->error()) {
        QByteArray response = reply->readAll();
        LOG_DEBUG() << "response: " << response;
        QJsonDocument json = QJsonDocument::fromJson(response);
        if (!json.isNull() && json.object().value("version_string").type() == QJsonValue::String) {
            QString version = json.object().value("version_string").toString();
            if (version != qApp->applicationVersion()) {
                QAction* action = new QAction(tr("MovieMator version %1 is available! Click here to get it.").arg(version), nullptr);
                connect(action, SIGNAL(triggered(bool)), SLOT(onUpgradeTriggered()));
                showStatusMessage(action, 10 /* seconds */);
            } else {
                showStatusMessage(tr("You are running the latest version of MovieMator."));
            }
            reply->deleteLater();
            return;
        } else {
            LOG_WARNING() << "failed to parse version.json";
        }
    } else {
        LOG_WARNING() << reply->errorString();
    }
    QAction* action = new QAction(tr("Failed to read version.json when checking. Click here to go to the Web site."), nullptr);
    connect(action, SIGNAL(triggered(bool)), SLOT(onUpgradeTriggered()));
    showStatusMessage(action);
    reply->deleteLater();
}

void MainWindow::onUpgradeTriggered()
{
//    QDesktopServices::openUrl(QUrl("https://www.shotcut.org/download/"));
}

void MainWindow::onTimelineSelectionChanged()
{
    bool enable = (m_timelineDock->selection().size() > 0);
    ui->actionCut->setEnabled(enable);
    ui->actionCopy->setEnabled(enable);
    ui->actionExport_selected_clip_as_template_file->setEnabled(enable);
}

void MainWindow::on_actionCut_triggered()
{
    m_timelineDock->show();
    m_timelineDock->raise();
    m_timelineDock->removeSelection(true);
}

void MainWindow::on_actionCopy_triggered()
{
    m_timelineDock->show();
    m_timelineDock->raise();
    if (!m_timelineDock->selection().isEmpty())
        m_timelineDock->copyClip(m_timelineDock->currentTrack(), m_timelineDock->selection().first());
}

void MainWindow::on_actionPaste_triggered()
{
    m_timelineDock->show();
    m_timelineDock->raise();
//    m_timelineDock->insert(-1);
    m_timelineDock->paste();    //粘贴
}

void MainWindow::on_actionExport_selected_clip_as_template_file_triggered()
{
    m_timelineDock->exportSelectedClipAsTemplate();
}

void MainWindow::onClipCopied()
{
    m_player->enableTab(Player::SourceTabIndex);
}

void MainWindow::on_actionExportEDL_triggered()
{
    // Dialog to get export file name.
    QString path = Settings.savePath();
    path.append("/.edl");
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Export EDL"), path, tr("EDL (*.edl)"));
    if (!saveFileName.isEmpty()) {
        QFileInfo fi(saveFileName);
        if (fi.suffix() != "edl")
            saveFileName += ".edl";

        // Locate the JavaScript file in the filesystem.
        QDir qmlDir = QmlUtilities::qmlDir();
        qmlDir.cd("export-edl");
        QString jsFileName = qmlDir.absoluteFilePath("export-edl.js");
        QFile scriptFile(jsFileName);
        if (scriptFile.open(QIODevice::ReadOnly)) {
            // Read JavaScript into a string.
            QTextStream stream(&scriptFile);
            QString contents = stream.readAll();
            scriptFile.close();

            // Evaluate JavaScript.
            QJSEngine jsEngine;
            QJSValue result = jsEngine.evaluate(contents, jsFileName);
            if (!result.isError()) {
                // Call the JavaScript main function.
                QJSValue options = jsEngine.newObject();
                options.setProperty("useBaseNameForReelName", true);
                options.setProperty("useBaseNameForClipComment", true);
                options.setProperty("channelsAV", "AA/V");
                QJSValueList args;
                args << MLT.XML() << options;
                result = result.call(args);
                if (!result.isError()) {
                    // Save the result with the export file name.
                    QFile f(saveFileName);
                    f.open(QIODevice::WriteOnly | QIODevice::Text);
                    f.write(result.toString().toLatin1());
                    f.close();
                }
            }
            if (result.isError()) {
                LOG_ERROR() << "Uncaught exception at line"
                            << result.property("lineNumber").toInt()
                            << ":" << result.toString();
                showStatusMessage(tr("A JavaScript error occurred during export."));
            }
        } else {
            showStatusMessage(tr("Failed to open export-edl.js"));
        }
    }
}

void MainWindow::onFilterAddToTimeline(void *producer)
{

    m_timelineDock->addFilterToTimeline(static_cast<Mlt::Producer*>(producer));
}

void MainWindow::onTextAddToTimeline(void *producer)
{

    m_timelineDock->addTextToTimeline(static_cast<Mlt::Producer*>(producer));
}

void MainWindow::createMultitrackModelIfNeeded()
{
    LOG_DEBUG() << "begin";
    if (!m_timelineDock->model()->tractor())
    {
        setCurrentFile("");
        m_timelineDock->model()->createIfNeeded();
        m_timelineDock->model()->addAudioTrack();
        m_timelineDock->model()->addVideoTrack();
        m_timelineDock->model()->addAudioTrack();
   //     m_timelineDock->model()->addFilterTrack();
//        m_timelineDock->model()->addTextTrack();
        m_timelineDock->setCurrentTrack(1);  // 2视频轨时，V1轨道的索引为1
        setWindowModified(false);
        MLT.setProducer(new Mlt::Producer(*multitrack()));
    }
    LOG_DEBUG() << "end";
}


void MainWindow::setCurrentTrack(int trackIndex)
{
    LOG_DEBUG() << "begin";
    if (m_timelineDock->model()->tractor())
        m_timelineDock->setCurrentTrack(trackIndex);
    LOG_DEBUG() << "end";
}


void MainWindow::onEmail_triggered()
{
    QUrl url("mailto:market@effectmatrix.com");
    QDesktopServices::openUrl(url);
}

void MainWindow::onHelpButtonTriggered()
{
//    QDir dir(qApp->applicationDirPath());
//    dir.cdUp();
//    dir.cd("Resources");
//    QString pdfPath = dir.path().append("/MovieMator-Free-Mac-Video-Editor-User-Guide.pdf");
//    QDesktopServices::openUrl(QUrl::fromLocalFile(pdfPath));
    QDesktopServices::openUrl(QUrl(tr("http://www.macvideostudio.com/quickly-start-movie-video-editing-mac.html")));

//    QUrl url("http://www.macvideostudio.com/tutorial/MovieMator-Free-Mac-Video-Editor-User-Guide.pdf");
}

void MainWindow::onShowPropertiesVideoFilterDock()
{
    m_propertiesDockContainer->showDock(m_propertiesVideoFilterDock);
}
void MainWindow::onShowPropertiesAudioFilterDock()
{
    m_propertiesDockContainer->showDock(m_propertiesAudioFilterDock);
}


void MainWindow::setPauseAfterOpen(bool pause)
{
    m_player->setPauseAfterOpen(pause);
}

void MainWindow::appendClipToPlaylist()
{
//    m_playlistDock->on_actionAppendCut_triggered();
}


void MainWindow::highlightCurrentJobs(int index)
{
     m_jobsDock->highlightCurrentJobs(index);

}

QMenu * MainWindow::createPopupMenu()
{
    return nullptr;
}


void MainWindow::appendTextToTimeline(int index)
{
    Q_UNUSED(index);

//    m_textlistDock->addTextToTimeline(index);


}

void MainWindow::showCurrentTextSettingWidget(Mlt::Producer *textProcucer)
{
    Q_UNUSED(textProcucer);
//    m_textlistDock->showTextSettingWidget(textProcucer);
}

QToolButton *MainWindow::createToolButton(const QString& icon, const QString& iconPressed, const QString& iconDisabled, const QString& title, const QString& tooltip)
{
    QToolButton *toolButton = new QToolButton();
    toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolButton->setFixedHeight(60);
    int iconSize = 40;
    toolButton->setIconSize(QSize(iconSize,iconSize));
    QPixmap pixmap(iconSize,iconSize);
    pixmap.fill(QColor(0,0,0,0));
    toolButton->setIcon(QIcon(pixmap));
    QString styleSheet = QString("QToolButton { image: url(%1); image-position: top; border: none; }" "QToolButton:pressed{color: rgb(142, 53, 62); image: url(%2); image-position: top; }" "QToolButton:disabled{image: url(%3)}").arg(icon).arg(iconPressed).arg(iconDisabled);
    toolButton->setStyleSheet(styleSheet);
    toolButton->setToolTip(tooltip);
    toolButton->setText(title);
    return toolButton;
}


void MainWindow::customizeToolbar()
{
    QToolBar *toolbar = new QToolBar;
    toolbar->setObjectName("MainToolBar");
    toolbar->setFloatable(false);
    toolbar->setMovable(false);


    QWidget *widget = new QWidget(this);
    widget->setFixedHeight(75);


    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setHorizontalSpacing(12);

    m_addButton = createToolButton(QString(":/icons/light/32x32/toolbar-add.png"),
                                   QString(":/icons/light/32x32/toolbar-add-pressed.png"),
                                   QString(":/icons/light/32x32/toolbar-add.png"),
                                   tr("Open"), tr("Open a video, audio or image file"));
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(openVideo()));

//    m_removeButton = createToolButton(":/icons/light/32x32/toolbar-remove.png",
//                                      ":/icons/light/32x32/toolbar-remove-pressed.png",
//                                      "", tr("Remove"), tr("Remove media files"));
//    connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeVideo()));


    m_undoButton = createToolButton(":/icons/light/32x32/toolbar-undo.png",
                                    ":/icons/light/32x32/toolbar-undo-pressed.png",
                                    ":/icons/light/32x32/toolbar-undo-disable.png",
                                    tr("Undo"), tr("Undo"));
    connect(m_undoButton, SIGNAL(clicked()), this, SLOT(on_actionUndo_triggered()));


    m_redoButton = createToolButton(":/icons/light/32x32/toolbar-redo.png",
                                    ":/icons/light/32x32/toolbar-redo-pressed.png",
                                    ":/icons/light/32x32/toolbar-redo-disable.png",
                                    tr("Redo"), tr("Redo"));

    connect(m_redoButton, SIGNAL(clicked()), this, SLOT(on_actionRedo_triggered()));


    m_saveButton = createToolButton(":/icons/light/32x32/toolbar-save.png",
                                    ":/icons/light/32x32/toolbar-save-pressed.png",
                                    ":/icons/light/32x32/toolbar-save.png",
                                    tr("Save Project"), tr("Save Project"));
    connect(m_saveButton, SIGNAL(clicked()), this, SLOT(on_actionSave_triggered()));


    m_exportButton = createToolButton(":/icons/light/32x32/toolbar-export.png",
                                      ":/icons/light/32x32/toolbar-export-pressed.png",
                                      ":/icons/light/32x32/toolbar-export.png",
                                      tr("Export Video"), tr("Export video, audio or image file"));
    connect(m_exportButton, SIGNAL(clicked()), this, SLOT(onEncodeTriggered()));


    m_helpButton = createToolButton(":/icons/light/32x32/toolbar-help.png",
                                    ":/icons/light/32x32/toolbar-help-pressed.png",
                                    ":/icons/light/32x32/toolbar-help.png",
                                    tr("Tutorial"), tr("Tutorials"));
    connect(m_helpButton, SIGNAL(clicked()), this, SLOT(onHelpButtonTriggered()));


    m_emailButton = createToolButton(":/icons/light/32x32/toolbar-email.png",
                                     ":/icons/light/32x32/toolbar-email-pressed.png",
                                     ":/icons/light/32x32/toolbar-email.png",
                                     tr("Feedback"), tr("Send us your suggestions"));
    connect(m_emailButton, SIGNAL(clicked()), this, SLOT(onEmail_triggered()));

//    m_forumButton = createToolButton(":/icons/light/32x32/toolbar-forum.png",
//                                     ":/icons/light/32x32/toolbar-forum-pressed.png",
//                                     "", tr("Forum"), tr("Forum"));
//    connect(m_forumButton, SIGNAL(clicked()), this, SLOT(on_actionForum_triggered()));


//    m_exportTemplateButton = createToolButton(":/icons/light/32x32/toolbar-export.png",
//                                      ":/icons/light/32x32/toolbar-export-pressed.png",
//                                      "", tr("Export Template"), tr("Export project as a template file"));
//    connect(m_exportTemplateButton, SIGNAL(clicked()), this, SLOT(onExportTemplate()));


#if SHARE_VERSION
#if MOVIEMATOR_PRO
    if (Registration.registrationType() == Registration_None)
    {
        m_activateButton = createToolButton(":/icons/light/32x32/toolbar-activate.png",
                                            ":/icons/light/32x32/toolbar-activate-pressed.png",
                                            ":/icons/light/32x32/toolbar-activate.png",
                                            tr("Register"), tr("Enter Licensse Code"));
        connect(m_activateButton, SIGNAL(clicked()), this, SLOT(onActivateButton_clicked()));

        m_buynowButton = createToolButton(":/icons/light/32x32/toolbar-buynow.png",
                                          ":/icons/light/32x32/toolbar-buynow-pressed.png",
                                          ":/icons/light/32x32/toolbar-buynow.png",
                                          tr("Buy Now"), tr("Buy a License Code"));
        connect(m_buynowButton, SIGNAL(clicked()), this, SLOT(onBuynowButton_clicked()));
    }
#endif
#endif

#if MOVIEMATOR_FREE
#if defined(Q_OS_MAC)
    m_upgradeButton = createToolButton(":/icons/light/32x32/toolbar-upgrade.png",
                                       ":/icons/light/32x32/toolbar-upgrade-pressed.png",
                                       "", tr("Upgrade"), tr("Upgrade to Pro version"));
    connect(m_upgradeButton, SIGNAL(clicked()), this, SLOT(upgradeToProVersion()));

    m_tvcProButton = createToolButton(":/icons/light/32x32/toolbar-tvcpro.png",
                                      ":/icons/light/32x32/toolbar-tvcpro-pressed.png",
                                      "", tr("Great Converter & DVD Burner"), tr("Get Total Video Converter Pro - a great video converter and dvd burner"));
    connect(m_tvcProButton, SIGNAL(clicked()), this, SLOT(on_actionGet_Total_Video_Converter_Pro_triggered()));
#endif
#endif

    int buttonIndex = 0;
    QSpacerItem *spacer1 = new QSpacerItem(50,20);

    gridLayout->addWidget(m_addButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
//    gridLayout->addWidget(m_removeButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
    gridLayout->addItem(spacer1, 0, buttonIndex++, 1, 1);

    gridLayout->addWidget(m_undoButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(m_redoButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
    QSpacerItem *spacer2 = new QSpacerItem(50,20);
    gridLayout->addItem(spacer2, 0, buttonIndex++, 1, 1);

    gridLayout->addWidget(m_saveButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(m_exportButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
    //gridLayout->addWidget(m_exportTemplateButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
    QSpacerItem *spacer3 = new QSpacerItem(50,20, QSizePolicy::Expanding);
    gridLayout->addItem(spacer3, 0, buttonIndex++, 1, 1);

#if SHARE_VERSION
#if MOVIEMATOR_PRO
    if (Registration.registrationType() == Registration_None)
    {
        gridLayout->addWidget(m_activateButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
        gridLayout->addWidget(m_buynowButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
        QSpacerItem *spacer4 = new QSpacerItem(50,20);
        gridLayout->addItem(spacer4, 0, buttonIndex++, 1, 1);
    }
#endif
#endif



#if MOVIEMATOR_FREE
#if defined(Q_OS_MAC)
    gridLayout->addWidget(m_tvcProButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
    QSpacerItem *spacer4 = new QSpacerItem(50,20);
    gridLayout->addItem(spacer4, 0, buttonIndex++, 1, 1);

    gridLayout->addWidget(m_upgradeButton, 0, buttonIndex, 1, 1, Qt::AlignHCenter);
    QSpacerItem *spacer5 = new QSpacerItem(50,20);
    gridLayout->addItem(spacer5, 0, buttonIndex++, 1, 1);
#endif
#endif

    gridLayout->addWidget(m_helpButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(m_emailButton, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);

//    gridLayout->addWidget(m_forumButton, 0, 10,1,1, Qt::AlignHCenter);

//    QPushButton *clearButtonForTest;
//    clearButtonForTest = new QPushButton("");
//    clearButtonForTest->setFlat(true);
//    clearButtonForTest->setFixedSize(40,40);
//    clearButtonForTest->setIconSize(QSize(40,40));
//    clearButtonForTest->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/toolbar-activate.png)}" "QPushButton:pressed{ border-image: url(:/icons/light/32x32/toolbar-activate-pressed.png)}");
//    clearButtonForTest->setToolTip(tr("Enter Licensse Code"));
//    connect(clearButtonForTest, SIGNAL(clicked()), this, SLOT(on_clearButtonForTest_clicked()));
//    gridLayout->addWidget(clearButtonForTest, 0, buttonIndex++, 1, 1, Qt::AlignHCenter);



    widget->setLayout(gridLayout);
    toolbar->setStyleSheet("QToolBar{background-image: url(:/toolbar-bg.png); background-repeat: repeat-x; border: 0px}");
    toolbar->addWidget(widget);
    this->addToolBar(toolbar);
//    this->setUnifiedTitleAndToolBarOnMac(true);//不加这句，自定义的工具栏将不能显示

    connect(m_undoStack, SIGNAL(canUndoChanged(bool)), m_undoButton, SLOT(setEnabled(bool)));
    connect(m_undoStack, SIGNAL(canRedoChanged(bool)), m_redoButton, SLOT(setEnabled(bool)));
}

void MainWindow::showPlaylistDock()
{
//     m_playlistDock->show();
//     m_recentDock->hide();
//     m_filtersDock->hide();
////    m_playlistDock->raise();

}

void MainWindow::showRecentDock()
{
//    m_playlistDock->hide();
//    m_recentDock->show();
//    m_filtersDock->hide();
//    m_recentDock->raise();
}

void MainWindow::showFilterDock()
{
//    m_playlistDock->hide();
//    m_recentDock->hide();
    m_propertiesVideoFilterDock->show();
//    m_filtersDock->raise();
}
void MainWindow::showAudioFilterDock()
{
    m_propertiesAudioFilterDock->show();
}

void MainWindow::showLoadProgress()
{
    QProgressDialog progressDialog( QStringLiteral("Loading file..."), "",0,0,this, Qt::Sheet);
    progressDialog.setCancelButton(nullptr);
    progressDialog.resize(progressDialog.size() + QSize(200,20));
    connect(this, SIGNAL(hideProgressDialog()), &progressDialog, SLOT(cancel()));
    progressDialog.exec();
}

void MainWindow::onActivateButton_clicked()
{
    m_registrationDialog->show();
    m_registrationDialog->raise();
}

void MainWindow::onBuynowButton_clicked()
{
//    QDesktopServices::openUrl(QUrl("http://www.macvideostudio.com/purchase/buy-video-editor-moviemator-pro.html"));
    QDesktopServices::openUrl(QUrl(tr("http://moviemator.net/buynow.html")));
}

int MainWindow::showRegistrationTipDialog()
{
    return m_registrationTipsDialog->exec();
}

void MainWindow::on_actionBuy_a_License_Code_triggered()
{
    this->onBuynowButton_clicked();
}

void MainWindow::on_actionEnter_License_Code_triggered()
{
    this->onActivateButton_clicked();
}

void MainWindow::on_actionExecute_Python_triggered()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Execute Python File"), ".", tr("Python File (*.py)"));
    if (filenames.count() > 0)
    {
        m_mainController.evalFile(filenames.first());
    }
}

void MainWindow::onClearButtonForTest_clicked()
{
    Settings.remove("kUserMail");
    Settings.remove("kLicenseKey");
}

void MainWindow::updateWindowTitle()
{
    this->setCurrentFile(m_currentFile);
}

void MainWindow::showProFeaturePromptDialog()
{
    m_proFeaturePromptDialog->show();
    m_proFeaturePromptDialog->raise();
}

void MainWindow::showUpgradeToProPromptDialog()
{
    m_upgradeToProPromptDialog->show();
    m_upgradeToProPromptDialog->raise();
}

void MainWindow::upgradeToProVersion()
{
#ifdef SHARE_VERSION
//    QDesktopServices::openUrl(QUrl("http://www.macvideostudio.com/purchase/buy-video-editor-moviemator-pro.html"));
    QDesktopServices::openUrl(QUrl(tr("http://moviemator.net/buynow.html")));
#else
    QString language = QLocale::system().name();
    if (language == "zh_CN")
    {
        QDesktopServices::openUrl(QUrl("https://itunes.apple.com/cn/app/video-editor-moviemator-pro-movie-film-maker/id1178887999?mt=12"));
    }
    else
    {
        QDesktopServices::openUrl(QUrl("https://itunes.apple.com/app/id1178887999"));
    }
#endif

}

#ifdef MOVIEMATOR_PRO
#ifndef SHARE_VERSION
void MainWindow::showInvalidProjectDialog()
{
    m_invalidProjectDiaog->show();
    m_invalidProjectDiaog->raise();
}
#endif
#endif

void MainWindow::on_actionGet_Total_Video_Converter_Pro_triggered()
{

    QString language = QLocale::system().name();
    if (language == "zh_CN")
    {
        QDesktopServices::openUrl(QUrl("https://itunes.apple.com/cn/app/total-video-converter-pro-dvd/id426654691?mt=12"));
    }
    else
    {
        QDesktopServices::openUrl(QUrl("https://itunes.apple.com/app/id426654691"));
    }
}

void MainWindow::on_actionGet_Total_Video_Player_triggered()
{
    QString language = QLocale::system().name();
    if (language == "zh_CN")
    {
        QDesktopServices::openUrl(QUrl("https://itunes.apple.com/cn/app/total-video-player-freely-play-any-movie/id919402022?mt=12"));
    }
    else
    {
        QDesktopServices::openUrl(QUrl("https://itunes.apple.com/app/id919402022"));
    }
}

void MainWindow::on_actionTutorial_triggered()
{
    QDesktopServices::openUrl(QUrl(tr("http://www.macvideostudio.com/quickly-start-movie-video-editing-mac.html")));
}

void MainWindow::setCurrentFilterForVideoWidget(QObject* filter, QmlMetadata* meta)
{
    Mlt::GLWidget* videoWidget = static_cast<Mlt::GLWidget*>(&(MLT));
    QQmlContext *context = videoWidget->rootContext();
    context->setContextProperty("filter", filter);

    if (meta && QFile::exists(meta->vuiFilePath().toLocalFile())) {
        videoWidget->setSource(meta->vuiFilePath());
    } else {
        videoWidget->setBlankScene();
    }
}

void MainWindow::initParentDockForResourceDock()
{
    m_resourceDockContainer = new ContainerDock(TabPosition_Left, this);
    m_resourceDockContainer->setObjectName("ResourceDockContainer");//savestate needs to be used
    m_resourceDockContainer->setMinimumWidth(360);
    //m_resourceDockContainer->setMinimumHeight()
    addDockWidget(Qt::LeftDockWidgetArea, m_resourceDockContainer);
}


void MainWindow::initParentDockForPropteriesDock()
{
    m_propertiesDockContainer = new ContainerDock(TabPosition_Top, this);
    m_propertiesDockContainer->setObjectName("PropertiesDockContainer");//savestate needs to be used

    addDockWidget(Qt::RightDockWidgetArea, m_propertiesDockContainer);
}

void MainWindow::addResourceDock(QDockWidget *dock, QString tabButtonTitle, QIcon tabButtonNormalIcon, QIcon tabButtonAcitveIcon)
{
    Q_ASSERT(dock);
    dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    dock->setMinimumSize(300, 272);
    dock->setTitleBarWidget(new QWidget());

    m_resourceDockContainer->addDock(dock, tabButtonTitle, tabButtonNormalIcon, tabButtonAcitveIcon);
}

void MainWindow::addPropertiesDock(QDockWidget *dock, QString tabButtonTitle, QIcon tabButtonNormalIcon, QIcon tabButtonAcitveIcon)
{
    Q_ASSERT(dock);
    dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    dock->setMinimumSize(410, 272);
    dock->setTitleBarWidget(new QWidget());

    m_propertiesDockContainer->addDock(dock, tabButtonTitle, tabButtonNormalIcon, tabButtonAcitveIcon);
}

void MainWindow::onFileOpened(QString filePath)
{
    if (filePath.endsWith(".mlt"))
    {
        //拷贝到模板目录，并添加到模板管理界面
    }
    else
    {
//        RecentDock_add(filePath);
        RDG_AddFileToRecentDock(filePath);
#ifdef Q_OS_MAC
    create_security_bookmark(filePath.toUtf8().constData());
#endif
    }
}

void MainWindow::onOpenFailed(QString filePath)
{
//    RecentDock_remove(filePath);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    resizePlayer();
}

void MainWindow::resizePlayer()
{
    if (!m_player || !m_propertiesDockContainer || !m_resourceDockContainer || !m_timelineDock)
        return;
    QSize playerSize = m_player->videoSize();
    QSize propertiesDockSize = m_propertiesDockContainer->size();
    QSize resourceDockSize = m_resourceDockContainer->size();
    QSize timelineDockSize = m_timelineDock->size();

    if (fabs(playerSize.width()*1.0 / playerSize.height() - 16.0/9.0) < 0.05)
        return;

    if (playerSize.width()*1.0 / playerSize.height() > 16.0/9.0)
    {//太宽
        int newPlayerHeight = int(playerSize.width() * 9.0/16.0);
        int deltaH = newPlayerHeight - playerSize.height();
        int newTimelineHeight = timelineDockSize.height() - deltaH;
        if (newTimelineHeight >= m_timelineDock->minimumHeight())
        {//减小时间线高度
            resizeDocks({m_timelineDock}, {timelineDockSize.height() - deltaH}, Qt::Vertical);
        }
        else
        {//增加播放器左右两边dock的宽度
            int newPlayerWidth = int(playerSize.height() * 16.0/9.0);
            int deltaW = playerSize.width() - newPlayerWidth;
            resizeDocks({m_propertiesDockContainer}, {propertiesDockSize.width() + deltaW}, Qt::Horizontal);
        }
    }
    else
    {// 太窄
        int newPlayerWidth = int(playerSize.height() * 16.0/9.0);
        int deltaW = newPlayerWidth - playerSize.width();
        int extraResourceDockWidth = resourceDockSize.width() - m_resourceDockContainer->minimumWidth();
        int extraPropertiesDockWidth = propertiesDockSize.width() - m_propertiesDockContainer->minimumWidth();
        if (deltaW <= extraResourceDockWidth + extraPropertiesDockWidth)
        {//减小播放器左右两边宽度
            if (deltaW <= extraResourceDockWidth)
                resizeDocks({m_resourceDockContainer}, {resourceDockSize.width() - deltaW}, Qt::Horizontal);
            else
            {
                int remainedDeltaW = deltaW - extraResourceDockWidth;
                resizeDocks(
                            {m_resourceDockContainer, m_propertiesDockContainer},
                            {resourceDockSize.width() - extraResourceDockWidth, propertiesDockSize.width() - remainedDeltaW},
                            Qt::Horizontal
                            );
            }
        }
        else
        {//增加时间线高度
            int newPlayerHeight = int(playerSize.width() * 9.0/16.0);
            int deltaH = playerSize.height() - newPlayerHeight;
            resizeDocks({m_timelineDock}, {timelineDockSize.height()+deltaH}, Qt::Vertical);
        }
    }

}


void MainWindow::addPlayer()
{
    ui->centralWidget->layout()->addWidget(m_player);
}



void MainWindow::onExportTemplate()
{
    QString templatePath = Util::templatePath();
    QString sampleFile = QString("%1/Samples/1.png").arg(templatePath);

    Mlt::Tractor *tractor = m_timelineDock->model()->tractor();
    QString xml = MLT.XML(tractor);




    //create a temporary tractor
    Mlt::Tractor *tempTractor = new Mlt::Tractor(MLT.profile(), const_cast<char*>("xml-string"), const_cast<char*>(xml.toUtf8().constData()));

    int trackCount = tempTractor->count();



    //replace clips
    for (int i = 0; i < trackCount; i++)
    {
        QScopedPointer<Mlt::Producer> track(tempTractor->track(i));
        if (track) {
            Mlt::Playlist playlist(*track);
            int clipCount = playlist.count();
            for (int j = 0; j < clipCount; j++)
            {
                QScopedPointer<Mlt::ClipInfo> originalInfo(playlist.clip_info(j));
                Mlt::Producer *originalProducer = originalInfo->producer;

                QString mltService(originalProducer->parent().get("mlt_service"));
                if (!mltService.isEmpty()
                        && mltService != "color"
                        && mltService != "colour"
                        && mltService != "blank"
                        && mltService != "tractor"
                        && mltService != "gifdec")
                {
                    Mlt::Producer *newProducer = new Mlt::Producer(MLT.profile(),
                                                                   sampleFile.toUtf8().constData());

                    MLT.getHash(*newProducer);
                    MLT.copyFilters(*originalProducer, *newProducer);
                    //set same in/out/length as original
                    newProducer->set("length", originalInfo->frame_count);
                    newProducer->set("in", originalInfo->frame_in);
                    newProducer->set("out", originalInfo->frame_out);

                    playlist.remove(j);
                    playlist.insert(*newProducer, j, originalInfo->frame_in, originalInfo->frame_out);
                }
            }

            //process transitions
            for (int j = 0; j < clipCount; j++)
            {
                QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(j));
                Mlt::Producer *producer = info->producer;

                QString mltService(producer->parent().get("mlt_service"));
                if (mltService == "tractor")
                {
                    QScopedPointer<Mlt::ClipInfo> preClipInfo(playlist.clip_info(j-1));
                    QScopedPointer<Mlt::ClipInfo> nextClipInfo(playlist.clip_info(j+1));
                    Mlt::Producer *preProducer = preClipInfo->producer;
                    Mlt::Producer *nextProducer = nextClipInfo->producer;
                    //newProducer->set_in_and_out(0, info->frame_count);
                    int duration = info->frame_count;
                    preProducer->set("length", preClipInfo->frame_count+duration);
                    preProducer->set("out", preClipInfo->frame_out+duration);
                    nextProducer->set("length", nextClipInfo->frame_count+duration);
                    nextProducer->set("in", nextClipInfo->frame_in-duration);

                    Mlt::Tractor tractor(producer->parent());
                    Mlt::Producer *aTrack = preProducer->cut(preClipInfo->frame_out + 1, preClipInfo->frame_out + duration);
                    Mlt::Producer *bTrack = nextProducer->cut(nextClipInfo->frame_in - duration, nextClipInfo->frame_in - 1);
                    tractor.set_track(*aTrack, 0);
                    tractor.set_track(*bTrack, 1);
                    //playlist.resize_clip(j-1, preClipInfo->frame_in, preClipInfo->frame_out - duration);
                    //playlist.resize_clip(j+1, nextClipInfo->frame_in+duration, nextClipInfo->frame_out);
                }
            }
        }
    }

    // get temp filename
    QString tmpTemplate = QString("%1/tmp_XXXXX").arg(templatePath);

    QTemporaryFile tmp(tmpTemplate);
    tmp.open();
    tmp.close();

    QString path = Settings.savePath();
    path.append("/untitled.mlt");
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Template"), path, tr("Template (*.mlt)"));
    if (!filename.isEmpty()) {
        QFileInfo fi(filename);
        Settings.setSavePath(fi.path());
        if (fi.suffix() != "mlt")
            filename += ".mlt";
        MLT.saveXML(tmp.fileName(), tempTractor, true);
        QFile::remove(filename);
        QFile::copy(tmp.fileName(), filename);
        QFile::remove(tmp.fileName());
    }
    delete tempTractor;
}

//加载模板信息
void MainWindow::loadTemplateInfo(Mlt::Producer *producer)
{
    Q_UNUSED(producer);
//    Q_ASSERT(producer);
//    if (!producer || !producer->is_valid())
//    {
//       m_templateEditor->setProducer(0);
//       return;
//    }
//    QString resource(producer->get("resource"));
//    QString service(producer->get("mlt_service"));
//    if (service == "xml" || service == "xml-string")
//        m_templateEditor->setProducer(producer);
//    else
//        m_templateEditor->setProducer(0);
}

void MainWindow::on_actionNewProject_triggered()
{
    on_actionClose_triggered();
}

void MainWindow::on_actionVideoMode_triggered()
{
    VideoModeSettingsDialog videoModeSettingsDialog(this);
    videoModeSettingsDialog.setWindowModality(QmlApplication::dialogModality());

    videoModeSettingsDialog.exec();
}

void MainWindow::onFiltersInfoLoaded()
{
    RDG_SetVideoFiltersInfo(m_filterController->getVideoFiltersInfo());
    RDG_SetAudioFiltersInfo(m_filterController->getAudioFiltersInfo());
}
