/*
 * Copyright (c) 2013-2015 Meltytech, LLC
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

#ifndef COMMANDS_H
#define COMMANDS_H

#include "models/multitrackmodel.h"
#include "models/attachedfiltersmodel.h"
#include "docks/timelinedock.h"
#include "undohelper.h"
#include "qmlmetadata.h"
#include <QString>
#include <QObject>
#include <QTime>
#include <MltTransition.h>
#include "abstractcommand.h"

namespace Timeline
{

enum {
    UndoIdTrimClipIn,
    UndoIdTrimClipOut,
    UndoIdFadeIn,
    UndoIdFadeOut,
    UndoIdTrimTransitionIn,
    UndoIdTrimTransitionOut,
    UndoIdAddTransitionByTrimIn,
    UndoIdAddTransitionByTrimOut,

    UndoIdFilterCommand, //wzq
    UndoIdKeyFrameInsertCommand,
    UndoIdKeyFrameRemoveCommand,
    UndoIdKeyFrameUpdateCommand,
    UndoIdUpdate,
    UndoIdTransitionProperty,
    UndoIdTranstionDurationSetting
};

//拓展连接clip
class AppendClipCommand : public AbstractCommand
{
public:
    AppendClipCommand(MultitrackModel& model, int trackIndex, const QString& xml, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    QString m_xml;
    UndoHelper m_undoHelper;
};

//插入clip
class InsertClipCommand : public AbstractCommand
{
public:
    InsertClipCommand(MultitrackModel& model, int trackIndex, int position, const QString &xml, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_position;
    QString m_xml;
    QStringList m_oldTracks;
    UndoHelper m_undoHelper;
};

//覆盖clip
class OverwriteClipCommand : public AbstractCommand
{
public:
    OverwriteClipCommand(MultitrackModel& model, int trackIndex, int position, const QString &xml, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    QString m_playlistXml;
    int m_position;
    QString m_xml;
    UndoHelper m_undoHelper;
};

//移除clip
class LiftClipCommand : public AbstractCommand
{
public:
    LiftClipCommand(MultitrackModel& model, TimelineDock &timeline, int trackIndex, int clipIndex, const QString &xml, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    QString m_xml;
    UndoHelper m_undoHelper;
    TimelineDock &m_timeline;
};

//移除clip
class RemoveClipCommand : public AbstractCommand
{
public:
    RemoveClipCommand(MultitrackModel& model, TimelineDock& timeline, int trackIndex, int clipIndex, const QString &xml, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    QString m_xml;
    UndoHelper m_undoHelper;
    TimelineDock& m_timeline;
};

//设置轨道名字
class NameTrackCommand : public AbstractCommand
{
public:
    NameTrackCommand(MultitrackModel& model, int trackIndex, const QString& name, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    QString m_name;
    QString m_oldName;
};

//设置声音是否可听见
class MuteTrackCommand : public AbstractCommand
{
public:
    MuteTrackCommand(MultitrackModel& model, int trackIndex, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    bool m_oldValue;
};

//隐藏轨道上的片段
class HideTrackCommand : public AbstractCommand
{
public:
    HideTrackCommand(MultitrackModel& model, int trackIndex, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    bool m_oldValue;
};

class CompositeTrackCommand : public AbstractCommand
{
public:
    CompositeTrackCommand(MultitrackModel& model, int trackIndex, Qt::CheckState value, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    Qt::CheckState m_value;
    Qt::CheckState m_oldValue;
};

//锁定轨道
class LockTrackCommand : public AbstractCommand
{
public:
    LockTrackCommand(MultitrackModel& model, int trackIndex, bool value, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    bool m_value;
    bool m_oldValue;
};

//拖动clip
class MoveClipCommand : public AbstractCommand
{
public:
    MoveClipCommand(MultitrackModel& model, int fromTrackIndex, int toTrackIndex, int clipIndex, int position, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_fromTrackIndex;
    int m_toTrackIndex;
    int m_fromClipIndex;
    int m_fromStart;
    int m_toStart;
    UndoHelper m_undoHelper;
};

class TrimClipInCommand : public AbstractCommand
{
public:
    TrimClipInCommand(MultitrackModel& model, int trackIndex, int clipIndex, int delta, bool ripple, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
protected:
    int id() const { return UndoIdTrimClipIn; }
//    using AbstractCommand::mergeWith;
    bool mergeWith(const QUndoCommand *other);
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_originalClipIndex;
    int m_delta;
    bool m_ripple;
    UndoHelper m_undoHelper;
};

class TrimClipOutCommand : public AbstractCommand
{
public:
    TrimClipOutCommand(MultitrackModel& model, int trackIndex, int clipIndex, int delta, bool ripple, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
protected:
    int id() const { return UndoIdTrimClipOut; }
    //using AbstractCommand::mergeWith;
    bool mergeWith(const QUndoCommand *other);
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    bool m_ripple;
    UndoHelper m_undoHelper;
};

//切分
class SplitCommand : public AbstractCommand
{
public:
    SplitCommand(MultitrackModel& model, int trackIndex, int clipIndex, int position, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_position;
};

//淡入
class FadeInCommand : public AbstractCommand
{
public:
    FadeInCommand(MultitrackModel& model, int trackIndex, int clipIndex, int duration, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
protected:
    int id() const { return UndoIdFadeIn; }
//    using AbstractCommand::mergeWith;
    bool mergeWith(const QUndoCommand *other);
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_duration;
    int m_previous;
};

//淡出
class FadeOutCommand : public AbstractCommand
{
public:
    FadeOutCommand(MultitrackModel& model, int trackIndex, int clipIndex, int duration, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
protected:
    int id() const { return UndoIdFadeOut; }
//    using AbstractCommand::mergeWith;
    bool mergeWith(const QUndoCommand *other);
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_duration;
    int m_previous;
};

//添加转场
class AddTransitionCommand : public AbstractCommand
{
public:
    AddTransitionCommand(MultitrackModel& model, int trackIndex, int clipIndex, int position, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_position;
    int m_transitionIndex;
    UndoHelper m_undoHelper;
};

class TrimTransitionInCommand : public AbstractCommand
{
public:
    TrimTransitionInCommand(MultitrackModel& model, int trackIndex, int clipIndex, int delta, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
protected:
    int id() const { return UndoIdTrimTransitionIn; }
//    using AbstractCommand::mergeWith;
    bool mergeWith(const QUndoCommand *other);
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    bool m_notify;
};

class TrimTransitionOutCommand : public AbstractCommand
{
public:
    TrimTransitionOutCommand(MultitrackModel& model, int trackIndex, int clipIndex, int delta, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
protected:
    int id() const { return UndoIdTrimTransitionOut; }
//    using AbstractCommand::mergeWith;
    bool mergeWith(const QUndoCommand *other);
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    bool m_notify;
};

class AddTransitionByTrimInCommand : public AbstractCommand
{
public:
    AddTransitionByTrimInCommand(MultitrackModel& model, int trackIndex, int clipIndex, int delta, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
protected:
    int id() const { return UndoIdAddTransitionByTrimIn; }
//    using AbstractCommand::mergeWith;
    bool mergeWith(const QUndoCommand *other);
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    bool m_notify;
};

class AddTransitionByTrimOutCommand : public AbstractCommand
{
public:
    AddTransitionByTrimOutCommand(MultitrackModel& model, int trackIndex, int clipIndex, int delta, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
protected:
    int id() const { return UndoIdAddTransitionByTrimOut; }
//    using AbstractCommand::mergeWith;
    bool mergeWith(const QUndoCommand *other);
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    bool m_notify;
};

//添加轨道
class AddTrackCommand: public AbstractCommand
{
public:
    AddTrackCommand(MultitrackModel& model, TrackType trackType, AbstractCommand* parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    bool m_isVideo;
    TrackType m_trackType;
};

//插入轨道
class InsertTrackCommand : public AbstractCommand
{
public:
    InsertTrackCommand(MultitrackModel& model, int trackIndex, AbstractCommand* parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    TrackType m_trackType;
};

//移除轨道
class RemoveTrackCommand : public AbstractCommand
{
public:
    RemoveTrackCommand(MultitrackModel& model, int trackIndex, AbstractCommand* parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int m_trackIndex;
    QString m_xml;
    TrackType m_trackType;
    QString m_trackName;
};

class ChangeBlendModeCommand : public QObject, public AbstractCommand
{
    Q_OBJECT
public:
    ChangeBlendModeCommand(MultitrackModel& model, Mlt::Transition& transition, const QString& propertyName, const QString& mode, AbstractCommand* parent = nullptr);
    void redo_impl();
    void undo_impl();
signals:
    void modeChanged(QString& mode);
private:
    Mlt::Transition m_transition;
    QString m_propertyName;
    QString m_newMode;
    QString m_oldMode;
};

//更新
class UpdateClipCommand : public AbstractCommand
{
public:
    UpdateClipCommand(TimelineDock& timeline, MultitrackModel& model, int trackIndex, int clipIndex, int position,
        AbstractCommand * parent = nullptr);
    void setXmlAfter(const QString& xml) { m_xmlAfter = xml; }
    void setSpeedChanged(bool isSpeedChanged) {m_isSpeedChanged = isSpeedChanged;}
    void redo_impl();
    void undo_impl();
private:
    TimelineDock& m_timeline;
    int m_trackIndex;
    int m_clipIndex;
    int m_position;
    QString m_xmlAfter;
    bool m_isFirstRedo;
    bool m_isSpeedChanged;
    UndoHelper m_undoHelper;
};


//移除转场
class RemoveTransitionCommand: public AbstractCommand
{
public:
    RemoveTransitionCommand(MultitrackModel &model, int trackIndex, int clipIndex, int transitionIndex, int position, AbstractCommand *parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_transitionIndex;
    int m_position;
    UndoHelper m_undoHelper;
};

//移除clip上的转场
class RemoveTransitionsOnClipCommand: public AbstractCommand
{
public:
    RemoveTransitionsOnClipCommand(MultitrackModel &model, TimelineDock &timeline, int trackIndex, int clipIndex, AbstractCommand *parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel &m_model;
    TimelineDock &m_timeline;
    int m_trackIndex;
    int m_clipIndex;
    UndoHelper m_undoHelper;
};

//拖动文件插入到时间线上
class MoveInsertClipCommand:public AbstractCommand
{
public:
    MoveInsertClipCommand(MultitrackModel &model, int fromTrack, int toTrack, int clipIndex, int position, AbstractCommand *parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel &m_model;
    int m_fromTrackIndex;
    int m_toTrackIndex;
    int m_clipIndex;
    int m_position;
    UndoHelper m_undoHelper;
};


class FilterClipCommand : public AbstractCommand
{
public:
    FilterClipCommand(MultitrackModel& model, int TrackIndex, int clipIndex, QString strFromXml, QString strToXml, AbstractCommand * parent = nullptr);
    void redo_impl();
    void undo_impl();
private:
    MultitrackModel& m_model;
    int     m_trackIndex;
    int     m_clipIndex;
    QString m_strFromXml;
    QString m_strToXml;
    UndoHelper m_undoHelper;
};


//滤镜参数设置
class FilterCommand: public AbstractCommand
{

public:
    FilterCommand(MultitrackModel& model, AttachedFiltersModel& attachedFiltersModel, int nFilterIndex, QString strFilterKeyName,  double dFromValue, double dToValue, bool bIsFirstExec,AbstractCommand * pParent= nullptr);
    FilterCommand(MultitrackModel& model, AttachedFiltersModel& attachedFiltersModel, int nFilterIndex, QString strFilterKeyName,  int nFromValue, int nToValue, bool bIsFirstExec,AbstractCommand * pParent= nullptr);
    FilterCommand(MultitrackModel& model, AttachedFiltersModel& attachedFiltersModel, int nFilterIndex, QString strFilterKeyName,  QString strFromValue, QString strToValue, bool bIsFirstExec,AbstractCommand * pParent= nullptr);
    FilterCommand(MultitrackModel& model, AttachedFiltersModel& attachedFiltersModel, int nFilterIndex, QString strFilterKeyName,  QRectF rectFromValue, QRectF rectToValue, bool bIsFirstExec,AbstractCommand * pParent= nullptr);

    ~FilterCommand();
    void redo_impl();
    void undo_impl();
protected:
    int id() const { return UndoIdFilterCommand; }
//    using AbstractCommand::mergeWith;
    bool mergeWith(const QUndoCommand *pOtherCommand);

    int transitionValue(QVariant &varFrom, QVariant &varTo, QString strName,  double dFromValue, double dToValue);

    void notify();
    void setValue(QVariant value);
protected:
    AttachedFiltersModel& m_attachedFiltersModel;
    int         m_nFilterIndex;
    QString     m_strKeyName;
    QVariant    m_varFromValue;
    QVariant    m_varToValue;
    bool        m_bFirstExec;
};

//选择添加滤镜
class FilterAttachCommand: public AbstractCommand
{

public:
    FilterAttachCommand(MultitrackModel& model, QmlMetadata *meta, int rowIndex, int metaIndex, bool bAdd, bool isFirst, AbstractCommand * parent= nullptr);
   // ~FilterAttachCommand();
    void redo_impl();
    void undo_impl();
protected:
//    void notify();

protected:
 //   Mlt::Filter*    m_filter;

    QmlMetadata     *m_meta;
    int             m_rowIndex;
    int             m_metaIndex;
    bool            m_bAdd;

    bool            m_bFirstExec;
};

//移动已选滤镜
class FilterMoveCommand: public AbstractCommand
{

public:
    FilterMoveCommand(MultitrackModel& model, int rowIndexFrom, int rowIndexTo, bool isFirst, AbstractCommand * parent= nullptr);
 //   ~FilterMoveCommand();
    void redo_impl();
    void undo_impl();
protected:
//    void notify();

protected:
    Mlt::Filter*    m_filter;

    int             m_rowIndexFrom;
    int             m_rowIndexTo;

    bool            m_bFirstExec;
};


//增加关键帧
class KeyFrameInsertCommand: public AbstractCommand
{
public:
    KeyFrameInsertCommand(MultitrackModel& model, Mlt::Filter* filter, const QVector<key_frame_item>  &from_value, const QVector<key_frame_item>  &insert_value, bool isFirst, AbstractCommand * parent = nullptr);
    ~KeyFrameInsertCommand();
    void redo_impl();
    void undo_impl();

protected:
    int id() const { return UndoIdKeyFrameInsertCommand; }
    bool mergeWith(const QUndoCommand *other);

private:
    Mlt::Filter* m_filter;
    QVector<key_frame_item>  m_from_value;    //存加关键帧之前的值
    QVector<key_frame_item>  m_insert_value;

    bool      m_bFirstExec;
    QTime     m_execTime;
};

//删除关键帧
class KeyFrameRemoveCommand: public AbstractCommand
{
public:
    KeyFrameRemoveCommand(MultitrackModel& model, Mlt::Filter* filter, const QVector<key_frame_item>  &remove_value, bool isFirst, AbstractCommand * parent = nullptr);
    ~KeyFrameRemoveCommand();
    void redo_impl();
    void undo_impl();

protected:
    int id() const { return UndoIdKeyFrameRemoveCommand; }
    bool mergeWith(const QUndoCommand *other);

private:
    Mlt::Filter* m_filter;
    QVector<key_frame_item>  m_remove_value;

    bool      m_bFirstExec;
    QTime     m_execTime;
};

//修改关键帧数据
class KeyFrameUpdateCommand: public AbstractCommand
{
public:
    KeyFrameUpdateCommand(MultitrackModel& model, Mlt::Filter* filter, int nFrame, QString name, QString from_value, QString to_value,bool isFirst, AbstractCommand * parent= nullptr);
    ~KeyFrameUpdateCommand();
    void redo_impl();
    void undo_impl();

protected:
    int id() const { return UndoIdKeyFrameUpdateCommand; }
    bool mergeWith(const QUndoCommand *other);

private:
    Mlt::Filter* m_filter;
    int         m_nKeyFrame;
    QString     m_sKeyName;

    QString  m_from_value;
    QString  m_to_value;

    bool      m_bFirstExec;
    QTime     m_execTime;
};


//选择clip
class ClipsSelectCommand: public AbstractCommand
{

public:
    ClipsSelectCommand(MultitrackModel& model, QList<int> newSelection, int newTrackIndex, bool isNewMultitrack,
                       QList<int> oldSelection, int oldTrackIndex, bool isOldMultitrack,
                       bool isFirst,AbstractCommand * parent= nullptr);

    void redo_impl();
    void undo_impl();

protected:
    bool            m_bFirstExec;
    QList<int>  m_newSelection, m_oldSelection;
    int         m_newTrackIndex, m_oldTrackIndex;
    bool        m_bNewIsMultitrack, m_bOldIsMultitrack;
};


//保存构造TransitionPropertyCommand的参数
typedef struct {
    int     nTrackIndex; //transition所在的轨道索引
    int     nClipIndex; //transition在轨道上的索引

    QString strTransitionName; //transition名字，视频luma、音频mix
    QString strPropertyName; //要修改的属性名
    QString strPropertyValue;
    int     nInvertTransition;
    double  dTransitionSoftness;
} TransitionPropertyCommandParameters;

//转场属性设置
class TransitionPropertyCommand: public AbstractCommand
{
public:
    TransitionPropertyCommand(TimelineDock& timeline, MultitrackModel& model, int trackIndex, int clipIndex, const QString& transitionName, const QString& propertyName, const QString& propertyValue, bool isFirst = true, int invert = 0, double softness = 0.0, AbstractCommand *parent = nullptr);
    TransitionPropertyCommand(TimelineDock& timeline, MultitrackModel& model, const TransitionPropertyCommandParameters& parameters, bool bIsFirstRedo = true, AbstractCommand *parent = nullptr);
    void redo_impl();
    void undo_impl();

protected:
    int id() const { return UndoIdTransitionProperty; }
    bool mergeWith(const QUndoCommand *other);

private:
    TimelineDock&       m_timeline;
    MultitrackModel&    m_model;
    int                 m_trackIndex;
    int                 m_clipIndex;
    QString             m_transitionName;
    QString             m_propertyName;
    QString             m_propertyValue;
    UndoHelper          m_undoHelper;
    int                 m_invert;
    double              m_softness;
    bool                m_isFirstRedo;
};


//转场时长
class TransitionDurationSettingCommand: public AbstractCommand
{
public:
    TransitionDurationSettingCommand(TimelineDock &timeline, MultitrackModel& model, int trackIndex, int clipIndex, int duration, bool isFirst, AbstractCommand *parent = nullptr);
    void redo_impl();
    void undo_impl();

protected:
    int id() const { return UndoIdTranstionDurationSetting; }
    bool mergeWith(const QUndoCommand *other);

private:
    TimelineDock&       m_timeline;
    MultitrackModel&    m_model;
    int                 m_trackIndex;
    int                 m_clipIndex;
    int                 m_duration;
    UndoHelper          m_undoHelper;
    bool                m_isFirstRedo;
};




} // namespace Timeline

#endif
