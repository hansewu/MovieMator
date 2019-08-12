/*
 * Copyright (c) 2013-2016 Meltytech, LLC
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

#ifndef MULTITRACKMODEL_H
#define MULTITRACKMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QString>
#include <MltTractor.h>
#include <MltPlaylist.h>

typedef enum {
    PlaylistTrackType = 0,
    BlackTrackType,
    SilentTrackType,
    AudioTrackType, //3
    VideoTrackType, //4
    FilterTrackType, //5
    TextTrackType,   //6
    VoiceTrackType,
    SoundFxTrackType,
    OverlayTrackType
} TrackType;

typedef struct {
    TrackType type;
    int number;
    int mlt_index;
} Track;

typedef QList<Track> TrackList;

typedef struct {
    // 选中的各个 clip序号(只有 1个剪辑)
    QList<int> selectedClips;
    // 被选中的轨道序号
    int selectedTrack;
    // 多轨道选中（只能选中一个轨道）
    bool isMultitrackSelected;
} Selection;


bool ProducerIsTimewarp( Mlt::Producer* producer );
char* GetFilenameFromProducer( Mlt::Producer* producer );
double GetSpeedFromProducer( Mlt::Producer* producer );


class MultitrackModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(int trackHeight READ trackHeight WRITE setTrackHeight NOTIFY trackHeightChanged)
    Q_PROPERTY(double scaleFactor READ scaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged)

public:
    /// Two level model: tracks and clips on track
    enum {
        NameRole = Qt::UserRole + 1,
        ResourceRole,    /// clip only
        ServiceRole,     /// clip only
        IsBlankRole,     /// clip only
        StartRole,       /// clip only
        DurationRole,
        InPointRole,     /// clip only
        OutPointRole,    /// clip only
        FramerateRole,   /// clip only
        IsMuteRole,      /// track only
        IsHiddenRole,    /// track only
        IsAudioRole,
        AudioLevelsRole, /// clip only
        IsCompositeRole, /// track only
        IsLockedRole,    /// track only
        FadeInRole,      /// clip only
        FadeOutRole,     /// clip only
        IsTransitionRole,/// clip only
        FileHashRole,    /// clip only
        SpeedRole,        /// clip only
        IsVideoRole,    ///
        IsFilterRole,    ///
        IsTextRole,
        TrackTypeRole,
        IsDefaultTrackRole,
        ThumbnailRole,
        HasFilterRole,
        IsAnimStickerRole
    };

    explicit MultitrackModel(QObject *parent = nullptr);
    ~MultitrackModel();

    Mlt::Tractor* tractor() const { return m_tractor; }
    const TrackList& trackList() const { return m_trackList; }

//    根据QAbstractItemModel生成数据模型必须有以下几个函数
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column = 0,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    QHash<int, QByteArray> roleNames() const;

    void audioLevelsReady(const QModelIndex &index);

    bool createIfNeeded();  //    如果还没有轨道列表m_tractor的话就初始化一个
    void addBackgroundTrack();  //添加空白轨道
    int addAudioTrack();    //添加音频轨道
    int addVideoTrack();    //添加视频轨道
    int addFilterTrack();
    int addTextTrack();
    void renumberOtherTracks(const Track& track);   //轨道发生改变之后重新刷新轨道的数字number并修改名称
    void removeTrack(int trackIndex);   //移除轨道
    void load();
    Q_INVOKABLE void reload(bool asynchronous = false);
    void close(); //清空轨道列表
    int clipIndex(int trackIndex, int position); //获取指定轨道指定位置的clip的index
    bool trimClipInValid(int trackIndex, int clipIndex, int delta, bool ripple);
    bool trimClipOutValid(int trackIndex, int clipIndex, int delta, bool ripple);
    int trackHeight() const;  //获取轨道高度，没获取到就用默认值
    void setTrackHeight(int height); //设置轨道高度
    double scaleFactor() const;  //获取时间线缩放比率
    void setScaleFactor(double scale);  //设置时间线缩放比率，并发送信号
    bool isTransition(Mlt::Playlist& playlist, int clipIndex) const;  //判断是否为转场
    void insertTrack(int trackIndex, TrackType type = VideoTrackType);  //插入轨道
    void insertOrAdjustBlankAt(QList<int> tracks, int position, int length);    //插入clip之后调整轨道的布局
    int  getTrackType(int trackIndex);  //获取轨道的类型

    bool hasFilterApplied(Mlt::Producer *producer) const;   //判断该片段是否添加了滤镜

    int getStartPositionOfClip(int trackIndex, int clipIndex);  //获取clip的开始位置

    void setClipSpeed(int trackIndex, int clipIndex, double speed);   //设置clip的速度

    Mlt::Producer* producer(Mlt::Producer *producer, Mlt::Profile& profile, double speed);  //根据条件重新深度复制一个producer

    Mlt::Producer* copiedProducer();
    void setCopiedProducer(Mlt::Producer *pProducer);

    Mlt::Producer* selectedProducer();
    void setSelectedProducer(Mlt::Producer *pProducer);

    //设置转场属性
    void setTransitionProperty(int trackIndex, int clipIndex, const QString& transitionName, const QString& propertyName, const QString& propertyValue);

    Selection selection() { return m_selection; }
    void setSelection(Selection aSelection) { m_selection = aSelection; }

signals:
    void created();
    void loaded();
    void closed();
    void modified();
    void seeked(int position);
    void trackHeightChanged();
    void scaleFactorChanged();
    void showStatusMessage(QString);
    void durationChanged();
    void producerChanged(Mlt::Producer*);
    void reloadRequested();

public slots:
    void refreshTrackList();    //刷新轨道列表
    void setTrackName(int row, const QString &value);   //设置轨道名称
    void setTrackMute(int row, bool mute);  //设置轨道音频是否可听见
    void setTrackHidden(int row, bool hidden);  //设置轨道视频是否可见
    void setTrackComposite(int row, Qt::CheckState composite);
    void setTrackLock(int row, bool lock);//设置轨道锁定状态
    int trimClipIn(int trackIndex, int clipIndex, int delta, bool ripple);
    void notifyClipIn(int trackIndex, int clipIndex);
    int trimClipOut(int trackIndex, int clipIndex, int delta, bool ripple);
    void notifyClipOut(int trackIndex, int clipIndex);
    bool moveClipValid(int fromTrack, int toTrack, int clipIndex, int position);//判断clip的移动是否合法
    bool moveClip(int fromTrack, int toTrack, int clipIndex, int position);//移动clip
    int overwriteClip(int trackIndex, Mlt::Producer& clip, int position, bool seek = true);//覆盖clip
    QString overwrite(int trackIndex, Mlt::Producer& clip, int position, bool seek = true);
    void updateTransition(int trackIndex, int clipIndex); //更新转场
    int insertClip(int trackIndex, Mlt::Producer& clip, int position);//插入clip
    int appendClip(int trackIndex, Mlt::Producer &clip);//拓展clip
    void removeClip(int trackIndex, int clipIndex);//移除clip
    void liftClip(int trackIndex, int clipIndex);//选中高亮显示clip
    void splitClip(int trackIndex, int clipIndex, int position);//切分clip
    void joinClips(int trackIndex, int clipIndex);//连接合并clip
    void appendFromPlaylist(Mlt::Playlist* playlist, int trackIndex);
    void overwriteFromPlaylist(Mlt::Playlist& playlist, int trackIndex, int position);
    void AttachedfilterChanged(int trackIndex, int clipIndex);//已选滤镜发生变化
    void fadeIn(int trackIndex, int clipIndex, int duration);//淡入
    void fadeOut(int trackIndex, int clipIndex, int duration);//淡出
    bool addTransitionValid(int fromTrack, int toTrack, int clipIndex, int position);
    int addTransition(int trackIndex, int clipIndex, int position);//增加转场
    void removeTransition(int trackIndex, int clipIndex);//删除转场
    void removeTransitionByTrimIn(int trackIndex, int clipIndex, int delta);
    void removeTransitionByTrimOut(int trackIndex, int clipIndex, int delta);
    bool trimTransitionInValid(int trackIndex, int clipIndex, int delta);
    void trimTransitionIn(int trackIndex, int clipIndex, int delta);
    bool trimTransitionOutValid(int trackIndex, int clipIndex, int delta);
    void trimTransitionOut(int trackIndex, int clipIndex, int delta);
    bool addTransitionByTrimInValid(int trackIndex, int clipIndex, int delta);
    void addTransitionByTrimIn(int trackIndex, int clipIndex, int delta);
    bool addTransitionByTrimOutValid(int trackIndex, int clipIndex, int delta);
    void addTransitionByTrimOut(int trackIndex, int clipIndex, int delta);

    //OK返回插入位置，否则返回－1
    int moveInsertClipValid(int fromTrack, int toTrack, int clipIndex, int position);
    int moveInsertClip(int fromTrack, int toTrack, int clipIndex, int position);
    //test:add filter to timeline
    void addFilter(int trackIndex, Mlt::Producer *filter);
    //test:add text to timeline
    void addText(int trackIndex, Mlt::Producer *filter);
    //    设置转场持续时间
    void setTransitionDuration(int trackIndex, int clipIndex, int duration);
    void addMixReferences(int trackIndex, int clipIndex);

    int refreshClipFromXmlForFilter(int trackIndex, int clipIndex, QString strXml);

    void debugPrintState();

private:
    Mlt::Tractor* m_tractor;
    TrackList m_trackList;
    bool m_isMakingTransition;
    Selection m_selection;

    QScopedPointer<Mlt::Producer> m_copiedProducer;     // 保存时间线轨道上复制的 clip
    // 保存时间线轨道上当前选中的 clip
    // 只是给预览后添加滤镜用的，没有其他用途
    QScopedPointer<Mlt::Producer> m_selectedProducer;


    bool moveClipToTrack(int fromTrack, int toTrack, int clipIndex, int position);
    void moveClipToEnd(Mlt::Playlist& playlist, int trackIndex, int clipIndex, int position);//将clip移动到轨道末尾
    void relocateClip(Mlt::Playlist& playlist, int trackIndex, int clipIndex, int position);
    void moveClipInBlank(Mlt::Playlist& playlist, int trackIndex, int clipIndex, int position);
    void consolidateBlanks(Mlt::Playlist& playlist, int trackIndex);
    void consolidateBlanksAllTracks();
    void getAudioLevels();
    void addBlackTrackIfNeeded();
    void convertOldDoc();
    Mlt::Transition* getTransition(const QString& name, int mltIndex) const;
    Mlt::Filter* getFilter(const QString& name, int trackIndex) const;
    Mlt::Filter* getFilter(const QString& name, Mlt::Service* service) const;
    void removeBlankPlaceholder(Mlt::Playlist& playlist, int trackIndex);
    void retainPlaylist();
    void loadPlaylist();
    void removeRegion(int trackIndex, int position, int length);
    void clearMixReferences(int trackIndex, int clipIndex);
    void initMixReferences();

    bool checkClip(Mlt::Producer &clip);
    friend class UndoHelper;
    Mlt::Producer *getClipProducer(int trackIndex, int clipIndex);
    Mlt::Transition *getClipTransition(int trackIndex, int clipIndex, const QString& transitionName);
    //    给轨道重新命名
    void renameTrack(Track t, int trackIndex,bool emitSignal);

private slots:
    void adjustBackgroundDuration();

};

#endif // MULTITRACKMODEL_H
