/*
 * Copyright (c) 2014 Meltytech, LLC
 * Author: Brian Matherly <code@brianmatherly.com>
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

#ifndef FILTERCONTROLLER_H
#define FILTERCONTROLLER_H

#include <QObject>
#include <QScopedPointer>
#include <QFuture>
#include "models/metadatamodel.h"
#include "models/attachedfiltersmodel.h"
#include "qmlmetadata.h"
#include "qmltypes/qmlfilter.h"
#include "resourcedockgenerator_global.h"
//#include <../frei0r.h>

class QTimerEvent;

//filter相关的逻辑控制类，包括左侧的filter列表以及右侧的filter属性控制
class FilterController : public QObject
{
    Q_OBJECT

public:
    explicit FilterController(QObject* parent = nullptr);
    //获取已加载的所有filter的元数据
    MetadataModel* metadataModel();
    //获取已添加到producer上的所有filter的元数据
    AttachedFiltersModel* attachedModel();

    //通过Mlt::Service获取QmlMetadata
    QmlMetadata* metadataForService(Mlt::Service *service);
    //通过uniqueId获取QmlMetadata，与metadataForService有重复的地方
    QmlMetadata* metadataForUniqueId(const char *uniqueId);
    //获取QmlMetadata
    QmlMetadata* getQmlMetadata(int nIndex);

    int currentFilterIndex() { return m_currentFilterIndex; }

    //获取已加载的音频滤镜信息
    QList<FilterInfo> getAudioFiltersInfo();

    //获取已加载的视频滤镜信息
    QList<FilterInfo> getVideoFiltersInfo();

signals:
    void currentFilterAboutToChange(int index);
    //当前的filter发生改变时发出的信号
    void currentFilterChanged(QObject* filter, QmlMetadata* meta, int index);
    //添加失败时，通知界面更新状态显示信息
    void statusChanged(QString);
    //filtersInfo已加载完成的信号
    void filtersInfoLoaded();

public slots:
    //接收时间线选中producer的消息（selected），并将选中的producer传递给AttachedFiltersModel
    void setProducer(Mlt::Producer *producer = nullptr);
    //接收filtersdock发送的信号（currentFilterRequested），并设置当前设置filter为第attachedIndex个filter
    void setCurrentFilter(int attachedIndex);

    //add positon&size filter
    //添加一个size&position滤镜
    void addPositionAndSizeFilter();
    //添加一个选中滤镜
    void addRotateFilter();
    //添加一个裁剪滤镜
    void addCropFilter();
    //添加一个视频淡入滤镜
    void addFadeInVideoFilter();
    //添加一个视频淡出滤镜
    void addFadeOutVideoFilter();
    //添加一个音频淡入滤镜
    void addFadeInAudioFilter();
    //添加一个音频淡出滤镜
    void addFadeOutAudioFilter();
    //添加一个调节音量滤镜
    void addVolumeFilter();
    //添加一个文字滤镜
    void addTextFilter();

    //通过滤镜的id添加一个滤镜
    void addFilter(const QString& filterID);
    //通过index添加一个滤镜，从m_metadataModel获取滤镜，并添加到m_attachedModel中
    void addFilter(int nFilterIndex = -1);
    //移除第row个filter，即从m_attachedModel删除
    void removeFilter(int row);

    //刷新当前的filter，当filter的参数或者关键帧变化时
    void refreshCurrentFilter(int filterIndex); //wzq
    //刷新滤镜的关键帧信息，当关键帧发生变化时。实际时通知qmlFilter更新关键帧信息
    void refreshKeyFrame(Mlt::Filter *filter, const QVector<key_frame_item> &listKeyFrame); //wzq
    void refreshNoAnimation(Mlt::Filter *filter, const QVector<key_frame_item> &listParameter);
    void insertKeyFrame(Mlt::Filter *filter, const QVector<key_frame_item> &listKeyFrame);
    void removeKeyFrame(Mlt::Filter *filter, const QVector<key_frame_item> &listKeyFrame);
    void updateKeyFrame(Mlt::Filter *filter, int nFrame, QString name, QString value);
private slots:
    //接收m_attachedModel发送的信号（changed），并刷新consumer
    void handleAttachedModelChange();
    //接收QAbstractItemModel的modelAboutToBeReset信号（当模型内部的状态信息失效前），设置当前滤镜的索引为-1
    void handleAttachedModelAboutToReset();
    //添加QmlMetadata到m_metadataModel中
    void addMetadata(QmlMetadata*);
    //接收m_attachedModel的rowsRemoved信号，即当m_attachedModel中有filter删除时触发，并更新当前的滤镜
    void handleAttachedRowsRemoved(const QModelIndex & parent, int first, int last);
    //接收m_attachedModel的rowsInserted信号，即当m_attachedModel中增加filter时触发，并设置插入的滤镜为当前滤镜
    void handleAttachedRowsInserted(const QModelIndex & parent, int first, int last);
    //接收m_attachedModel的duplicateAddFailed信号，即当m_attachedModel中重复添加filter失败时触发，并设置当前滤镜为index
    void handleAttachDuplicateFailed(int index);

protected:
    //重写QObject的timerEvent方法，用于接收定时器事件，进行加载filter的meta以及更新filter和关闭定时器
    void timerEvent(QTimerEvent*);

private:
    //加载所有滤镜的metadata到m_metadataModel中
    void loadFilterMetadata();
    //加载frei0r滤镜的metadata到m_metadataModel中
    void loadFrei0rFilterMetadata();
    //从文件中读取滤镜的类型到一个map中
    void readFilterTypeFromFile(QString &pFilePath, std::map<QString, QString> &filterTypes);
//    void getFrei0rPluginInfo(Mlt::Filter *filter, f0r_plugin_info_t &info);
//    void getFrei0rParamInfo(Mlt::Filter *filter, int index, f0r_param_info_t &info);

    //根据mlt_service加载滤镜参数
    void loadFilterParameter(QmlMetadata *meta);

    //获取已加载的滤镜信息，0：表示视频滤镜，1：表示音频滤镜
    QList<FilterInfo> getFiltersInfo(int nFilterType = 0);

    //获取filter的缩略图路径
    QString getFilterThumbnailPath(QString filterName, bool isAudio = false);
    //获取filter的类型
    QString getFilterType(QString filterType);
    //滤镜中文分类名
    QString getFilterClassNameZH(QString strFilterType);

    //没有使用到
    QFuture<void> m_future;
    //当前filter的QmlFilter对象
    QScopedPointer<QmlFilter> m_currentFilter;
    //所有已加载滤镜的数据
    MetadataModel m_metadataModel;
    //已填加到producer中的所有滤镜的数据
    AttachedFiltersModel m_attachedModel;
    //当前滤镜的索引
    int m_currentFilterIndex;
};

#endif // FILTERCONTROLLER_H
