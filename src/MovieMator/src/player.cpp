/*
 * Copyright (c) 2012-2016 Meltytech, LLC
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

#include "player.h"
#include "scrubbar.h"
#include "mainwindow.h"
#include "widgets/timespinbox.h"
#include "widgets/audioscale.h"
#include "settings.h"
#include "util.h"
#include <QtWidgets>

#define VOLUME_KNEE (88)
#define SEEK_INACTIVE (-1)
#define VOLUME_SLIDER_HEIGHT (300)

static const int STATUS_ANIMATION_MS = 350;

//Player::Player(QWidget *parent)
//    : QWidget(parent)
Player::Player(QWidget *parent)
    : QWidget(parent)
    , m_position(0)
    , m_isMeltedPlaying(-1)
    , m_zoomToggleFactor(Settings.playerZoom() == 0.0f? 1.0f : Settings.playerZoom())
    , m_pauseAfterOpen(false)
    , m_monitorScreen(-1)
    , m_currentTransport(nullptr)
{
//    this->setFrameStyle(QFrame::Box | QFrame::Raised);
//    this->setLineWidth(2);


  //  this->setBackgroundRole(QPalette::ColorRole::Dark);

    m_bPlayedOrStoped = false;
    setObjectName("Player");
    Mlt::Controller::singleton();
    setupActions(this);
    m_playIcon = actionPlay->icon();
    m_pauseIcon = actionPause->icon();

    //setMinimumSize(480, 360);
    //setFixedWidth(800);


    // Create a layout.
//    QVBoxLayout *vlayout = new QVBoxLayout(this);
//    vlayout->setObjectName("playerLayout");
//    vlayout->setContentsMargins(0, 0, 0, 0);
//    vlayout->setSpacing(0);

    // Add tab bar to indicate/select what is playing: clip, playlist, timeline.
    m_tabs = new QTabBar;
    m_tabs->setShape(QTabBar::RoundedSouth);

    QHBoxLayout* tabLayout = new QHBoxLayout;
    tabLayout->setSpacing(0);
    tabLayout->addWidget(m_tabs);
    connect(m_tabs, SIGNAL(tabBarClicked(int)), SLOT(onTabBarClicked(int)));

    // Add status bar.
    m_statusLabel = new QPushButton;
    m_statusLabel->setFlat(true);
    m_statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    Util::setColorsToHighlight(m_statusLabel, QPalette::Button);
    tabLayout->addWidget(m_statusLabel);
    tabLayout->addStretch(10);
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
    m_statusLabel->setGraphicsEffect(effect);
    m_statusFadeIn = new QPropertyAnimation(effect, "opacity", this);
    m_statusFadeIn->setDuration(STATUS_ANIMATION_MS);
    m_statusFadeIn->setStartValue(0);
    m_statusFadeIn->setEndValue(1);
    m_statusFadeIn->setEasingCurve(QEasingCurve::InBack);
    m_statusFadeOut = new QPropertyAnimation(effect, "opacity", this);
    m_statusFadeOut->setDuration(STATUS_ANIMATION_MS);
    m_statusFadeOut->setStartValue(0);
    m_statusFadeOut->setEndValue(0);
    m_statusFadeOut->setEasingCurve(QEasingCurve::OutBack);
    m_statusTimer.setSingleShot(true);
    connect(&m_statusTimer, SIGNAL(timeout()), m_statusFadeOut, SLOT(start()));
    connect(m_statusFadeOut, SIGNAL(finished()), SLOT(onFadeOutFinished()));
    m_statusFadeOut->start();

    // Add the layouts for managing video view, scroll bars, and audio controls.
//    m_videoLayout = new QHBoxLayout;
//    m_videoLayout->setSpacing(0);
//    m_videoLayout->setContentsMargins(0, 0, 0, 0);
//    vlayout->addLayout(m_videoLayout, 10);
//    vlayout->addStretch();
    m_videoScrollWidget = new QWidget;
//    m_videoLayout->addWidget(m_videoScrollWidget, 10);
//    m_videoLayout->addStretch();
    QGridLayout* glayout = new QGridLayout(m_videoScrollWidget);
    glayout->setSpacing(0);
    glayout->setContentsMargins(0, 0, 0, 0);
    // Add the video widgets.
    m_videoWidget = qobject_cast<QWidget*>(MLT.videoWidget());
    Q_ASSERT(m_videoWidget);

    //16:9显示
    m_videoWidget->setMinimumSize(QSize(480, 270));

    glayout->addWidget(m_videoWidget, 0, 0);

    m_verticalScroll = new QScrollBar(Qt::Vertical);

    glayout->addWidget(m_verticalScroll, 0, 1);
    m_verticalScroll->hide();
    m_horizontalScroll = new QScrollBar(Qt::Horizontal);

    glayout->addWidget(m_horizontalScroll, 1, 0);
    m_horizontalScroll->hide();
#ifdef Q_OS_WIN
    // Workaround video not showing on Windows with Qt 5.5 upgrade.
    m_videoWidget->hide();
#endif

    // Add the scrub bar.
    m_scrubber = new ScrubBar();
    m_scrubber->setOrientation(Qt::Horizontal);
    m_scrubber->setFocusPolicy(Qt::NoFocus);
    m_scrubber->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
//    m_scrubber->setFixedHeight(8);


    m_positionSpinner = new TimeSpinBox(this);
    m_positionSpinner->setFixedSize(150, 33);
    m_positionSpinner->setFontSize(21);

    m_positionSpinner->setToolTip(tr("Current position"));
    m_positionSpinner->setEnabled(false);
    m_positionSpinner->setKeyboardTracking(false);
    m_positionSpinner->setStyleSheet("QSpinBox {background-color: rgb(127,127,127);color:rgb(255,255,255);border-image:url()}");

    //add progressWidget
    QWidget *progressWidget = new QWidget;
    progressWidget->setStyleSheet(".QWidget{background-color:rgb(52,52,52);border-radius:0px}");
    progressWidget->setContentsMargins(0,0,0,0);

    QHBoxLayout *hBoxLayout = new QHBoxLayout ;
    hBoxLayout->setContentsMargins(0,0,0,0);
    hBoxLayout->setSpacing(20);


    m_durationLabel = new QLabel(this);
    m_durationLabel->setToolTip(tr("Total Duration"));
    m_durationLabel->setText(QString("<h4><font color=white>%1</font></h4>").arg("00:00:00:00"));
    m_durationLabel->setFixedWidth(96);

    hBoxLayout->addWidget(m_scrubber);
    hBoxLayout->addWidget(m_durationLabel);
    hBoxLayout->setAlignment(Qt::AlignVCenter);

    progressWidget->setLayout(hBoxLayout);

//    vlayout->addLayout(hBoxLayout);
//    vlayout->addWidget(progressWidget);

    m_fitButton = new QPushButton();
    m_fullScreenButton = new QPushButton();
    m_zoomSetButton = new QPushButton();

    m_fitButton->setFixedSize(QSize(20, 20));
    m_fitButton->setIconSize(QSize(20, 20));
    m_fitButton->setFlat(true);
    m_fullScreenButton->setFixedSize(QSize(20, 20));
    m_fullScreenButton->setIconSize(QSize(20, 20));
    m_fullScreenButton->setFlat(true);
#ifdef Q_OS_WIN
    m_fullScreenButton->setVisible(false);
    m_fullScreenButton->setEnabled(false);
#endif
    m_zoomSetButton->setFixedSize(QSize(20, 20));
    m_zoomSetButton->setIconSize(QSize(20, 20));
    m_zoomSetButton->setFlat(true);

    m_zoomMenu = new QMenu(this);
    m_zoomMenu->setStyleSheet("QMenu{background-color:rgb(82,82,82);border:none;}"
                              "QMenu::item:selected{background-color:rgb(192,72,44);}");
    m_zoomMenu->setWindowFlags(Qt::Popup | Qt::NoDropShadowWindowHint); // 消除阴影

    const int size = 5;
    QString zoomTitle[size] = {"10%", "25%", "50%", "100%", "200%"};
    QActionGroup *actionGroup = new QActionGroup(this);
    for(int i=0; i<size; i++)
    {
        QAction *action = new QAction(this);
        action->setText(zoomTitle[i]);
        action->setObjectName("actionZoom_" + QString::number(i));
        action->setCheckable(true);
        action->setChecked(false);
        actionGroup->addAction(action);     // 互斥
        m_zoomMenu->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(onZoomActionTriggered()));
    }

//    QString buttonStyle = "QPushButton {background-color:rgb(49,50,52); border:1px solid;border-color:rgb(109,109,109);}";
//    m_fitButton->setStyleSheet(buttonStyle);
//    m_fullScreenButton->setStyleSheet(buttonStyle);
    m_fitButton->setStyleSheet("QPushButton{border-image: url(:/icons/light/32x32/player-zoom-fit.png)}"
                               "QPushButton:pressed{ border-image: url(:/icons/light/32x32/player-zoom-fit-pressed.png)}");
    m_fullScreenButton->setStyleSheet("QPushButton{border-image: url(:/icons/light/32x32/player-zoom-fullscreen.png)}"
                                      "QPushButton:pressed{ border-image: url(:/icons/light/32x32/player-zoom-fullscreen-pressed.png)}");
    m_zoomSetButton->setStyleSheet("QPushButton{border-image: url(:/icons/light/32x32/player-zoom-set.png)}"
                                   "QPushButton:pressed{ border-image: url(:/icons/light/32x32/player-zoom-set-pressed.png)}");

    connect(m_fitButton, SIGNAL(clicked()), this, SLOT(onFitButtonClicked()));
    connect(m_fullScreenButton, SIGNAL(clicked()), this, SLOT(onFullScreenButtonClicked()));
    connect(m_zoomSetButton, SIGNAL(clicked()), this, SLOT(onShowZoomMenu()));

//    m_zoomComBox = new QComboBox;
//    m_zoomComBox->setFixedWidth(65);
////    m_zoomComBox->setCheckable(true);
//    m_zoomComBox->setToolTip(tr("Toggle zoom"));
//    m_zoomComBox->setStyleSheet("QComboBox { background-color:rgb(100,100,100);color:rgb(225,225,225); }");

//    m_zoomComBox->addItem(tr("Fit"));
//    m_zoomComBox->addItem(tr("10%"));
//    m_zoomComBox->addItem(tr("25%"));
//    m_zoomComBox->addItem(tr("50%"));
//    m_zoomComBox->addItem(tr("100%"));
//    m_zoomComBox->addItem(tr("200%"));

//    connect(m_zoomComBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ZoomChanged(int)));

//     m_zoomComBox->setCurrentIndex(0);
     this->ZoomChanged(0);


     //volume control

    m_volumePopup = new QFrame(this, Qt::Popup | Qt::NoDropShadowWindowHint);
    m_volumePopup->setStyleSheet("background-color:rgb(82,82,82);");
    QVBoxLayout *volumeLayout = new QVBoxLayout(m_volumePopup);

//     m_muteButton = new QPushButton;
//     m_muteButton->setFlat(true);
// //    m_muteButton->setIconSize(QSize(33,33));
// //    m_muteButton->setStyleSheet("background-color:rgb(82,82,82);");

// //    m_muteButton->setIcon(QIcon(":/icons/light/32x32/player-volume.png"));
//     m_muteButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/player-volume.png)}");

//     m_muteButton->setCheckable(true);
//     m_muteButton->setChecked(false);
//     m_bMute = false;
//     m_muteButton->setFixedSize(QSize(33,33));
//     m_muteButton->setIconSize(QSize(32,32));
//     connect(m_muteButton, SIGNAL(clicked(bool)),this,SLOT(onMuteButtonToggled(bool)));


     m_volumeButton = new QPushButton;
     m_volumeButton->setFlat(true);
     m_volumeButton->setStyleSheet("QPushButton{border-image: url(:/icons/light/32x32/player-volume.png)}"
                                   "QPushButton:pressed{border-image: url(:/icons/light/32x32/player-volume-pressed.png)}");
     if(Settings.playerVolume()==0)
     {
         m_volumeButton->setStyleSheet("QPushButton{border-image: url(:/icons/light/32x32/player-mute.png)}");
     }
     m_volumeButton->setCheckable(true);
     m_volumeButton->setChecked(false);
     m_bMute = false;
     m_volumeButton->setFixedSize(QSize(20,20));
     m_volumeButton->setIconSize(QSize(20,20));
     connect(m_volumeButton, SIGNAL(clicked()),this,SLOT(onShowVolumeSlider()));


//     m_volumeSlider = new QSlider(Qt::Horizontal);
     m_volumeSlider = new QSlider(Qt::Vertical);
     m_volumeSlider->setFocusPolicy(Qt::NoFocus);
     m_volumeSlider->setMinimumWidth(20);   // 30   //VOLUME_SLIDER_HEIGHT);
     m_volumeSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
     m_volumeSlider->setRange(0,99);

     m_volumeSlider->setValue(Settings.playerVolume());

     QString strStyle = "QSlider::add-page:vertical{background:rgb(15,114,103);border-radius: 4px}";
     strStyle.append("QSlider::groove:vertical{background:'grey';width:8px;border-radius: 4px;}");
     strStyle.append("QSlider::handle:vertical{background:'white';height:5px;margin:0 -2px;border-radius: 2px}");
     strStyle.append("QToolTip{background-color:rgb(255,255,210)}");
     m_volumeSlider->setStyleSheet(strStyle);

     m_volumeSlider->setContentsMargins(0,0,10,0);
     setVolume(m_volumeSlider->value());

     m_volumeSlider->setToolTip(tr("Adjust the audio volume"));
     connect(m_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(onVolumeChanged(int)));

     volumeLayout->addWidget(m_volumeSlider);

    //add widget for player toolbar

    QGridLayout *toolbarlayout = new QGridLayout(this);
    toolbarlayout->setContentsMargins(0,0,0,0);
    toolbarlayout->setSpacing(0);

    QWidget *toolWidget = new QWidget;
    toolWidget->setFixedHeight(52);
    toolWidget->setContentsMargins(0,0,0,0);
    toolWidget->setStyleSheet(".QWidget{background-color:rgb(82,82,82); border: 0px;border-radius: 3px}");


    m_btnSeekPrevious = new QPushButton();//QIcon(":/icons/light/32x32/media-seek-backward.png"),"");
    m_btnSeekPrevious->setFlat(true);
    m_btnSeekPrevious->setFixedSize(32,32);
    m_btnSeekPrevious->setIconSize(QSize(32,32));
    m_btnSeekPrevious->setToolTip(tr("Skip Previous"));
    connect(m_btnSeekPrevious, SIGNAL(clicked()), this, SLOT(on_actionSkipPrevious_triggered()));
    m_btnSeekPrevious->setContentsMargins(0,0,0,0);
    m_btnSeekPrevious->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-seek-backward.png)}" "QPushButton:pressed{ border-image: url(:/icons/light/32x32/media-seek-backward-pressed.png)}");



    m_btnSeekNext = new QPushButton();//QIcon();//":/icons/light/32x32/media-seek-forward.png"),"");
    m_btnSeekNext->setFlat(true);
    m_btnSeekNext->setFixedSize(32,32);
    m_btnSeekNext->setIconSize(QSize(32,32));
    connect(m_btnSeekNext, SIGNAL(clicked()), this, SLOT(on_actionSkipNext_triggered()));
    m_btnSeekNext->setContentsMargins(0,0,0,0);
    m_btnSeekNext->setToolTip(tr("Skip Next"));
    m_btnSeekNext->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-seek-forward.png)}" "QPushButton:pressed{ border-image: url(:/icons/light/32x32/media-seek-forward-pressed.png)}");


    m_btnPlay = new QPushButton();//m_playIcon,"");
    m_btnPlay->setFlat(true);
    m_btnPlay->setFixedSize(32,32);
    m_btnPlay->setIconSize(QSize(32,32));
    connect(m_btnPlay, SIGNAL(clicked()), this, SLOT(togglePlayPaused()));
    m_btnPlay->setContentsMargins(0,0,0,0);
    m_btnPlay->setToolTip(tr("Start playback"));
    m_btnPlay->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-playback-start.png)}" );

//    m_btnRewind = new QPushButton();//QIcon(":/icons/light/32x32/media-skip-backward.png"),"");
//    m_btnRewind->setFlat(true);
//    m_btnRewind->setFixedSize(32,32);
//    m_btnRewind->setIconSize(QSize(32,32));
//    connect(m_btnRewind, SIGNAL(clicked()), this, SLOT(rewind()));
//    m_btnRewind->setContentsMargins(0,0,0,0);
//    m_btnRewind->setToolTip(tr("Play quickly backwards (J)"));
//    m_btnRewind->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-skip-backward.png)}" "QPushButton:pressed{ border-image: url(:/icons/light/32x32/media-skip-backward-pressed.png)}");

//    m_btnFastForword = new QPushButton();//QIcon(":/icons/light/32x32/media-skip-forward.png"),"");
//    m_btnFastForword->setFlat(true);
//    m_btnFastForword->setFixedSize(32,32);
//    m_btnFastForword->setIconSize(QSize(32,32));
//    connect(m_btnFastForword, SIGNAL(clicked()), this, SLOT(fastForward()));
//    m_btnFastForword->setContentsMargins(0,0,0,0);
//    m_btnFastForword->setToolTip(tr("Play quickly forwards (L)"));
//    m_btnFastForword->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-skip-forward.png)}" "QPushButton:pressed{ border-image: url(:/icons/light/32x32/media-skip-forward-pressed.png)}");

    m_btnPreFrame = new QPushButton();//QIcon(":/icons/light/32x32/media-seek-preframe.png"),"");
    m_btnPreFrame->setFlat(true);
    m_btnPreFrame->setFixedSize(32,32);
    m_btnPreFrame->setIconSize(QSize(32,32));
    connect(m_btnPreFrame, SIGNAL(clicked()), this, SLOT(seekPreFrame()));
    m_btnPreFrame->setContentsMargins(0,0,0,0);
//    m_btnPreFrame->setToolTip(tr("Play quickly backwards (J)"));
    m_btnPreFrame->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-seek-preframe.png)}" "QPushButton:pressed{ border-image: url(:/icons/light/32x32/media-seek-preframe-pressed.png)}");

    m_btnNextFrame = new QPushButton();//QIcon(":/icons/light/32x32/media-seek-nextframe.png"),"");
    m_btnNextFrame->setFlat(true);
    m_btnNextFrame->setFixedSize(32,32);
    m_btnNextFrame->setIconSize(QSize(32,32));
    connect(m_btnNextFrame, SIGNAL(clicked()), this, SLOT(seekNextFrame()));
    m_btnNextFrame->setContentsMargins(0,0,0,0);
//    m_btnNextFrame->setToolTip(tr("Play quickly forwards (L)"));
    m_btnNextFrame->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-seek-nextframe.png)}" "QPushButton:pressed{ border-image: url(:/icons/light/32x32/media-seek-nextframe-pressed.png)}");


//    toolbarlayout->addWidget(m_videoScrollWidget,0,0,1,14);
    toolbarlayout->addWidget(progressWidget,0,0,1,17);      // 14

    QSpacerItem *spacer0 = new QSpacerItem(10,20);
    toolbarlayout->addItem(spacer0, 1,0,1,1);


    toolbarlayout->addWidget(m_positionSpinner, 1,1,1,1);

    QSpacerItem *spacer1 = new QSpacerItem(100,20, QSizePolicy::Expanding);
    toolbarlayout->addItem(spacer1, 1,2,1,1);

    toolbarlayout->addWidget(m_btnSeekPrevious, 1,3,1,1);
//    toolbarlayout->addWidget(m_btnRewind,1,4,1,1);
    toolbarlayout->addWidget(m_btnPreFrame,1,4,1,1);
    toolbarlayout->addWidget(m_btnPlay,1,5,1,1);
//    toolbarlayout->addWidget(m_btnFastForword,1,6,1,1);
    toolbarlayout->addWidget(m_btnNextFrame,1,6,1,1);
    toolbarlayout->addWidget(m_btnSeekNext,1,7,1,1);

    QSpacerItem *spacer2 = new QSpacerItem(50,20, QSizePolicy::Expanding);
    toolbarlayout->addItem(spacer2, 1,8,1,1);

//    toolbarlayout->addWidget(m_zoomComBox,1,9,1,1);
    toolbarlayout->addWidget(m_fitButton,1,9,1,1);
    QSpacerItem *spacer3 = new QSpacerItem(15, 20);
    toolbarlayout->addItem(spacer3,1,10,1,1);
    toolbarlayout->addWidget(m_fullScreenButton,1,11,1,1);
#ifdef Q_OS_WIN
    QSpacerItem *spacer4 = new QSpacerItem(0, 20);
    toolbarlayout->addItem(spacer4,1,12,1,1);
#else
    QSpacerItem *spacer4 = new QSpacerItem(15, 20);
    toolbarlayout->addItem(spacer4,1,12,1,1);
#endif
//    toolbarlayout->addWidget(m_zoomComBox,1,13,1,1);
    toolbarlayout->addWidget(m_zoomSetButton,1,13,1,1);

    QSpacerItem *spacer5 = new QSpacerItem(15,20);   // 20, 20
    toolbarlayout->addItem(spacer5, 1,14,1,1);      // 10

//    toolbarlayout->addWidget(m_muteButton,1,11,1,1);
//    toolbarlayout->addWidget(m_volumeSlider,1,12,1,1);
    toolbarlayout->addWidget(m_volumeButton,1,15,1,1);

    QSpacerItem *spacer6 = new QSpacerItem(10,20);
    toolbarlayout->addItem(spacer6, 1,16,1,1);      // 13

    toolWidget->setLayout(toolbarlayout);

    m_playerLayout = new QVBoxLayout(this);
    m_playerLayout->setSpacing(0);
    m_playerLayout->setContentsMargins(0,0,0,0);
    m_playerLayout->addWidget(m_videoScrollWidget, 1);
    m_playerLayout->addWidget(toolWidget,1);



//    toolbar->addWidget(toolWidget);

//    vlayout->addWidget(toolWidget);

//    toolbar->addWidget(m_positionSpinner);

//    toolbar->addAction(actionSkipPrevious);
//    toolbar->addAction(actionRewind);
//    toolbar->addAction(actionPlay);
//    toolbar->addAction(actionFastForward);
//    toolbar->addAction(actionSkipNext);

//    toolbar->addWidget(m_zoomComBox);



 //   vlayout->addWidget(toolWidget);



    connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onFrameDisplayed(const SharedFrame&)));
    connect(actionPlay, SIGNAL(triggered()), this, SLOT(togglePlayPaused()));
    connect(actionPause, SIGNAL(triggered()), this, SLOT(pause()));
    connect(actionFastForward, SIGNAL(triggered()), this, SLOT(fastForward()));
    connect(actionRewind, SIGNAL(triggered()), this, SLOT(rewind()));
    connect(m_scrubber, SIGNAL(seeked(int)), this, SLOT(seek(int)));
    connect(m_scrubber, SIGNAL(inChanged(int)), this, SLOT(onInChanged(int)));
    connect(m_scrubber, SIGNAL(outChanged(int)), this, SLOT(onOutChanged(int)));
    connect(m_positionSpinner, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
    connect(m_positionSpinner, SIGNAL(editingFinished()), this, SLOT(setFocus()));
    connect(this, SIGNAL(endOfStream()), this, SLOT(pause()));
    connect(this, SIGNAL(zoomChanged(float)), MLT.videoWidget(), SLOT(setZoom(float)));
    connect(m_horizontalScroll, SIGNAL(valueChanged(int)), MLT.videoWidget(), SLOT(setOffsetX(int)));
    connect(m_verticalScroll, SIGNAL(valueChanged(int)), MLT.videoWidget(), SLOT(setOffsetY(int)));
    setFocusPolicy(Qt::StrongFocus);


    QString strVStyle = "QScrollBar::vertical{background-color:rgb(0,0,0);width:14px;border: 3px solid rgb(0,0,0)}";
    strVStyle.append("QScrollBar::handle:vertical{background:#787878;border-radius:4px}");
    strVStyle.append("QScrollBar::add-page:vertical{background:none}");
    strVStyle.append("QScrollBar::sub-page:vertical{background:none}");
    strVStyle.append("QScrollBar::add-line:vertical{height:0px; background:none}");
    strVStyle.append("QScrollBar::sub-line:vertical{height:0px; background:none}");
    m_verticalScroll->setStyleSheet(strVStyle);

    QString strHStyle = "QScrollBar::horizontal{background-color:rgb(0,0,0);height:14px;border: 3px solid rgb(0,0,0)}";
    strHStyle.append("QScrollBar::handle:horizontal{background:#787878;border-radius:4px}");
    strHStyle.append("QScrollBar::add-page:horizontal{background:none;}");
    strHStyle.append("QScrollBar::sub-page:horizontal{background:none;}");
    strHStyle.append("QScrollBar::add-line:horizontal{width:0px; background:none}");
    strHStyle.append("QScrollBar::sub-line:horizontal{width:0px; background:none}");
    m_horizontalScroll->setStyleSheet(strHStyle);

}

void Player::connectTransport(const TransportControllable* receiver)
{
    if (receiver == m_currentTransport) return;
    if (m_currentTransport)
        disconnect(m_currentTransport);
    m_currentTransport = receiver;
    connect(this, SIGNAL(played(double)), receiver, SLOT(play(double)));
    connect(this, SIGNAL(paused()), receiver, SLOT(pause()));
    connect(this, SIGNAL(stopped()), receiver, SLOT(stop()));
    connect(this, SIGNAL(seeked(int)), receiver, SLOT(seek(int)));
    connect(this, SIGNAL(rewound()), receiver, SLOT(rewind()));
    connect(this, SIGNAL(fastForwarded()), receiver, SLOT(fastForward()));
    connect(this, SIGNAL(previousSought(int)), receiver, SLOT(previous(int)));
    connect(this, SIGNAL(nextSought(int)), receiver, SLOT(next(int)));
    connect(this, SIGNAL(inChanged(int)), receiver, SLOT(setIn(int)));
    connect(this, SIGNAL(outChanged(int)), receiver, SLOT(setOut(int)));
}

void Player::setupActions(QWidget* widget)
{
    actionPlay = new QAction(widget);
    actionPlay->setObjectName(QString::fromUtf8("actionPlay"));
    actionPlay->setIcon(QIcon(":/icons/light/32x32/media-playback-start.png"));
    actionPlay->setDisabled(true);

    actionPause = new QAction(widget);
    actionPause->setObjectName(QString::fromUtf8("actionPause"));
    actionPause->setIcon(QIcon(":/icons/light/32x32/media-playback-pause.png"));
    actionPause->setDisabled(true);

    actionSkipNext = new QAction(widget);
    actionSkipNext->setObjectName(QString::fromUtf8("actionSkipNext"));
    actionSkipNext->setIcon(QIcon(":/icons/light/32x32/media-skip-forward.png"));
    actionSkipNext->setDisabled(true);

    actionSkipPrevious = new QAction(widget);
    actionSkipPrevious->setObjectName(QString::fromUtf8("actionSkipPrevious"));
    actionSkipPrevious->setIcon(QIcon(":/icons/light/32x32/media-skip-backward.png"));
    actionSkipPrevious->setDisabled(true);

    actionRewind = new QAction(widget);
    actionRewind->setObjectName(QString::fromUtf8("actionRewind"));
    actionRewind->setIcon( QIcon(":/icons/light/32x32/media-seek-backward.png"));
    actionRewind->setDisabled(true);

    actionFastForward = new QAction(widget);
    actionFastForward->setObjectName(QString::fromUtf8("actionFastForward"));
    actionFastForward->setIcon( QIcon(":/icons/light/32x32/media-seek-forward.png"));
    actionFastForward->setDisabled(true);

    actionVolume = new QAction(widget);
    actionVolume->setObjectName(QString::fromUtf8("actionVolume"));
    actionVolume->setIcon(QIcon(":/icons/light/32x32/player-volume.png"));
    retranslateUi(widget);
    QMetaObject::connectSlotsByName(widget);
}

void Player::retranslateUi(QWidget* widget)
{
    Q_UNUSED(widget)
    actionPlay->setText(tr("Play"));
#ifndef QT_NO_TOOLTIP
    actionPlay->setToolTip(tr("Start playback (L)"));
#endif // QT_NO_TOOLTIP
    actionPlay->setShortcut(QString("Space"));
    actionPause->setText(tr("Pause"));
#ifndef QT_NO_TOOLTIP
    actionPause->setToolTip(tr("Pause playback (K)"));
#endif // QT_NO_TOOLTIP
    actionSkipNext->setText(tr("Skip Next"));
#ifndef QT_NO_TOOLTIP
    actionSkipNext->setToolTip(tr("Skip to the next point (Alt+Right)"));
#endif // QT_NO_TOOLTIP
    actionSkipNext->setShortcut(QString("Alt+Right"));
    actionSkipPrevious->setText(tr("Skip Previous"));
#ifndef QT_NO_TOOLTIP
    actionSkipPrevious->setToolTip(tr("Skip to the previous point (Alt+Left)"));
#endif // QT_NO_TOOLTIP
    actionSkipPrevious->setShortcut(QString("Alt+Left"));
    actionRewind->setText(tr("Rewind"));
#ifndef QT_NO_TOOLTIP
    actionRewind->setToolTip(tr("Play quickly backwards (J)"));
#endif // QT_NO_TOOLTIP
    actionFastForward->setText(tr("Fast Forward"));
#ifndef QT_NO_TOOLTIP
    actionFastForward->setToolTip(tr("Play quickly forwards (L)"));
#endif // QT_NO_TOOLTIP
    actionVolume->setText(tr("Volume"));
#ifndef QT_NO_TOOLTIP
    actionVolume->setToolTip(tr("Show the volume control"));
#endif
}

void Player::setIn(int pos)
{
    Q_ASSERT(m_scrubber);
    m_scrubber->setInPoint(pos);
    if (pos >= 0 && pos > m_previousOut)
        setOut(m_duration - 1);
}

void Player::setOut(int pos)
{
    Q_ASSERT(m_scrubber);
    m_scrubber->setOutPoint(pos);
    if (pos >= 0 && pos < m_previousIn)
        setIn(0);
}

void Player::setMarkers(const QList<int> &markers)
{
    Q_ASSERT(m_scrubber);
    m_scrubber->setMarkers(markers);
}

QSize Player::videoSize() const
{
    Q_ASSERT(m_videoWidget);
    return m_videoWidget->size();
}

void Player::resizeEvent(QResizeEvent*)
{
    MLT.onWindowResize();
    if (Settings.playerZoom() > 0.0f) {
        float horizontal = 400;//float(m_horizontalScroll->value()) / m_horizontalScroll->maximum();
        float vertical = 300;//float(m_verticalScroll->value()) / m_verticalScroll->maximum();
        adjustScrollBars(horizontal, vertical);
    } else {
        m_horizontalScroll->hide();
        m_verticalScroll->hide();
    }
}

void Player::play(double speed)
{
    emit played(speed);
    if (m_isSeekable) {
      //  m_btnPlay->setIcon(m_pauseIcon);
        m_btnPlay->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-playback-pause.png)}" );
        m_btnPlay->setToolTip(tr("Pause playback (K)"));
        actionPlay->setIcon(m_pauseIcon);
        actionPlay->setText(tr("Pause"));
        actionPlay->setToolTip(tr("Pause playback (K)"));
    }
    else {
     //   m_btnPlay->setIcon( QIcon(":/icons/light/32x32/media-playback-stop.png"));
        m_btnPlay->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-playback-stop.png)}" );
        m_btnPlay->setToolTip(tr("Stop playback (K)"));

        actionPlay->setIcon( QIcon(":/icons/light/32x32/media-playback-stop.png"));
        actionPlay->setText(tr("Stop"));
        actionPlay->setToolTip(tr("Stop playback (K)"));
    }
}

void Player::pause()
{
    emit paused();
    showPaused();
}

void Player::stop()
{
    emit stopped();
//    m_btnPlay->setIcon(m_playIcon);
    m_btnPlay->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-playback-start.png)}" );
    m_btnPlay->setToolTip(tr("Start playback (L)"));
    actionPlay->setIcon(m_playIcon);
    actionPlay->setText(tr("Play"));
    actionPlay->setToolTip(tr("Start playback (L)"));
}

void Player::togglePlayPaused()
{
    if (actionPlay->icon().cacheKey() == m_playIcon.cacheKey())
    {
        play();
        m_bPlayedOrStoped = true;
    }
    else if (m_isSeekable)
    {
        pause();
        m_bPlayedOrStoped = false;
    }
    else
    {
        stop();
        m_bPlayedOrStoped = false;
    }
}

void Player::seek(int position)
{
    if (m_isSeekable) {
        if (position >= 0) {
            emit seeked(qMin(position, m_duration - 1));
        }
    }
    // Seek implies pause.
    actionPlay->setIcon(m_playIcon);
    actionPlay->setText(tr("Play"));
    actionPlay->setToolTip(tr("Start playback (L)"));
}

void Player::onProducerOpened(bool play)
{
#ifdef Q_OS_WIN
    // Workaround video not showing on Windows with Qt 5.5 upgrade.
    m_videoWidget->show();
#endif
    m_duration = MLT.producer()->get_length();
    m_isSeekable = MLT.isSeekable();
    MLT.producer()->set("ignore_points", 1);
    m_scrubber->setFramerate(MLT.profile().fps());
    m_scrubber->setScale(m_duration);
//    m_progressBar->setRange(0,m_duration);
    if (!MLT.isPlaylist())
        m_scrubber->setMarkers(QList<int>());
//    m_inPointLabel->setText("--:--:--:-- / ");
//    m_selectedLabel->setText("--:--:--:--");
    if (m_isSeekable) {
        m_durationLabel->setText(QString("<h4><font color=white>%1</font></h4>").arg(QString(MLT.producer()->get_length_time())));
        m_previousIn = MLT.isClip()? MLT.producer()->get_in() : -1;
        m_scrubber->setEnabled(true);
        m_scrubber->setInPoint(m_previousIn);
        m_previousOut = MLT.isClip()? MLT.producer()->get_out() : -1;
        m_scrubber->setOutPoint(m_previousOut);
    }
    else {
        m_durationLabel->setText(QString("<h4><font color=white>%1</font></h4>").arg(QString(MLT.producer()->get_length_time())));
        m_scrubber->setDisabled(true);
        // cause scrubber redraw
        m_scrubber->setScale(m_duration);
    }
    m_positionSpinner->setEnabled(m_isSeekable);
    setVolume(m_volumeSlider->value());
    m_savedVolume = MLT.volume();
    onMuteButtonToggled(Settings.playerMuted());
    toggleZoom(Settings.playerZoom() > 0.0f);

    actionPlay->setEnabled(true);
    actionSkipPrevious->setEnabled(m_isSeekable);
    actionSkipNext->setEnabled(m_isSeekable);
    actionRewind->setEnabled(m_isSeekable);
    actionFastForward->setEnabled(m_isSeekable);

    connectTransport(MLT.transportControl());

    // Closing the previous producer might call pause() milliseconds before
    // calling play() here. Delays while purging the consumer on pause can
    // interfere with the play() call. So, we delay play a little to let
    // pause purging to complete.
    if (play) {
        if (m_pauseAfterOpen) {
            m_pauseAfterOpen = false;
            QTimer::singleShot(500, this, SLOT(postProducerOpened()));
        } else {
            if (MLT.consumer()->is_stopped()) {
                this->play();
            } else {
                // This seek purges the consumer to prevent latent end-of-stream detection.
                seek(0);
                QTimer::singleShot(500, this, SLOT(play()));
            }
        }
    }
}

void Player::postProducerOpened()
{
    if (MLT.producer())
        seek(MLT.producer()->position());
}

void Player::onMeltedUnitOpened()
{
    m_isMeltedPlaying = -1; // unknown
    Q_ASSERT(MLT.producer());
    m_duration = MLT.producer()->get_length();
    m_isSeekable = true;
    MLT.producer()->set("ignore_points", 1);
    m_scrubber->setFramerate(MLT.profile().fps());
    m_scrubber->setScale(m_duration);
    m_scrubber->setMarkers(QList<int>());
//    m_progressBar->setRange(0,m_duration);
  //  m_inPointLabel->setText("--:--:--:-- / ");
  //  m_selectedLabel->setText("--:--:--:--");
    m_durationLabel->setText(QString("<h4><font color=white>%1</font></h4>").arg(QString(MLT.producer()->get_length_time())));
    m_previousIn = MLT.producer()->get_in();
    m_scrubber->setEnabled(true);
    m_scrubber->setInPoint(m_previousIn);
    m_previousOut = MLT.producer()->get_out();
    m_scrubber->setOutPoint(m_previousOut);
    m_positionSpinner->setEnabled(m_isSeekable);
    setVolume(m_volumeSlider->value());
    m_savedVolume = MLT.volume();
    onMuteButtonToggled(Settings.playerMuted());
    actionPlay->setEnabled(true);
    actionSkipPrevious->setEnabled(m_isSeekable);
    actionSkipNext->setEnabled(m_isSeekable);
    actionRewind->setEnabled(m_isSeekable);
    actionFastForward->setEnabled(m_isSeekable);
    setIn(-1);
    setOut(-1);
    setFocus();
}

void Player::onDurationChanged()
{
    m_duration = MLT.producer()->get_length();
    m_isSeekable = MLT.isSeekable();
    m_scrubber->setScale(m_duration);
    m_scrubber->setMarkers(QList<int>());
//    m_progressBar->setRange(0, m_duration);
    m_durationLabel->setText(QString("<h4><font color=white>%1</font></h4>").arg(QString(MLT.producer()->get_length_time())));
    if(qFuzzyIsNull(MLT.producer()->get_speed()))
//    if (MLT.producer()->get_speed() == 0)
        seek(m_position);
    else if (m_position >= m_duration)
        seek(m_duration - 1);
}

void Player::onShowFrame(int position, double fps, int in, int out, int length, bool isPlaying)
{
    m_duration = length;
    MLT.producer()->set("length", length);
    m_durationLabel->setText(QString("<h4><font color=white>%1</font></h4>").arg(QString(MLT.producer()->get_length_time())));
    m_scrubber->setFramerate(fps);
    m_scrubber->setScale(m_duration);
//    m_progressBar->setRange(0, m_duration);
    if (position < m_duration) {
        m_position = position;
        m_positionSpinner->blockSignals(true);
        m_positionSpinner->setValue(position);
        m_positionSpinner->blockSignals(false);
        m_scrubber->onSeek(position);
    //    m_progressBar->setValue(position);
    }
    if (m_isMeltedPlaying == -1 || isPlaying != m_isMeltedPlaying) {
        m_isMeltedPlaying = isPlaying;
        if (isPlaying) {
            actionPlay->setIcon(m_pauseIcon);
            actionPlay->setText(tr("Pause"));
            actionPlay->setToolTip(tr("Pause playback (K)"));
        }
        else {
            actionPlay->setIcon(m_playIcon);
            actionPlay->setText(tr("Play"));
            actionPlay->setToolTip(tr("Start playback (L)"));
        }
    }
    m_previousIn = in;
    m_previousOut = out;
    m_scrubber->blockSignals(true);
    setIn(in);
    setOut(out);
    m_scrubber->blockSignals(false);
}

void Player::onFrameDisplayed(const SharedFrame& frame)
{

    int position = frame.get_position();
    if (position < m_duration) {
        m_position = position;
        m_positionSpinner->blockSignals(true);
        m_positionSpinner->setValue(position);
        m_positionSpinner->blockSignals(false);
        m_scrubber->onSeek(position);
   //     m_progressBar->setValue(position);
    }
    if (position >= m_duration - 1)
    {
        emit endOfStream();
        m_scrubber->onSeek(m_duration);
    //    m_progressBar->setValue(m_duration);
//        emit endOfStream();
    }
}

void Player::updateSelection()
{
    if (MLT.producer() && MLT.producer()->get_in() > 0) {
     //   m_inPointLabel->setText(QString(MLT.producer()->get_time("in")).append(" / "));
     //   m_selectedLabel->setText(MLT.producer()->frames_to_time(MLT.producer()->get_playtime()));
    } else {
     //   m_inPointLabel->setText("--:--:--:-- / ");
        if (MLT.producer() && MLT.isClip() &&
                MLT.producer()->get_out() < m_duration - 1) {
       //     m_selectedLabel->setText(MLT.producer()->frames_to_time(MLT.producer()->get_playtime()));
        } else if (!MLT.producer() || MLT.producer()->get_in() == 0) {
       //     m_selectedLabel->setText("--:--:--:--");
        }
    }
}

void Player::onInChanged(int in)
{
    if (in != m_previousIn)
        emit inChanged(in);
    m_previousIn = in;
    updateSelection();
}

void Player::onOutChanged(int out)
{
    if (out != m_previousOut)
        emit outChanged(out);
    m_previousOut = out;
    updateSelection();
}

void Player::on_actionSkipNext_triggered() //跳到下一曲
{

    emit showStatusMessage(actionSkipNext->toolTip());

    if (m_scrubber->markers().size() > 0) {
        foreach (int x, m_scrubber->markers()) {
            if (x > m_position) {
                emit seeked(x);
                return;
            }
        }
        emit seeked(m_duration - 1);
    }
    else
    {
        emit nextSought(m_position);
        emit nextSought();
    }
}

void Player::on_actionSkipPrevious_triggered() //跳到上一曲
{

    emit showStatusMessage(actionSkipPrevious->toolTip());

    if (m_scrubber->markers().size() > 0) {
        QList<int> markers = m_scrubber->markers();
        int n = markers.count();
        while (n--) {
            if (markers[n] < m_position) {
                emit seeked(markers[n]);
                return;
            }
        }
        emit seeked(0);
    }
    else
    {
        emit previousSought(m_position);
        emit previousSought();
    }
}

void Player::rewind()//快退
{
    if (m_isSeekable)
        emit rewound();
}

void Player::fastForward()//快进
{
    if (m_isSeekable)
        emit fastForwarded();
    else
        play();
}

void Player::seekPreFrame()
{
    m_positionSpinner->setValue(m_positionSpinner->value()-1);
}

void Player::seekNextFrame()
{
    m_positionSpinner->setValue(m_positionSpinner->value()+1);
}

void Player::showPaused()
{
    actionPlay->setIcon(m_playIcon);
    actionPlay->setText(tr("Play"));
    actionPlay->setToolTip(tr("Start playback (L)"));

//    m_btnPlay->setIcon(m_playIcon);
    m_btnPlay->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-playback-start.png)}" );
    m_btnPlay->setToolTip(tr("Start playback (L)"));
}

void Player::showPlaying()
{
    actionPlay->setIcon(m_pauseIcon);
    actionPlay->setText(tr("Pause"));
    actionPlay->setToolTip(tr("Pause playback (K)"));

//    m_btnPlay->setIcon(m_pauseIcon);
    m_btnPlay->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/media-playback-pause.png)}" );
    m_btnPlay->setToolTip(tr("Pause playback (K)"));
}

void Player::switchToTab(TabIndex index)
{
    m_tabs->setCurrentIndex(index);
    emit tabIndexChanged(index);
}

void Player::enableTab(TabIndex index, bool enabled)
{
    m_tabs->setTabEnabled(index, enabled);
}

void Player::onTabBarClicked(int index)
{
    switch (index) {
    case SourceTabIndex:
        if (MLT.savedProducer() && MLT.savedProducer()->is_valid() && MLT.producer()
            && MLT.producer()->get_producer() != MLT.savedProducer()->get_producer()) {
            m_pauseAfterOpen = true;
            MAIN.open(new Mlt::Producer(MLT.savedProducer()));
        }
        break;
    case ProjectTabIndex:
        if (MAIN.multitrack()) {
            if (!MLT.isMultitrack() && MAIN.multitrack())
                MAIN.seekTimeline(MAIN.multitrack()->position());
        } else {
            if (!MLT.isPlaylist() && MAIN.playlist())
                MAIN.seekPlaylist(MAIN.playlist()->position());
        }
        break;
    }
}

void Player::setStatusLabel(const QString &text, int timeoutSeconds, QAction* action)
{
    QString s = QString("  %1  ").arg(
                m_statusLabel->fontMetrics().elidedText(text, Qt::ElideRight,
                    m_scrubber->width() - m_tabs->width() - 30));
    m_statusLabel->setText(s);
    m_statusLabel->setToolTip(text);
    if (action)
        connect(m_statusLabel, SIGNAL(clicked(bool)), action, SIGNAL(triggered(bool)));
    else
        disconnect(m_statusLabel, SIGNAL(clicked(bool)));

    // Cancel the fade out.
    if (m_statusFadeOut->state() == QAbstractAnimation::Running) {
        m_statusFadeOut->stop();
    }
    if (text.isEmpty()) {
        // Make it transparent.
        m_statusTimer.stop();
        m_statusFadeOut->setStartValue(0);
        m_statusFadeOut->start();
    } else {
        // Reset the fade out animation.
        m_statusFadeOut->setStartValue(1);

        // Fade in.
        if (m_statusFadeIn->state() != QAbstractAnimation::Running && !m_statusTimer.isActive()) {
            m_statusFadeIn->start();
            m_statusTimer.start(timeoutSeconds * 1000);
        }
    }
}

void Player::onFadeOutFinished()
{
    m_statusLabel->disconnect(SIGNAL(clicked(bool)));
    m_statusLabel->setToolTip(QString());
}

void Player::adjustScrollBars(float horizontal, float vertical)
{
    if (MLT.profile().width() * m_zoomToggleFactor > m_videoWidget->width()) {
        m_horizontalScroll->setPageStep(m_videoWidget->width());
        m_horizontalScroll->setMaximum(int(MLT.profile().width() * m_zoomToggleFactor)
                                       - m_horizontalScroll->pageStep());
        m_horizontalScroll->setValue(qRound(horizontal * m_horizontalScroll->maximum()));
        emit m_horizontalScroll->valueChanged(m_horizontalScroll->value());
        m_horizontalScroll->show();
    } else {
        int max = int(MLT.profile().width() * m_zoomToggleFactor) - m_videoWidget->width();
        emit m_horizontalScroll->valueChanged(qRound(0.5 * max));
        m_horizontalScroll->hide();
    }

    if (MLT.profile().height() * m_zoomToggleFactor > m_videoWidget->height()) {
        m_verticalScroll->setPageStep(m_videoWidget->height());
        m_verticalScroll->setMaximum(int(MLT.profile().height() * m_zoomToggleFactor)
                                     - m_verticalScroll->pageStep());
        m_verticalScroll->setValue(qRound(vertical * m_verticalScroll->maximum()));
        emit m_verticalScroll->valueChanged(m_verticalScroll->value());
        m_verticalScroll->show();
    } else {
        int max = int(MLT.profile().height() * m_zoomToggleFactor) - m_videoWidget->height();
        emit m_verticalScroll->valueChanged(qRound(0.5 * max));
        m_verticalScroll->hide();
    }
}

double Player::setVolume(int volume)
{
    const double gain = double(volume) / VOLUME_KNEE;
    MLT.setVolume(gain);
    return gain;
}

void Player::moveVideoToScreen(int screen)
{
    if (screen == m_monitorScreen) return;
    if (screen == -2) {
        // -2 = embedded
        if (!m_videoScrollWidget->isFullScreen()) return;
        m_videoScrollWidget->showNormal();
        m_playerLayout->insertWidget(0, m_videoScrollWidget, 10);
    } else if (QApplication::desktop()->screenCount() > 1) {
        // -1 = find first screen the app is not using
        for (int i = 0; screen == -1 && i < QApplication::desktop()->screenCount(); i++) {
            if (i != QApplication::desktop()->screenNumber(this))
                screen = i;
        }
        m_videoScrollWidget->setParent(QApplication::desktop()->screen(screen));
        m_videoScrollWidget->move(QApplication::desktop()->screenGeometry(screen).bottomLeft());
        m_videoScrollWidget->showFullScreen();
    }
    m_monitorScreen = screen;
}

void Player::setPauseAfterOpen(bool pause)
{
    m_pauseAfterOpen = pause;
}

Player::TabIndex Player::tabIndex() const
{
    return static_cast<TabIndex>(m_tabs->currentIndex());   //(TabIndex)m_tabs->currentIndex();
}

//----------------------------------------------------------------------------
// IEC standard dB scaling -- as borrowed from meterbridge (c) Steve Harris

static inline float IEC_dB ( float fScale )
{
	float dB = 0.0f;

	if (fScale < 0.025f)	    // IEC_Scale(-60.0f)
		dB = (fScale / 0.0025f) - 70.0f;
	else if (fScale < 0.075f)	// IEC_Scale(-50.0f)
		dB = (fScale - 0.025f) / 0.005f - 60.0f;
	else if (fScale < 0.15f)	// IEC_Scale(-40.0f)
		dB = (fScale - 0.075f) / 0.0075f - 50.0f;
	else if (fScale < 0.3f)		// IEC_Scale(-30.0f)
		dB = (fScale - 0.15f) / 0.015f - 40.0f;
	else if (fScale < 0.5f)		// IEC_Scale(-20.0f)
		dB = (fScale - 0.3f) / 0.02f - 30.0f;
	else /* if (fScale < 1.0f)	// IED_Scale(0.0f)) */
		dB = (fScale - 0.5f) / 0.025f - 20.0f;

	return (dB > -0.001f && dB < 0.001f ? 0.0f : dB);
}

void Player::onVolumeChanged(int volume)
{
    const double gain = setVolume(volume);
    emit showStatusMessage(QString("%L1 dB").arg(double(IEC_dB(float(gain)))));
    Settings.setPlayerVolume(volume);
    Settings.setPlayerMuted(false);
    if(volume == 0)
    {
//        m_muteButton->setChecked(true);
//        m_bMute = true;
//   //     m_muteButton->setIcon(QIcon(":/icons/light/32x32/player-mute.png"));
//        m_muteButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/player-mute.png)}");
//    //    m_muteButton->setFlat(true);
//        m_muteButton->setIconSize(QSize(32,32));

        onMuteButtonToggled(true);

        m_volumeButton->setChecked(true);
        m_bMute = true;
//        m_volumeButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/player-mute.png)}");
//        m_volumeButton->setIconSize(QSize(32,32));

    }
//    else if(m_muteButton->isChecked())
    else if(m_volumeButton->isChecked())
    {

//   //     m_muteButton->setIcon(QIcon(":/icons/light/32x32/player-volume.png"));
//         m_muteButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/player-volume.png)}");
//   //     m_muteButton->setChecked(false);
//   //     m_muteButton->setFlat(true);
//        m_muteButton->setIconSize(QSize(32,32));

        m_volumeButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/player-volume.png)}"
                                      "QPushButton:pressed{ border-image: url(:/icons/light/32x32/player-volume-pressed.png)}");
//        m_volumeButton->setIconSize(QSize(32,32));
    }
}

void Player::onCaptureStateChanged(bool active)
{
    actionPlay->setDisabled(active);
}

void Player::on_actionVolume_triggered()
{
//    int x = (m_volumePopup->width() - m_volumeWidget->width()) / 2;
//    x = mapToParent(m_volumeWidget->geometry().bottomLeft()).x() - x;
//    int y = 8 - m_volumePopup->height();
//    m_volumePopup->move(mapToGlobal(m_scrubber->geometry().bottomLeft()) + QPoint(x, y));
//    m_volumePopup->show();
//    m_volumeWidget->hide();
//    m_volumeWidget->show();
}

void Player::onShowVolumeSlider()
{
    m_volumeButton->setChecked(true);
    m_volumePopup->show();              // 先显示再移动
    int x = m_volumeButton->width()/2 - m_volumePopup->width()/2 -3;
    int y = -m_volumePopup->height();
    m_volumePopup->move(m_volumeButton->mapToGlobal(QPoint(x, y)));
}

void Player::onMuteButtonToggled(bool checked)
{
    if (checked) {
        m_savedVolume = MLT.volume();
        MLT.setVolume(0);
        m_volumeSlider->setValue(0);
//     //   m_muteButton->setIcon(QIcon(":/icons/light/32x32/player-mute.png"));
//         m_muteButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/player-mute.png)}");
//        m_muteButton->setFlat(true);
//        m_muteButton->setIconSize(QSize(32,32));

        m_volumeButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/player-mute.png)}");
//        m_volumeButton->setFlat(true);
//        m_volumeButton->setIconSize(QSize(32,32));
    } else {
        MLT.setVolume(m_savedVolume);
        m_volumeSlider->setValue(int(m_savedVolume*100));
//     //   m_muteButton->setIcon(QIcon(":/icons/light/32x32/player-volume.png"));
//         m_muteButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/player-volume.png)}");
//        m_muteButton->setFlat(true);
//        m_muteButton->setIconSize(QSize(32,32));

        m_volumeButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/player-volume.png)}"
                                      "QPushButton:pressed{ border-image: url(:/icons/light/32x32/player-volume-pressed.png)}");
//        m_volumeButton->setFlat(true);
//        m_volumeButton->setIconSize(QSize(32,32));
    }
    Settings.setPlayerMuted(checked);
  //  m_volumePopup->hide();
}

void Player::setZoom(float factor)//, const QIcon& icon)
{
    emit zoomChanged(factor);
    Settings.setPlayerZoom(factor);
    if (factor == 0.0f) {
     //   m_zoomButton->setIcon(icon);
    //    m_zoomButton->setChecked(false);
        m_horizontalScroll->hide();
        m_verticalScroll->hide();
    } else {
        m_zoomToggleFactor = factor;
        adjustScrollBars(0.5f, 0.5f);
      //  m_zoomButton->setIcon(icon);
     //   m_zoomButton->setChecked(true);
    }
}

void Player::zoomPlayer(float fZoomFactor)
{
    setZoom(fZoomFactor);
    //MAIN.showFullScreen();
}

void Player::onFitButtonClicked()
{
    setZoom(0.0f);
}

void Player::onSameSizeButtonClicked()
{
    setZoom(0.0f);
    setZoom(1.0f);
}

void Player::onFullScreenButtonClicked()
{
    toggleFullScreen();
}

void Player::onShowZoomMenu()
{
    m_zoomMenu->show();
    int x = m_zoomSetButton->width()/2 - m_zoomMenu->width()/2;
    int y = m_zoomSetButton->height();
    m_zoomMenu->exec(m_zoomSetButton->mapToGlobal(QPoint(x, y)));
}

void Player::onZoomActionTriggered()
{
    QString objName = sender()->objectName();
    int i = objName.split("_")[1].toInt();
    ZoomChanged(i);
}

void Player::toggleZoom(bool checked)
{
    if (!checked)
        m_zoomToggleFactor = 0.0f;
    zoomPlayer(m_zoomToggleFactor);
}

//bool Player::eventFilter(QObject* target, QEvent* event)
//{

//    qDebug()<<"Play::eventFilter is called";
//    if (event->type() == QEvent::Enter && target == m_volumeWidget)
//        return true;
//    else if (event->type() == QEvent::Leave && target == m_volumeWidget)
//    {
//        return true;
//    }
//        return Player::eventFilter(target, event);
//}

void Player::ZoomChanged(int index)
{
    float fZoom = 0.10f;    // 0.0f
    switch(index)
    {
    case 0:
        break;
    case 1:
        fZoom = 0.25f;
        break;
    case 2:
        fZoom = 0.50f;
        break;
    case 3:
        fZoom = 1.0f;
        break;
    case 4:
        fZoom = 2.0f;
        break;
//    case 5:
//        fZoom = 2.0f;
//        break;
    }
    zoomPlayer(fZoom);
}


void Player::toggleFullScreen()
{
#ifndef Q_OS_WIN
    if (isFullScreen())
    {
        this->showNormal();
        MAIN.addPlayer();
    }
    else
    {
        int screen = QApplication::desktop()->screenNumber(this);
        this->setParent(QApplication::desktop()->screen(screen));
        this->move(QApplication::desktop()->screenGeometry(screen).bottomLeft());
        this->showFullScreen();
        setFocus();
    }
#endif
}

void Player::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        if (isFullScreen())
            toggleFullScreen();
        break;
    }
}

void Player::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    toggleFullScreen();
}
