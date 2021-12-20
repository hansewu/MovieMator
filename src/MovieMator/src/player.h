/*
 * Copyright (c) 2012-2016 Meltytech, LLC
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

#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QFrame>
#include <QIcon>
#include <QSize>
#include <QTimer>
#include "sharedframe.h"

class ScrubBar;
class QSpinBox;
class QLabel;
class TimeSpinBox;
//class QFrame;
class QSlider;
class QAction;
class QScrollBar;
class QToolButton;
class QTabBar;
class QHBoxLayout;
class QPushButton;
class TransportControllable;
class QLabel;
class QPropertyAnimation;
class QPushButton;
class QComboBox;
class MySlider;
class QVBoxLayout;
class QMenu;

class Player : public QWidget//QFrame//
{
    Q_OBJECT
public:
    typedef enum {
        SourceTabIndex = 0,
        ProjectTabIndex
    } TabIndex;

    explicit Player(QWidget *parent = nullptr);
    void connectTransport(const TransportControllable*);
    void setIn(int);
    void setOut(int);
    void setMarkers(const QList<int>&);
    QSize videoSize() const;
    int position() const {
        return m_position;
    }
    void moveVideoToScreen(int screen = -1);
    void setPauseAfterOpen(bool pause);
    TabIndex tabIndex() const;

signals:
    void endOfStream();
    void showStatusMessage(QString);
    void inChanged(int);
    void outChanged(int);
    void played(double speed);
    void paused();
    void stopped();
    void seeked(int position);
    void rewound();//快退
    void fastForwarded();//快进
    void previousSought(int currentPosition);
    void previousSought();//上一曲
    void nextSought(int currentPosition);
    void nextSought();
    void profileChanged();
    void zoomChanged(float zoom);
    void scrolledHorizontally(int x);
    void scrolledVertically(int y);
    void tabIndexChanged(int index);

    void oneActionTrimIn(int);
    void oneActionTrimOut(int);

public slots:
    void play(double speed = 1.0);
    void pause();
    void stop();
    void togglePlayPaused();
    void seek(int position);
    void onProducerOpened(bool play = true);
    void postProducerOpened();
    void onMeltedUnitOpened();
    void onDurationChanged();
    void onShowFrame(int position, double fps, int in, int out, int length, bool isPlaying);
    void onFrameDisplayed(const SharedFrame& frame);
    void onVolumeChanged(int);
    void onCaptureStateChanged(bool);
    void rewind();
    void fastForward();
    void showPaused();
    void showPlaying();
    void switchToTab(TabIndex index);
    void enableTab(TabIndex index, bool enabled = true);
    void onTabBarClicked(int index);
    void setStatusLabel(const QString& text, int timeoutSeconds, QAction* action);
    void toggleFullScreen();

    void onOneActionTrimIn(int in);
    void onOneActionTrimOut(int out);

    void seekPreFrame();
    void seekNextFrame();

protected:
    void resizeEvent(QResizeEvent* event);
//    bool eventFilter(QObject* target, QEvent* event);
    void keyPressEvent(QKeyEvent*);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    void setupActions(QWidget* widget);
    void retranslateUi(QWidget* widget);
    void adjustScrollBars(float horizontal, float vertical);
    double setVolume(int volume);
    int getPlayDuration();

    QAction *actionPlay;
    QAction *actionPause;
    QAction *actionSkipNext;
    QAction *actionSkipPrevious;
    QAction *actionRewind;
    QAction *actionFastForward;
    QAction *actionVolume;

    QPushButton *m_btnSeekPrevious;
    QPushButton *m_btnSeekNext;
    QPushButton *m_btnPlay;
//    QPushButton *m_btnRewind;
//    QPushButton *m_btnFastForword;
    QPushButton *m_btnPreFrame;     // 上一帧
    QPushButton *m_btnNextFrame;    // 下一帧

    ScrubBar* m_scrubber;
    QSlider *m_progressBar;
    TimeSpinBox* m_positionSpinner;
    QLabel* m_durationLabel;
    QLabel* m_inPointLabel;
    QLabel* m_selectedLabel;
    int m_position;
    QIcon m_playIcon;
    QIcon m_pauseIcon;
    QFrame* m_volumePopup;
    QSlider* m_volumeSlider;
    QWidget* m_volumeWidget;
    QPushButton* m_muteButton;      // 静音按钮
    QPushButton* m_volumeButton;    // 弹出音量调节按钮
    int m_previousIn;
    int m_previousOut;
    double m_savedVolume;
    int m_duration;
    bool m_isSeekable;
    int m_isMeltedPlaying;
    QScrollBar* m_horizontalScroll;
    QScrollBar* m_verticalScroll;
    QToolButton* m_zoomButton;
    QComboBox *m_zoomComBox;
    QAction* m_zoomFitAction;
    QAction* m_zoomOriginalAction;
    QAction* m_zoomOutAction50;
    QAction* m_zoomOutAction25;
    QAction* m_zoomInAction;
    float m_zoomToggleFactor;
    QTabBar* m_tabs;
    bool m_pauseAfterOpen;
    int m_monitorScreen;
    QWidget* m_videoWidget;
    QHBoxLayout* m_videoLayout;
    QVBoxLayout* m_playerLayout;
    QWidget* m_videoScrollWidget;
    const TransportControllable* m_currentTransport;
    QPushButton * m_statusLabel;
    QPropertyAnimation* m_statusFadeIn;
    QPropertyAnimation* m_statusFadeOut;
    QTimer m_statusTimer;

    QPushButton *m_fitButton;           // 合适
    QPushButton *m_fullScreenButton;    // 全屏
    QPushButton *m_zoomSetButton;
    QFrame      *m_zoomPopup;
    QMenu       *m_zoomMenu;

    bool m_bPlayedOrStoped;
    bool m_bMute;

private slots:
    void updateSelection();
    void onInChanged(int in);
    void onOutChanged(int out);
    void on_actionSkipNext_triggered();
    void on_actionSkipPrevious_triggered();
    void on_actionVolume_triggered();
    void onMuteButtonToggled(bool bChecked);
    void setZoom(float factor);//, const QIcon &icon);
    void toggleZoom(bool checked);
    void onFadeOutFinished();
    void ZoomChanged(int index);

    void zoomPlayer(float fZoomFactor);

    void onFitButtonClicked();
    void onFullScreenButtonClicked();
    void onShowZoomMenu();
    void onZoomActionTriggered();

    void onShowVolumeSlider();

    void checkCroppedOfCurrentProducer();
};

#endif // PLAYER_H
