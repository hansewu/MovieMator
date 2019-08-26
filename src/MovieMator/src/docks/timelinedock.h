/*
 * Copyright (c) 2013-2016 Meltytech, LLC
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

#ifndef TIMELINEDOCK_H
#define TIMELINEDOCK_H

#include <QDockWidget>
#include <QQuickWidget>
#include <QApplication>
#include "models/multitrackmodel.h"
#include "sharedframe.h"
#include "qmltypes/qmlfilter.h"

namespace Ui {
class TimelineDock;
}
namespace Timeline {
class UpdateClipCommand;
}

class TimelineDock : public QDockWidget
{
    Q_OBJECT
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int currentTrack READ currentTrack WRITE setCurrentTrack NOTIFY currentTrackChanged)
    Q_PROPERTY(QList<int> selection READ selection WRITE setSelection NOTIFY selectionChanged)
    Q_PROPERTY(QRect dockPosition READ dockPosition NOTIFY dockPositionChanged)

public:
    // 构造函数
    explicit TimelineDock(QWidget *parent = nullptr);
 //   explicit TimelineDock(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    // 析构函数
    ~TimelineDock();

    // 修剪位置？？？
    // 从入点修剪？从出点修剪？
    enum TrimLocation {
        TrimInPoint,
        TrimOutPoint
    };

    // 多个轨道模型
    MultitrackModel* model() { return &m_model; }
    // 获取播放游标的位置 m_position
    int position() const { return m_position; }
    // 设置 timelinedock播放游标位置(m_position)为 position
    void setPosition(int position);
    // 把帧数 frames转换成时间
    Q_INVOKABLE QString timecode(int frames);
    // 获取轨道 trackIndex的剪辑 clipIndex的信息 ClipInfo
    Mlt::ClipInfo* getClipInfo(int trackIndex, int clipIndex);
    // 从轨道 trackIndex的剪辑 clipIndex获取到的信息 ClipInfo中获取 Producer
    Mlt::Producer* producerForClip(int trackIndex, int clipIndex);
    // 获取轨道 trackIndex上播放游标所在位置的剪辑序号
    int clipIndexAtPlayhead(int trackIndex = -1);
    // 获取轨道 trackIndex在位置 position处的剪辑序号 index
    int clipIndexAtPosition(int trackIndex, int position);
    // 通过设定 trackIndex和 clipIndex的值来检查在位置 position处有没有剪辑
    // 主要是传值给 trackIndex和 clipIndex
    // assignIndexsByPosition()
    void chooseClipAtPosition(int position, int * trackIndex, int * clipIndex);
    // 设定轨道 currentTrack为当前轨道（传递给 qml使用）
    void setCurrentTrack(int currentTrack);
    // 从 qml界面中获取当前轨道序号
    int currentTrack() const;
    // 获取轨道 trackIndex上的剪辑数量
    int clipCount(int trackIndex) const;
    // 缩放时间线轨道之放大（调用 qml函数）
    void zoomIn();
    // 缩放时间线轨道之缩小（调用 qml函数）
    void zoomOut();
    // 重置时间线轨道为原比例大小（调用 qml函数）
    void resetZoom();
    // 鼠标滚轮事件
    //【无内容】
    void wheelEvent(QWheelEvent * event);

    // 使轨道高度变矮
    void makeTracksShorter();
    // 使轨道高度变高
    void makeTracksTaller();
    // 设置 m_selection的值
    Q_INVOKABLE void setSelection(QList<int> selection = QList<int>(), int trackIndex = -1, bool isMultitrack = false);
    // 获取 m_selection中的 selectedClips（选中的剪辑）值
    QList<int> selection() const;
    // 选中播放游标处当前轨道的剪辑
    void selectClipUnderPlayhead();
    //  获取轨道 trackIndex的剪辑 clipIndex的中间位置
    // 【无内容】
    int centerOfClip(int trackIndex, int clipIndex);
    // 轨道 trackIndex是否处于锁定状态
    bool isTrackLocked(int trackIndex) const;
    // 修剪处于播放游标的 Clip（删除空白的部分）？？？
    // 修剪播放游标处的剪辑？？？
    void trimClipAtPlayhead(TrimLocation location, bool ripple);
    // 是否波纹（ripple）（视频/音频剪辑、空白剪辑）
    bool isRipple() const;
    // 当前选中的是否含有多个轨道
    Q_INVOKABLE bool isMultitrackSelected() const { return m_model.selection().bIsMultitrackSelected; }
    // 选中的轨道的序号
    Q_INVOKABLE int selectedTrackIndex() const { return m_model.selection().nIndexOfSelectedTrack; }
    // 选中的剪辑是否为空    
    Q_INVOKABLE bool isAClipSelected() const {return m_model.selection().nIndexOfSelectedClip >= 0;} // selectedAClip()
    // 选中轨道 trackIndex在位置 position处的剪辑
    void selectClipAtPosition(int trackIndex, int position);
    // 获取当前轨道选中剪辑的位置（播放游标-剪辑起始位置）
    Q_INVOKABLE int getPositionInCurrentClip();
    // 获取当前轨道选中剪辑的长度
    Q_INVOKABLE int getCurrentClipLength();
    // 定位播放游标到选中剪辑的关键帧所在位置 position处
    // 剪辑的起始位置 + position
    Q_INVOKABLE void seekToKeyFrame(int position);

    // 获取当前轨道选中剪辑的 ClipInfo的 Producer的长度
    Q_INVOKABLE int getCurrentClipParentLength();
    // 获取当前选中剪辑的位置 position在父 Producer中的的位置
    // 如果一个剪辑切分后分成的子剪辑就会用到这个
    int getPositionOnParentProducer(int position);
    // 获取当前选中的剪辑的实际位置？？？
    int getPositionOnClip(int position);

    // 将时间 timecode转换成帧数
    Q_INVOKABLE int timeToFrames(QString timecode);
    // 发送 showFilterDock()信号
    // 【外部无调用，是否保留】
    Q_INVOKABLE void emitShowFilterDock();

    // 获取 timelinedock的坐标位置
    // 直接调用的 geometry()，getGeometry()？？？
    // 这个函数是作为 qml属性使用的，需要保留
    QRect dockPosition();


    // 设置额外的 qml属性（qml名称 name<==>qt对象 object）
    // 供外部调用
    void setExtraQmlContextProperty(QString name, QObject *object);

    // 将轨道 trackIndex的剪辑 clipIndex导出为模板
    Q_INVOKABLE void exportAsTemplate(int trackIndex, int clipIndex);
    //将选中的clip导出为xml模版
    void exportSelectedClipAsTemplate();

    // 发送 sizeAndPositionFilterSelected()信号
    void selectSizeAndPositionFilter(int index);

    // timelinecommand单元测试
    void unitTestCommand();

signals:
    // 当前轨道发生了变化
    void currentTrackChanged();
    // m_selection发生了变化
    void selectionChanged();
    // 定位到 position
    void seeked(int position);
    // 播放游标位置（m_position）发生了变化
    void positionChanged();
    // 剪辑上的 Producer被打开
    // 【有连接槽函数，但是没有地方发送这个信号】
    void clipOpened(Mlt::Producer* producer);
    // 拖动
    void dragging(const QPointF& pos, int duration);
    // 放置
    void dropped();
    // timelinedock接收到了 xml
    void dropAccepted(const QString &xml);
    // 淡入持续时间变成了 duration
    void fadeInChanged(int duration);
    // 淡出持续时间变成了 duration
    void fadeOutChanged(int duration);
    // producer被选中
    void selected(Mlt::Producer* producer);
    // 剪辑被点击
    void clipClicked();
    // 显示状态信息
    void showStatusMessage(QString);
    // 轨道 trackIndex的剪辑 clipIndex被保存到了 m_savedProducer里
    void clipCopied();
    // 显示属性
    //【没有发送此信号】
    void showProperties(bool checked);

    // 添加 PositionAndSize滤镜
    void addPositionAndSize();
    // 添加 Rotate滤镜
    void addRotate();
    // 添加 Crop滤镜
    void addCrop();
    // 添加音频淡入滤镜
    void addFadeInAudio();
    // 添加视频淡入滤镜
    void addFadeInVideo();
    // 添加音频淡出滤镜
    void addFadeOutAudio();
    // 添加视频淡出滤镜
    void addFadeOutVideo();
    // 添加音量调节滤镜
    void addVolume();
    // 滤镜被添加
    void filterAdded();
    // 添加文字滤镜
    void addText();
    // 显示 FilterDock（qml界面）
    void showFilterDock();
    // timelinedock位置被改变
    void dockPositionChanged();
    // SizeAndPosition滤镜被选中
    void sizeAndPositionFilterSelected(int index);

    //MovieMator Pro
//#ifdef MOVIEMATOR_PRO
    // 关键帧位置被改变
    // 【有连接槽函数，没有发送此信号的代码】
    void positionChangedForKeyFrame(int position);
//#endif



public slots:
    // 添加音频轨道
    void addAudioTrack();
    // 添加视频轨道
    void addVideoTrack();
    // 添加滤镜轨道
    //【没有调用此函数的代码】
    void addFilterTrack();
    // 添加文字轨道
    //【没有调用此函数的代码】
    void addTextTrack();
    // 设置播放游标当前位置为帧数 frame处
    void onShowFrame(const SharedFrame& frame);
    // 定位播放游标到位置 position处
    void onSeeked(int position);
    // 在轨道 trackIndex上添加当前播放的clip
    void append(int trackIndex);
    // 移除轨道 trackIndex上的剪辑 clipIndex
    void remove(int trackIndex, int clipIndex);
    // lift轨道 trackIndex上的剪辑 clipIndex
    void lift(int trackIndex, int clipIndex);
    // 清理 m_selection（当前轨道上）的 clips
    // withCopy：是否保存被移除的 selection
    void removeSelection(bool withCopy = false);
    // lift m_selection
    void liftSelection();
    // 选中与当前轨道间隔 by的轨道
    void selectTrack(int by);
    // 选中轨道 trackIndex的轨道头
    // 就是不往选中的轨道中存放 clips
    void selectTrackHead(int trackIndex);
    // 调用 qml函数 selectMultitrack
    void selectMultitrack();
    // 保存轨道 trackIndex的剪辑 clipIndex到 m_savedProducer里
    // 调用 MLT.setSavedProducer()
    void copyClip(int trackIndex, int clipIndex);
    // 粘贴，调用 insert
    void paste();
    // 设定轨道 trackIndex的名字为 value
    void setTrackName(int trackIndex, const QString& value);
    // 切换轨道 trackIndex的静音状态
    void toggleTrackMute(int trackIndex);
    // 切换轨道 trackIndex的隐藏状态
    void toggleTrackHidden(int trackIndex);
    // 设置轨道 trackIndex的混合模式为 composite
    //【没有调用此函数的代码】
    void setTrackComposite(int trackIndex, Qt::CheckState composite);
    // 设置轨道 trackIndex是否上锁
    void setTrackLock(int trackIndex, bool lock);
    // 从轨道 fromTrack移动剪辑 clipIndex到轨道 toTrack的位置 position
    bool moveClip(int fromTrack, int toTrack, int clipIndex, int position);
    // 修剪轨道 trackIndex上的剪辑 clipIndex
    // 添加转场，修剪转场，修剪剪辑？？？
    bool trimClipIn(int trackIndex, int clipIndex, int delta, bool ripple);
    // 修剪轨道上的剪辑（同上？？？）
    bool trimClipOut(int trackIndex, int clipIndex, int delta, bool ripple);
    // 在轨道 trackIndex的位置 position处插入 xml生成的剪辑
    void insert(int trackIndex, int position = -1, const QString &xml = QString());
    // 使用 xml生成的剪辑覆盖轨道 trackIndex在位置 position处的剪辑
    void overwrite(int trackIndex, int position = -1, const QString &xml = QString());
    // 向当前轨道添加播放列表 playlist
    void appendFromPlaylist(Mlt::Playlist* playlist);
    // 切分轨道 trackIndex上的剪辑 clipIndex
    void splitClip(int trackIndex = -1, int clipIndex = -1);
    // 附加的滤镜发生变化
    void AttachedfilterChanged();
    // 轨道 trackIndex的剪辑 clipIndex淡入 duration时间
    void fadeIn(int trackIndex, int clipIndex = -1, int duration = -1);
    // 轨道 trackIndex的剪辑 clipIndex淡出 duration时间
    void fadeOut(int trackIndex, int clipIndex = -1, int duration = -1);
    // 定位到上一段剪辑（含空白剪辑）起始处
    void seekPreviousEdit();
    // 定位到下一段剪辑（含空白剪辑）起始处
    void seekNextEdit();
    // 定位到当前轨道的剪辑 clipIndex的入点
    void seekInPoint(int clipIndex);
    // 清理 m_selection中的无效剪辑
    void clearSelectionIfInvalid();
    // 在当前轨道后插入新轨道
    void insertTrack();
    // 移除当前轨道
    void removeTrack();
    // 响应其他界面的 producerChanged()信号
    // 功能？？？
    void onProducerChanged(Mlt::Producer*);
    // 选中 m_selection第一个剪辑并发送信号
    void emitSelectedFromSelection();
    // 重做轨道 trackIndex上剪辑 clipIndex的 AudioLevels？？？？？？
    // 调节轨道 trackIndex上剪辑 clipIndex的音量？？？
    void remakeAudioLevels(int trackIndex, int clipIndex);

    // 从文件路径 path添加内容到轨道 trackIndex上
    void appendFromPath(int trackIndex, const QString &path);
    // 从 urlList添加内容到轨道 trackIndex上
    void appendFromUrls(int trackIndex, QList<QUrl> urlList);
    // 从 array添加内容到轨道 trackIndex上
    void appendFromAbstractModelItemDataList(int trackIndex, QByteArray array);


    //test:add filter to timeline
    // 添加 producer到时间线（的当前轨道）的滤镜轨道上
    void addFilterToTimeline(Mlt::Producer *producer);
    //test:add text to timeline
    // 添加 producer到时间线（的当前轨道）的文本轨道上
    void addTextToTimeline(Mlt::Producer *producer);
    // 显示轨道 trackIndex的剪辑 clipIndex的属性
    void onShowProperties(int trackIndex, int clipIndex);

    //add position&size filter to current clip
    // 给当前轨道选中的剪辑添加 PositionAndSize滤镜
    void addPositionAndSizeFilter();
    // 给当前轨道选中的剪辑添加 Rotate滤镜
    void addRotateFilter();
    // 给当前轨道选中的剪辑添加 Crop滤镜
    void addCropFilter();
    // 给当前轨道选中的剪辑添加音频淡入滤镜
    void addFadeInAudioFilter();
    // 给当前轨道选中的剪辑同时添加视频淡入和音频淡入滤镜
    void addFadeInVideoFilter();
    // 给当前轨道选中的剪辑添加音频淡出滤镜
    void addFadeOutAudioFilter();
    // 给当前轨道选中的剪辑同时添加视频淡出和音频淡出滤镜
    void addFadeOutVideoFilter();
    // 添加 Volume滤镜
    void addVolumeFilter();
    // 添加 Text滤镜
    void addTextFilter();

    // 移除轨道 trackIndex的剪辑 clipIndex上的转场
    int removeTransitionOnClip(int trackIndex, int clipIndex);
    // 带有撤销功能的从轨道 trackIndex的剪辑 clipIndex删除转场
    int removeTransitionOnClipWithUndo(int trackIndex, int clipIndex);

    // 轨道 trackIndex的剪辑 clipIndex的文本设置
    // showCurrentTextSettingWidget()无内容
    void onTextSettings(int trackIndex, int clipIndex);

    // 添加转场，append clip时调用自动添加转场
    void addTransitionOnClipAfterAppend();

    // 设定转场的持续时间为 duration
    void setTransitionDuration(int duration);

    // 移除轨道 trackIndex的转场 transitionIndex
    void removeTransition(int trackIndex, int transitionIndex);

    //MovieMator Pro
//#ifdef MOVIEMATOR_PRO
    // 根据 FiltersDock发送的信号改变播放游标的位置为 position
    void changePostionFromFilter(int position);
    // 设置当前滤镜（qml界面使用）
    void setCurrentFilter(QObject* filter, QmlMetadata* meta, int index);
//#endif
    // 无实现，是否保留
    // 【无内容，FilterClipCommand也没有任何实现】
    void addFilterClipCommand(int TrackIndex, int clipIndex, QString strFromXml, QString strToXml);

    void onSelectionChanged(TIMELINE_SELECTION selectionOld, TIMELINE_SELECTION selectionNew);

protected:
    // 拖动进入 timelinedock的事件
    void dragEnterEvent(QDragEnterEvent* event);
    // 在 timelinedock中拖动的事件
    void dragMoveEvent(QDragMoveEvent* event);
    // 拖动离开 timelinedock的事件
    void dragLeaveEvent(QDragLeaveEvent* event);
    // 放置到 timelinedock的事件
    void dropEvent(QDropEvent* event);
    // timelinedock响应的事件
    // 忽略掉 Wheel Event事件
    bool event(QEvent *event);

    // 事件过滤器（只改变 Wheel Event的行为）
    bool eventFilter( QObject * o, QEvent * e );
    // 为 timelinedock所有的滚动条安装事件过滤器
    // 【除了自身递归调用外，没有其他地方调用】
    // ？？？installEventFilterOnScroll()？？？
    void filterScrollChild(QObject* parent);

private:
    // 轨道 trackIndex的剪辑 clipIndex是否是空白（blank）
    bool isBlank(int trackIndex, int clipIndex);
    // 操作锁定的轨道时播放锁定按钮缩放的动画
    void pulseLockButtonOnTrack(int trackIndex);
    // 加载 timeline.qml界面
    void load(bool force = false);

    // ui界面
    Ui::TimelineDock *ui;
    // timelinedock主界面（timeline主要是 qml界面）
    QQuickWidget m_quickView;
    // 多个轨道模型
    MultitrackModel& m_model;
    // 播放游标位置（从 clipIndexAtPlayhead中可知）
    int m_position;
    // updateCommand
    Timeline::UpdateClipCommand* m_updateCommand;
    // 是否忽略下一个位置变化？？？
    bool m_ignoreNextPositionChange;

    // 滤镜设置界面（qml界面）
    QQuickWidget m_filterSettingsView;
    // 被选中的 Producer
    // 只有 onTextSettings()使用
    Mlt::Producer* m_selectedTextProducer;

private slots:
    // timelinedock的 visibilityChanged()信号的响应槽函数
    void onVisibilityChanged(bool visible);
};

#endif // TIMELINEDOCK_H
