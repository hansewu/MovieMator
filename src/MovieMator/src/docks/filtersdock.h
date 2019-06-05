/*
 * Copyright (c) 2013-2014 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 * Author: Brian Matherly <code@brianmatherly.com>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: Dragon-S <15919917852@163.com>
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

#ifndef FILTERSDOCK_H
#define FILTERSDOCK_H

#include <QDockWidget>
#include <QObject>
#include <QQuickView>
#include <QQuickWidget>
#include <QQuickView>

class QmlFilter;
class QmlMetadata;
class MetadataModel;
class AttachedFiltersModel;

//主界面右侧属性中的的滤镜dock（音频滤镜、视频滤镜）
class FiltersDock : public QDockWidget
{
    Q_OBJECT
    Q_PROPERTY(QRect dockPosition READ dockPosition NOTIFY dockPositionChanged)
public:
    explicit FiltersDock(MetadataModel* metadataModel, AttachedFiltersModel* attachedModel,bool isvideo, QWidget *parent = 0);
    int position() const { return m_position; }

//    Q_INVOKABLE int getCurrentPosition();
//#ifdef MOVIEMATOR_PRO
    Q_PROPERTY(int position READ position WRITE setPosition);
    void setPosition(int position);
    //接收时间线发送的positionChangedForKeyFrame信号，并保存当前位置，并发出位置改变的信号
    void setCurrentClipFrameFromTimeline(int position);
//#endif
    Q_PROPERTY(bool proVersion READ proVersion)
    //获取是否Pro版
    bool proVersion();

    //显示提示升级窗口
    Q_INVOKABLE void promptForUpgrade();

    //设置filtersdock相关的QML的上下文
    void setExtraQmlContextProperty(QString name, QObject *object);
    //filtersdock的坐标
    QRect dockPosition();

    //是否videofilter dock
    bool isVideo() const { return m_isvideo; }
    void setIsVideo(bool isvideo) { m_isvideo = isvideo; }

signals:
    //添加滤镜后发出的信号
    void currentFilterRequested(int attachedIndex);
    //当filter的参数发生变化时发出此信号
    void filterParameterChanged(); /// Notifies when a filter parameter changes.
    //时间线上位置发生变化时发出此信号给filter对应的qml，使其做相应的更新
    void positionChanged();
//#ifdef MOVIEMATOR_PRO
    void changePosition(int position);
//#endif
    //filtersdock的位置变化时发出此信号，通知qml
    void dockPositionChanged();
    //filtersdock中的当前选中的filter发生改变时发出此信号，通知qml相应的处理（更新关键帧等）
    void currentFilterChanged(); //current fiter changed

public slots:
    //清除当前的filter，主要时清除filter的元数据，及清除与qml的连接，清除对应滤镜的参数显示
    void clearCurrentFilter(int index);
    //设置当前的filter及其元数据，主要是给qml设置，并通知更新
    void setCurrentFilter(QObject* filter, QmlMetadata* meta, int index);
    //接受timelinedock的fadeInChanged信号，设置淡入时长，设置给qml
    void setFadeInDuration(int duration);
    //设置淡出时长，设置给qml
    void setFadeOutDuration(int duration);
    //接收timeline发送的位置改变信号（positionChanged），并转发给filter相关的qml
    void onChangePosition();
    //接收filtersdock发生可见变化信号，并通知qml重新选中滤镜，切换dock时调用
    void onSelectedTabChanged(bool visible);

protected:
    //重写QDockWidget的event函数，目前未作任何处理
    bool event(QEvent *event);

private:
    //获取filtersdock的qml上下文
    QQmlContext* getRootContext();
    //获取filtersdock的根对象
    QQuickItem* getRootObject();
    //重置QquickWidget，即重新连接对应的qml源文件（filterview.qml），Windows用此函数
    void resetQquickWidget();
    //重置QquickView，即重新连接对应的qml源文件（filterview.qml），Mac用此函数
    void resetQquickView();

private slots:
    //重置filtersdock对应的qml源文件，实际通过调用resetQquickWidget、resetQquickView实现
    void resetQview();

private:
    int m_position;
    //对应filter的qml对象
    QmlFilter *m_qmlFilter;

    //filtersdock的View，主要是Windows上使用
    QQuickView m_quickView;
    //用于包裹m_quickView的widget，然后放入dock，主要是Windows上使用
    QWidget *m_containerView;
    //filtersdock的View，主要是Mac上使用
    QQuickWidget m_qview;
    //是否是视频滤镜
    bool m_isvideo;
};

#endif // FILTERSDOCK_H
