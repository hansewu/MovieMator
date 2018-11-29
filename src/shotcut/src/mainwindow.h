/*
 * Copyright (c) 2011-2015 Meltytech, LLC
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include <QTimer>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QTabWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QQuickWidget>
#include "mltcontroller.h"
#include "mltxmlchecker.h"
#include "customtabwidget.h"
#include <QThread>
#include <QProgressDialog>



#define EXIT_RESTART (42)

namespace Ui {
    class MainWindow;
}
class Player;
class RecentDock;
class EncodeDock;
class JobsDock;
class PlaylistDock;
class QUndoStack;
class MeltedPlaylistDock;
class MeltedServerDock;
class QActionGroup;
class FilterController;
class ScopeController;
class FiltersDock;
//class HtmlEditor;
class TimelineDock;
class AutoSaveFile;
class QNetworkReply;
class FilterWidget;
class QmlMetadata;
class TextManagerWidget;
class ResourceButtonDockWidget;
class RegistrationTipsDialog;
class RegistrationDialog;
class ProFeaturePromptDialog;
class UpgradeToProPromptDialog;
class MainController;
class InvalidProjectDialog;
class EncodeTaskDock;
class QmlFilter;
class QUndoCommand;

//class TextlistDock;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow& singleton();
    ~MainWindow();

    bool continueModified();
    bool continueJobsRunning();
    QUndoStack* undoStack() const;
    static void changeTheme(const QString& theme);
    PlaylistDock* playlistDock() const { return m_playlistDock; }
    FilterController* filterController() const { return m_filterController; }
//    HtmlEditor* htmlEditor() const { return m_htmlEditor; }
    Mlt::Playlist* playlist() const;
    Mlt::Producer* multitrack() const;
    void doAutosave();
    void setFullScreen(bool isFullScreen);
    QString removeFileScheme(QUrl& url);
    QString untitledFileName() const;
    QString getFileHash(const QString& path) const;
    void setProfile(const QString& profile_name);

    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent *);

    void highlightCurrentJobs(int index);
    QMenu *createPopupMenu();

    TimelineDock* timelineDock() const {return m_timelineDock;}
    Player *player() const {return m_player;}
    EncodeDock* encodeDock() const {return m_encodeDock;}




signals:
    void producerOpened();
    void profileChanged();
    void openFailed(QString);
    void aboutToShutDown();
    void hideProgressDialog();

protected:
    MainWindow();
    bool eventFilter(QObject* target, QEvent* event);
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);
    void closeEvent(QCloseEvent*);
    void showEvent(QShowEvent*);

private:
    void setupSettingsMenu();
    QAction *addProfile(QActionGroup* actionGroup, const QString& desc, const QString& name);
    void readPlayerSettings();
    void readWindowSettings();
    void writeSettings();
    void configureVideoWidget();
    void setCurrentFile(const QString &filename);
    void changeDeinterlacer(bool checked, const char* method);
    void changeInterpolation(bool checked, const char* method);
    bool checkAutoSave(QString &url);
    void stepLeftBySeconds(int sec);
    bool saveRepairedXmlFile(MltXmlChecker& checker, QString& fileName);

    Ui::MainWindow* ui;
    Player* m_player;
    QDockWidget* m_propertiesDock;
    RecentDock* m_recentDock;
    EncodeDock* m_encodeDock;
    JobsDock* m_jobsDock;
    PlaylistDock* m_playlistDock;
    TimelineDock* m_timelineDock;
    QString m_currentFile;
    bool m_isKKeyPressed;
    QUndoStack* m_undoStack;
    QDockWidget* m_historyDock;
    MeltedServerDock* m_meltedServerDock;
    MeltedPlaylistDock* m_meltedPlaylistDock;
    QActionGroup* m_profileGroup;
    QActionGroup* m_externalGroup;
    QActionGroup* m_keyerGroup;
    FiltersDock* m_filtersDock;
    FilterController* m_filterController;
    ScopeController* m_scopeController;
    QMenu* m_customProfileMenu;
    QMenu* m_keyerMenu;
    QStringList m_multipleFiles;
    bool m_isPlaylistLoaded;
    QActionGroup* m_languagesGroup;
//    HtmlEditor* m_htmlEditor;
    AutoSaveFile* m_autosaveFile;
    QMutex m_autosaveMutex;
    QTimer m_autosaveTimer;
    int m_exitCode;
    int m_navigationPosition;
    QScopedPointer<QAction> m_statusBarAction;
//    QNetworkAccessManager m_network;

    //xjp add for managing resource
    QTabWidget  *m_tabForResource;
    QQuickWidget *m_quickWidgetForMedia;
    QQuickWidget *m_quickWidgetForFilter;
    QQuickWidget *m_quickWidgetForText;
    QQuickWidget *m_quickWidgetForSoundEffect;
    QQuickWidget *m_quickWidgetForVoice;
    FilterWidget *m_filterWidget;
    QDockWidget *m_mainDockWidget;
    QMainWindow *m_mainWindowForDockWidgets;

    TextManagerWidget *m_textManagerWidget;
//    TextlistDock *m_textlistDock;
    QDockWidget *m_textDock;

    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_undoButton;
    QPushButton *m_redoButton;
    QPushButton *m_saveButton;
    QPushButton *m_exportButton;
    QPushButton *m_emailButton;
    QPushButton *m_forumButton;
    QPushButton *m_helpButton;

    ResourceButtonDockWidget *m_resourceBtnDock;

    bool m_bLeftMouseBtnDown;
    QPoint m_ptLeftMouseDown;

    RegistrationTipsDialog *m_registrationTipsDialog;
    RegistrationDialog *m_registrationDialog;
    QPushButton *m_activateButton;
    QPushButton *m_buynowButton;

    ProFeaturePromptDialog *m_proFeaturePromptDialog;
    UpgradeToProPromptDialog *m_upgradeToProPromptDialog;
#ifdef MOVIEMATOR_PRO
#ifndef SHARE_VERSION
    InvalidProjectDialog *m_invalidProjectDiaog;
#endif
#endif

    QPushButton *m_upgradeButton;
    QPushButton *m_tvcProButton;


    MainController& m_mainController;

#ifdef WITH_LIBLEAP
    LeapListener m_leapListener;
#endif

    EncodeTaskDock *m_tasksDock;

public slots:
    void open(Mlt::Producer* producer);

    bool isCompatibleWithGpuMode(MltXmlChecker& checker);
    bool isXmlRepaired(MltXmlChecker& checker, QString& fileName);
    void updateAutoSave();
    void open(QString url, const Mlt::Properties* = 0);
    void open1(QString url, const Mlt::Properties * = 0);
    void openVideo();
    void openCut(Mlt::Producer* producer);
    void showStatusMessage(QAction* action, int timeoutSeconds = 5);
    void showStatusMessage(const QString& message, int timeoutSeconds = 5);
    void removeVideo();
    void seekPlaylist(int start);
    void seekTimeline(int position);
    QWidget* loadProducerWidget(Mlt::Producer* producer);
    void onProducerOpened();
    void onGpuNotSupported();
    void editHTML(const QString& fileName);
    void stepLeftOneFrame();
    void stepRightOneFrame();
    void stepLeftOneSecond();
    void stepRightOneSecond();
    void setInToCurrent(bool ripple);
    void setOutToCurrent(bool ripple);
    void onShuttle(float x);
    void createMultitrackModelIfNeeded();

    void showFilterDock();

    void onJobsDockTriggered(bool);
    void onPropertiesDockTriggered(bool checked = true);

    void setPauseAfterOpen(bool pause);
    void appendClipToPlaylist();

    void appendTextToTimeline(int index);
    void onTextAddToTimeline(void *producer);
    void showCurrentTextSettingWidget(Mlt::Producer *textProcucer);

    void showPlaylistDock();
    void showRecentDock();
    void onShowFilterDock();

    void setCurrentTrack(int trackIndex);

    void addToRecentDock(const QString &url);


    void openFiles(const QStringList &list);

    void showLoadProgress();

    int showRegistrationTipDialog();
    void on_buynowButton_clicked();
    void on_activateButton_clicked();

    void updateWindowTitle();

    void showProFeaturePromptDialog();
    void showUpgradeToProPromptDialog();

    void upgradeToProVersion();


    void saveXML(const QString& filename, bool withRelativePaths = true);

    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();

    void on_tasksDockTriggered(bool);

#ifdef MOVIEMATOR_PRO
#ifndef SHARE_VERSION
    void showInvalidProjectDialog();
#endif
#endif


    void setCurrentFilterForVideoWidget(QmlFilter* filter, QmlMetadata* meta);
    void pushCommand(QUndoCommand *command);


private slots:


    void on_actionAbout_TVE_triggered();
    void on_actionOpenOther_triggered();
    void onProducerChanged();
    bool on_actionSave_triggered();
    bool on_actionSave_As_triggered();
    void onEncodeTriggered(bool checked = true);
    void onCaptureStateChanged(bool started);
    void on_actionFacebook_triggered();
    void on_actionHomePage_triggered();

    void onRecentDockTriggered(bool checked = true);

    void onPlaylistDockTriggered(bool checked = true);
    void onTimelineDockTriggered(bool checked = true);
    void onHistoryDockTriggered(bool checked = true);
    void onFiltersDockTriggered(bool checked = true);

    void onPlaylistCreated();
    void onPlaylistLoaded();
    void onPlaylistCleared();
    void onPlaylistClosed();
    void onPlaylistModified();
    void onMultitrackCreated();
    void onMultitrackClosed();
    void onMultitrackModified();
    void onMultitrackDurationChanged();
    void onCutModified();
    void onFilterModelChanged();
    void updateMarkers();
    void updateThumbnails();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionFAQ_triggered();
    void on_actionForum_triggered();
    void onMeltedUnitOpened();
    void onMeltedUnitActivated();
    void on_actionEnter_Full_Screen_triggered();
    void on_actionRealtime_triggered(bool checked);
    void on_actionProgressive_triggered(bool checked);
    void on_actionOneField_triggered(bool checked);
    void on_actionLinearBlend_triggered(bool checked);
    void on_actionYadifTemporal_triggered(bool checked);
    void on_actionYadifSpatial_triggered(bool checked);
    void on_actionNearest_triggered(bool checked);
    void on_actionBilinear_triggered(bool checked);
    void on_actionBicubic_triggered(bool checked);
    void on_actionHyper_triggered(bool checked);
    void on_actionJack_triggered(bool checked);
    void on_actionGPU_triggered(bool checked);
    void onExternalTriggered(QAction*);
    void onKeyerTriggered(QAction*);
    void onProfileTriggered(QAction*);
    void onProfileChanged();
    void on_actionAddCustomProfile_triggered();
    void processMultipleFiles();
    void onLanguageTriggered(QAction*);
    void on_actionSystemTheme_triggered();
    void on_actionFusionDark_triggered();
    void on_actionFusionLight_triggered();
    void on_actionTutorials_triggered();
    void on_actionRestoreLayout_triggered();
    void on_actionShowTitleBars_triggered(bool checked);
    void on_actionShowToolbar_triggered(bool checked);
    void onToolbarVisibilityChanged(bool visible);
    void on_menuExternal_aboutToShow();
    void on_actionUpgrade_triggered();
    void on_actionOpenXML_triggered();
    void onAutosaveTimeout();
    void on_actionGammaSRGB_triggered(bool checked);
    void on_actionGammaRec709_triggered(bool checked);
    void onFocusChanged(QWidget *old, QWidget * now) const;
    void onFocusObjectChanged(QObject *obj) const;
    void onFocusWindowChanged(QWindow *window) const;
    void onTimelineClipSelected();
    void onAddAllToTimeline(Mlt::Playlist* playlist);
    void on_actionScrubAudio_triggered(bool checked);
    void customizeToolbar();

    void configureUI();


#ifdef Q_OS_WIN
    void onDrawingMethodTriggered(QAction*);
#endif
    void on_actionApplicationLog_triggered();
    void on_actionClose_triggered();
    void onPlayerTabIndexChanged(int index);
    void onUpgradeCheckFinished(QNetworkReply* reply);
    void onUpgradeTriggered();
    void onTimelineSelectionChanged();

    void onClipCopied();
    void on_actionExportEDL_triggered();

    void onEmail_triggered();

    void onFilterAddToTimeline(void *producer);

    void onHelpButtonTriggered();


    void on_actionBuy_a_License_Code_triggered();
    void on_actionEnter_License_Code_triggered();

    void on_actionExecute_Python_triggered();

    void on_clearButtonForTest_clicked();
    void on_actionGet_Total_Video_Converter_Pro_triggered();
    void on_actionGet_Total_Video_Player_triggered();
    void on_actionTutorial_triggered();
};

#define MAIN MainWindow::singleton()

#endif // MAINWINDOW_H
