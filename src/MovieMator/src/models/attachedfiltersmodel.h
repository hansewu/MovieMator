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

#ifndef ATTACHEDFILTERSMODEL_H
#define ATTACHEDFILTERSMODEL_H

#include <QAbstractListModel>
#include <MltFilter.h>
#include <MltProducer.h>
#include <MltEvent.h>

class QmlMetadata;

//存储已添加到producer上的filter元数据类
class AttachedFiltersModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)
    Q_PROPERTY(QString producerTitle READ producerTitle NOTIFY trackTitleChanged)
    Q_PROPERTY(bool isProducerSelected READ isProducerSelected NOTIFY isProducerSelectedChanged)
    Q_ENUMS(AttachedMetadataFilter)
    Q_PROPERTY(AttachedMetadataFilter filter READ filter WRITE setFilter NOTIFY filterChanged)

public:
    enum ModelRoles {
        TypeDisplayRole = Qt::UserRole + 1,
        ThumbnailRole,
    };

    //添加到AttachedFiltersModel中的filter的类型
    enum AttachedMetadataFilter {
        NoFilter,
        VideoFilter,
        AudioFilter
    };

    explicit AttachedFiltersModel(QObject *parent = nullptr);

    //是否可以对AttachedFiltersModel中的滤镜操作（添加、删除等）
    bool isReady();
    //获取第row个filter
    Mlt::Filter* getFilter(int row) const;
    //从MetadataList中获取第row个QmlMetadata
    QmlMetadata* getMetadata(int row) const;

    //目前没有实质作用，直接调用的是reset函数
    void setProducer(Mlt::Producer* producer = nullptr);
    //获取当前producer的标题，
    QString producerTitle() const;
    //判断producer是否被选中
    bool isProducerSelected() const;

    // QAbstractListModel Implementation
    //重写基类QAbstractListModel的方法，进行model中的数据操作（删除、插入、移动等）
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    QHash<int, QByteArray> roleNames() const;
    Qt::DropActions supportedDropActions() const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);
    bool moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationRow);
    QScopedPointer<Mlt::Producer> m_producer;

    //获取已添加的filter元数据的类型（视频滤镜、音频滤镜或其他）
    AttachedMetadataFilter filter() const { return m_filter; }
    //设置filter的类型
    void setFilter(AttachedMetadataFilter);
    //检测第row个滤镜是否是视频滤镜
    Q_INVOKABLE bool isVisible(int row) const;

signals:
    //当m_metaList中的数据发生变化时（添加、删除、移动等），发出此信号
    void changed();
    //暂未使用
    void readyChanged();
    //重复添加滤镜失败后发出此信号
    void duplicateAddFailed(int index);
    //轨道的标题发生改变时发出此信号，暂未使用
    void trackTitleChanged();
    //producer选中状态发生改变时发出此信号，暂未使用
    void isProducerSelectedChanged();
    //filter的类型发生改变时发出此信号
    void filterChanged();

    void filtersLoaded();
    void filtersLoadedForTextTemplateProducer(int nTextFilterIndex);

    void filterAdded(int nAttachedFilterIndex);
    void filterRemoved(int nAttachedFilterIndex);


public slots:
    //添加filter到m_metaList中，通过QmlMetadata创建新的filter设置到当前的producer上，并将meta保存到m_metaList中，
    //如果是非unod、redo过来的操作则不添加添加滤镜操作到undostack。暂无信号接收处理，直接处理手动操作即undo、redo操作。
    void add(QmlMetadata* meta, bool bFromUndo = false);
    //移除已添加到m_metaList中的第row个滤镜，暂无信号接收处理，目前只处理qml来的手动操作即undo、redo的移除滤镜操作
    void remove(int row, bool bFromUndo = false);
    //移动已添加到m_metaList中的第toRow个滤镜到fromRow位置，目前只处理qml来的手动操作即undo、redo的移除滤镜操作
    bool move(int fromRow, int toRow, bool bFromUndo = false);

private:
    //此函数为空函数，暂未使用—
    static void producerChanged(mlt_properties owner, AttachedFiltersModel* model);
    //当前的producer发生改变时，重置attachedfiltersmodel
    void reset(Mlt::Producer *producer = nullptr);

    //给新添加的滤镜设置缺省参数
    void setDefaultValueForAllParemeters(Mlt::Filter* pFilter,QmlMetadata* pMetadata);

    //加载producer的滤镜时发送消息
    void emitFiltersLoaded();

    //拖拽的filter的索引
    int m_dropRow;
    //移除的filter的索引
    int m_removeRow;

    //当对producer上的filter进行添加、删除、移动操作时，需要使用此事件进行加锁
    QScopedPointer<Mlt::Event> m_event;
    //定义一个存放QmlMetadata类型对象的list类型
    typedef QList<QmlMetadata*> MetadataList;
    //用于存放已添加到当家producer上的所有滤镜的源数据信息
    MetadataList m_metaList;
    //定义一个存放int类型数据的list类型
    typedef QList<int> IndexMap;
    //用于存放已添加到producer中的滤镜对应在mlt中的索引
    IndexMap m_mltIndexMap;

    //当前filter的类型
    AttachedMetadataFilter m_filter;
};

#endif // ATTACHEDFILTERSMODEL_H
