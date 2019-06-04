/*
 * Copyright (c) 2014-2016 Meltytech, LLC
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

#ifndef METADATAMODEL_H
#define METADATAMODEL_H

#include <QAbstractListModel>
#include <QList>

class QmlMetadata;

//存放已加载到moviemator中的所有滤镜的元数据类
class MetadataModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(MetadataFilter)
    Q_PROPERTY(MetadataFilter filter READ filter WRITE setFilter NOTIFY filterChanged)

public:

    //定义model中数据的类型
    enum ModelRoles {
        NameRole = Qt::UserRole + 1,
        HiddenRole,
        FavoriteRole,
        ServiceRole,
        IsAudioRole,
        NeedsGpuRole,
        VisibleRole,
        ThumbnailRole,
        NeedsProVersionRole,
        FreeVersionRole,
        FilterTypeDisplayRole,
    };

    //定义已加载的率类型的枚举类型
    enum MetadataFilter {
        NoFilter,
        FavoritesFilter,
        VideoFilter,
        AudioFilter
    };

    explicit MetadataModel(QObject *parent = 0);

    //重写基类QAbstractListModel的方法，进行model中的数据操作
    // Implement QAbstractListModel
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QHash<int, QByteArray> roleNames() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    // Direct access to QmlMetadata
    //添加一个滤镜的QmlMetadata到model的末尾
    void add(QmlMetadata* data);
    //获取model中的第index个滤镜的元数据
    Q_INVOKABLE QmlMetadata* get(int index) const;
    //获取当前filter的类型（音频、视频、喜爱的滤镜）
    MetadataFilter filter() const { return m_filter; }
    //设置当前filter的类型
    void setFilter(MetadataFilter);
    //获取model中的第row个滤镜是否可见
    Q_INVOKABLE bool isVisible(int row) const;
    void setIsClipProducer(bool isClipProducer);

signals:
    void filterChanged();

private:
    //定义一个存放QmlMetadata对象指针的list类型
    typedef QList<QmlMetadata*> MetadataList;
    //存放已加载到moviemator中的所有滤镜的元数据
    MetadataList m_list;
    //当前滤镜的类型
    MetadataFilter m_filter;
    bool m_isClipProducer;
};

#endif // METADATAMODEL_H
