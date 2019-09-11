/*
 * Copyright (c) 2011-2015 Meltytech, LLC
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
class EncodeDock;
class JobsDock;
//class PlaylistDock;
class QUndoStack;
//class MeltedPlaylistDock;
//class MeltedServerDock;
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
class ContainerDock;
class QToolButton;
class TemplateEidtor;

//class TextlistDock;

//软件主窗口类
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow& singleton();
    ~MainWindow();

    //提示是否继续编辑、保存
    bool continueModified();
    //提示尚有未完成的任务，是否继续退出
    bool continueJobsRunning();
    //获取当前的undostack
    QUndoStack* undoStack() const;
    //改变软件主界面的主题，当前支持黑色和白色
    static void changeTheme(const QString& theme);
    //获取playlistDock，旧的文件列表，目前已弃用
//    PlaylistDock* playlistDock() const { return m_playlistDock; }
    //获取FilterController
    FilterController* filterController() const { return m_filterController; }
//    HtmlEditor* htmlEditor() const { return m_htmlEditor; }
    //获取playlist，空函数，无用
    Mlt::Playlist* playlist() const;
    //获取当前整个时间线对象，即Tractor
    Mlt::Producer* multitrack() const;
    //自动保存工程文件
    void doAutosave();
    //设置主窗口全屏
    void setFullScreen(bool isFullScreen);
    //获取无标题的工程文件名
    QString untitledFileName() const;
    //获取path对应的文件的md5码
    QString getFileHash(const QString& path) const;
    //设置软件的profile为profile_name
    void setProfile(const QString& profile_name);

    //重写QMainWindow的键盘按下事件，处理主界面的键盘按下事件
    void keyPressEvent(QKeyEvent*);
    //重写QMainWindow的键盘松开事件，处理主界面的键盘松开按键事件
    void keyReleaseEvent(QKeyEvent *);

    //高亮导出文件列表中的第index条任务
    void highlightCurrentJobs(int index);
    //创建主界面弹出菜单，空函数，无用
    QMenu *createPopupMenu();

    //获取时间线dock对象
    TimelineDock* timelineDock() const {return m_timelineDock;}
    //获取播放器player对象
    Player *player() const {return m_player;}
    //获取导出文件dock
    EncodeDock* encodeDock() const {return m_encodeDock;}

    //设置整个时间线tractor为当前的producer
    Q_INVOKABLE void setMultitrackAsCurrentProducer();

    //获取模板编辑界面对象TemplateEidtor，目前无用
    TemplateEidtor *templateEditor() const { return m_templateEditor; }

    //修改profile
    void changeProfile(QString strProfileName);

signals:
    //打开producer时发送此信号
    void producerOpened();
    //软件profile发生改变时发出此信号
    void profileChanged();
    //打开文件失败时发出此信号，暂时未用
    void openFailed(QString);
    //关闭程序时发送此信号，未使用
    void aboutToShutDown();
    //隐藏加载文件进度对话框，打开文件时发送此信号
    void hideProgressDialog();

protected:
    MainWindow();
    //重写基类的相关拖拽事件，处理主界面的拖拽事件
    bool eventFilter(QObject* target, QEvent* event);
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);
    //重写基类的窗口的关闭、显示、调节大小事件，处理主界面的对应事件
    void closeEvent(QCloseEvent*);
    void showEvent(QShowEvent*);
    void resizeEvent(QResizeEvent* event);

private:
    //设置程序的设置菜单
    void setupSettingsMenu();
    //用一个profile信息，创建一个QAction；用于改profile菜单添加action
    QAction *addProfile(QActionGroup* actionGroup, const QString& desc, const QString& name);
    //读取Settings中保存的播放器设置，并设置到界面
    void readPlayerSettings();
    //读取Settings中保存的主窗口设置，并设置到界面
    void readWindowSettings();
    //保存窗口的设置到Settings对象中
    void writeSettings();
    //配置显示视频的widget，即预览视频的widget
    void configureVideoWidget();
    //设置当前主窗口左上边标题栏中显示的工程文件名为filename
    //命名修改——做了不止一件事：设置当前的文件名；设置窗口的标题，因该分成两个函数setCurrentFile和setWindowTitle
    void setCurrentFile(const QString &filename);
    //改变当前播放及consumer的扫描方法
    void changeDeinterlacer(bool checked, const char* method);
    //改变插值方法
    void changeInterpolation(bool checked, const char* method);
    //检测url对应文件是否已经存在自动保存文件
    bool checkAutoSave(QString &url);
    //使播放进度向后退sec秒
    void stepLeftBySeconds(int sec);
    //保存修复xml文件
    bool saveRepairedXmlFile(MltXmlChecker& checker, QString& fileName);

    Ui::MainWindow* ui;//主窗口UI
    Player* m_player;//播放器
    QDockWidget* m_propertiesDock;//属性dock，目前已经弃用
    QDockWidget* m_simplePropertiesDock;//右侧属性dock中的clip属性dock
    QDockWidget* m_resourceRecentDock;//左侧文件列表dock
    QDockWidget* m_resourceVideoFilterDock;//左侧视频滤镜列表dock
    QDockWidget* m_resourceAudioFilterDock;//左侧音频滤镜列表dock
    QDockWidget* m_templateDock;//左侧模板dock，暂时无用
    QDockWidget* m_resourceTextDock;//文字Dock
    QDockWidget* m_resourceStickerDock;//文字Dock
//    QDockWidget* m_templateEditorDock;//右侧模板编辑dock，暂时无用
    EncodeDock* m_encodeDock;//导出文件dock
    JobsDock* m_jobsDock;//导出文件进度列表dock
//    PlaylistDock* m_playlistDock;//旧的文件列表dock，暂时无用
    TimelineDock* m_timelineDock;//底下时间线dock
    QString m_currentFile;//当前的工程文件名
    bool m_isKKeyPressed;//是否按键按下
    QUndoStack* m_undoStack;//undo、redo栈
    QDockWidget* m_historyDock;
//    MeltedServerDock* m_meltedServerDock;
//    MeltedPlaylistDock* m_meltedPlaylistDock;
    QActionGroup* m_profileGroup;//profile所有菜单的操作
    QActionGroup* m_externalGroup;
    QActionGroup* m_keyerGroup;
    FiltersDock* m_propertiesVideoFilterDock;//右侧属性中的视频滤镜分类dock，即显示已添加的视频滤镜。
    FiltersDock* m_propertiesAudioFilterDock;//右侧属性中的音频滤镜分类dock，即显示已添加的音频滤镜。
    FilterController* m_filterController;
    ScopeController* m_scopeController;
    QMenu* m_customProfileMenu;//自定义profil菜单
    QMenu* m_keyerMenu;
    QStringList m_multipleFiles;//多个文件的路径，选中多个文件打开时
    bool m_isPlaylistLoaded;
    QActionGroup* m_languagesGroup;//语言设置菜单的所有操作
//    HtmlEditor* m_htmlEditor;
    AutoSaveFile* m_autosaveFile;//自动保存工程文件对象
    QMutex m_autosaveMutex;//自动保存互斥锁
    QTimer m_autosaveTimer;//自动保存定时器
    int m_exitCode;//程序退出码，可查看是否异常退出
    int m_navigationPosition;
    QScopedPointer<QAction> m_statusBarAction;
//    QNetworkAccessManager m_network;

    //以下没有用到
    //xjp add for managing resource
    QTabWidget  *m_tabForResource;
    QQuickWidget *m_quickWidgetForMedia;
    QQuickWidget *m_quickWidgetForFilter;
    QQuickWidget *m_quickWidgetForText;
    QQuickWidget *m_quickWidgetForSoundEffect;
    QQuickWidget *m_quickWidgetForVoice;
    FilterWidget *m_filterWidget;
    TextManagerWidget *m_textManagerWidget;

    //以下是工具栏上的按钮
    QToolButton *m_addButton;
//    QToolButton *m_removeButton;
    QToolButton *m_undoButton;
    QToolButton *m_redoButton;
    QToolButton *m_saveButton;
    QToolButton *m_exportButton;
    QToolButton *m_emailButton;
    QToolButton *m_forumButton;
    QToolButton *m_helpButton;
    QToolButton *m_activateButton;
    QToolButton *m_buynowButton;
    QToolButton *m_upgradeButton;
    QToolButton *m_tvcProButton;
    QToolButton *m_exportTemplateButton;

    //暂时未用到
    ResourceButtonDockWidget *m_resourceBtnDock;

    bool m_bLeftMouseBtnDown;//鼠标左键按下标记
    QPoint m_ptLeftMouseDown;//鼠标左键按下位置

    //注册及注册提示对话框
    RegistrationTipsDialog *m_registrationTipsDialog;
    RegistrationDialog *m_registrationDialog;


    //pro版功能提示对话框
    ProFeaturePromptDialog *m_proFeaturePromptDialog;
    UpgradeToProPromptDialog *m_upgradeToProPromptDialog;
#ifdef MOVIEMATOR_PRO
#ifndef SHARE_VERSION
    InvalidProjectDialog *m_invalidProjectDiaog;//无效的工程提示对话框
#endif
#endif



    MainController& m_mainController;

#ifdef WITH_LIBLEAP
    LeapListener m_leapListener;
#endif

    EncodeTaskDock *m_tasksDock;//导出文件dock

    ContainerDock *m_resourceDockContainer;//包裹左侧所有dock的容器dock
    ContainerDock *m_propertiesDockContainer;//包裹右侧所有dock的容器dock

    TemplateEidtor *m_templateEditor;//暂时无用

public slots:
    void open(Mlt::Producer* producer);//打开producer

    //检测当前的gpu模式
    bool isCompatibleWithGpuMode(MltXmlChecker& checker);
    bool isXmlRepaired(MltXmlChecker& checker, QString& fileName);//修改工程文件xml
    void updateAutoSave();//开启自动保存工程文件
    void open(QString url, const Mlt::Properties* = nullptr);//打开文件
    void open1(QString url, const Mlt::Properties * = nullptr);
    void openVideo();//打开多选文件
    void openCut(Mlt::Producer* producer);//打开producer
    void showStatusMessage(QAction* action, int timeoutSeconds = 5);//显示状态信息
    void showStatusMessage(const QString& message, int timeoutSeconds = 5);
    void removeVideo();//删除文件列表中的选中项媒体资源
    void seekPlaylist(int start);//跳到当前预览视频的start位置
    void seekTimeline(int position);//跳到当前预览的clip在时间线上的position位置
    QWidget* loadProducerWidget(Mlt::Producer* producer);//创建所有的producer widget，如producer属性控件avformatsampleproducerwidget
    void onProducerOpened();//打开producer后，设置属性dock、播放器、时间线相关
    void onGpuNotSupported();//提示gup不支持
    void editHTML(const QString& fileName);//进入html编辑界面，空函数，暂时无用
    void stepLeftOneFrame();//跳到当前预览视频的当前位置的前一帧
    void stepRightOneFrame();//跳到当前预览视频的当前位置的后一帧
    void stepLeftOneSecond();//跳到当前预览视频的当前时间点的前一秒
    void stepRightOneSecond();//跳到当前预览视频的当前时间点的后一秒
    void setInToCurrent(bool ripple);
    void setOutToCurrent(bool ripple);
    void onShuttle(float x);
    void createMultitrackModelIfNeeded();//如果时间线对应的tractor不存在则创建

    void showFilterDock();//显示右侧属性中已添加的视频滤镜dock，目前已弃用
    void showAudioFilterDock();//显示右侧属性中已添加的音频滤镜dock，目前已弃用

    void onJobsDockTriggered(bool);//显示导出文件进度列表dock
    void onPropertiesDockTriggered(bool checked = true);//显示属性面板
    void showPropertiesDock();//显示clip的属性dock

    void setPauseAfterOpen(bool pause);
    void appendClipToPlaylist();//空函数，无用

    void appendTextToTimeline(int index);//空函数，无用
    void onTextAddToTimeline(void *producer);//添加一个textproducer到时间线
    void showCurrentTextSettingWidget(Mlt::Producer *textProcucer);//空函数，无用

    void showPlaylistDock();//空函数无用
    void showRecentDock();//显示左侧文件列表dock
    void onShowPropertiesVideoFilterDock();//显示右侧属性中已添加的视频滤镜dock
    void onShowPropertiesAudioFilterDock();//显示右侧属性中已添加的音频滤镜的dock

    void setCurrentTrack(int trackIndex);//设置时间线上当前的track为第trackIndex个

    void openFiles(const QStringList &list);//打开媒体资源文件

    void showLoadProgress();//显示加载文件进度，未使用

    int showRegistrationTipDialog();//显示注册提示窗口
    void onBuynowButton_clicked();
    void onActivateButton_clicked();

    void updateWindowTitle();//更新窗口标题（更新显示的工程文件名）

    void showProFeaturePromptDialog();//显示pro版功能提示
    void showUpgradeToProPromptDialog();//显示升级pro版提示

    void upgradeToProVersion();//打开pro版网页


    void saveXML(const QString& filename, bool withRelativePaths = true);//保存工程文件xml

    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionExport_selected_clip_as_template_file_triggered();

    void on_tasksDockTriggered(bool);

    //接收filtersInfo已加载的消息
    void onFiltersInfoLoaded();

#ifdef MOVIEMATOR_PRO
#ifndef SHARE_VERSION
    void showInvalidProjectDialog();
#endif
#endif


    void setCurrentFilterForVideoWidget(QObject* filter, QmlMetadata* meta);//设置当前的filter的vui.qml到视频播放widget，目前只有text、sizeAndPosition滤镜需要
    void pushCommand(QUndoCommand *command);//添加一条命令到undostack

    void onFileOpened(QString filePath);//添加文件到文件里列表dock
    void onOpenFailed(QString filePath);//打开文件失败


    void resizePlayer();//调节播放器大小

    void addPlayer();//添加播放器到主界面

    void loadTemplateInfo(Mlt::Producer *producer);//空函数，暂时无用

private slots:


    void on_actionAbout_TVE_triggered();
    void on_actionOpenOther_triggered();
    void onProducerChanged();
    bool on_actionSave_triggered();
    bool on_actionSave_As_triggered();
    void onEncodeTriggered(bool checked = true);//导出视频
    void onCaptureStateChanged(bool started);
    void on_actionFacebook_triggered();
    void on_actionHomePage_triggered();

    void onRecentDockTriggered(bool checked = true);//无用函数

    void onPlaylistDockTriggered(bool checked = true);//无用函数
    void onTimelineDockTriggered(bool checked = true);
    void onHistoryDockTriggered(bool checked = true);//无用函数
    void onFiltersDockTriggered(bool checked = true);//无用函数

    void onPlaylistCreated();//暂未用到
    void onPlaylistLoaded();//暂未用到
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
    void on_actionGammaSRGB_triggered(bool checked);//设置播放预览的gamma为iec61966_2_1
    void on_actionGammaRec709_triggered(bool checked);//设置播放预览的gamma为bt709
    void onFocusChanged(QWidget *old, QWidget * now) const;
    void onFocusObjectChanged(QObject *obj) const;
    void onFocusWindowChanged(QWindow *window) const;
    void onTimelineClipSelected();
    void onAddAllToTimeline(Mlt::Playlist* playlist);//添加一个playlist到时间线
    void on_actionScrubAudio_triggered(bool checked);

    QToolButton *createToolButton(const QString& icon, const QString& iconPressed, const QString& iconDisabled, const QString& title, const QString& tooltip);
    void customizeToolbar();

    void configureUI();//配置主窗口

    void onExportTemplate();


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

    void onClearButtonForTest_clicked();
    void on_actionGet_Total_Video_Converter_Pro_triggered();
    void on_actionGet_Total_Video_Player_triggered();
    void on_actionTutorial_triggered();

    void initParentDockForResourceDock();//初始化左侧的容器dock
    void initParentDockForPropteriesDock();//初始化右侧的容器dock
    void addResourceDock(QDockWidget *dock, QString tabButtonTitle, QIcon tabButtonNormalIcon, QIcon tabButtonAcitveIcon);//添加dock到左侧的dock容器中
    void addPropertiesDock(QDockWidget *dock, QString tabButtonTitle, QIcon tabButtonNormalIcon, QIcon tabButtonAcitveIcon);//添加dock到右侧的dock容器中

    void on_actionNewProject_triggered();
    void on_actionVideoMode_triggered();
};

#define MAIN MainWindow::singleton()

#endif // MAINWINDOW_H
