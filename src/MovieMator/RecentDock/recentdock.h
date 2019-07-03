/*
 * Copyright (c) 2012 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: WanYuanCN <ebthon@hotmail.com>
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

#ifndef RECENTDOCK_H
#define RECENTDOCK_H

#include "recentdock_global.h"

#include <QDockWidget>
#include <QSortFilterProxyModel>
#include <maininterface.h>
#include "recentdockinterface.h"
#include "recentlistmodel.h"
#include "recentlistview.h"

#include <QMap>
#include <QLabel>
#include <QSpacerItem>
#include <QPushButton>

namespace Ui {
    class RecentDock;
}

class RecentDock : public QDockWidget
{
    Q_OBJECT

public:
    // 构造函数
    explicit RecentDock(MainInterface *main = nullptr, QWidget *parent = nullptr);
    // 析构函数
    ~RecentDock();
    // listView等的个数（Video、Audio、Image）
    static const int num = 3;

public slots:
    // 添加文件到对应类型的 listView中，并添加到历史记录里
    void add(const QString&);
    // 移除文件 s，并从历史记录中删除
    QString remove(const QString& s);
    // 获取选中的多个文件
    QList<FILE_HANDLE> getSelected();

private:
    // 界面大小改变事件
    void resizeEvent(QResizeEvent* event);

    // 调整 m_addToTimelineButton的位置
    void positionAddToTimelineButton();
private:
    // 界面 ui
    Ui::RecentDock *ui;
    // 历史记录（Settings.recent()）
    QStringList m_recent;
    // 主界面
    MainInterface *m_mainWindow;

    // 当前被选中的 listView项
    QModelIndex m_currentIndex;
    // 当前激活的列表（Videos/Audios/Images）
    RecentListView *m_currentListView;

    // 历史文件列表的列表
    // 每个类型的文件存放在一个列表里，这些列表又存放在一个列表里
    QList<RecentListView*> *m_listviewList;
    // 文件列表模型的列表，同上
    QList<RecentListModel*> *m_modelList;
    // 过滤器模型
    QSortFilterProxyModel *m_proxyArray[num];
    // 显示 Videos、Audios、Images的标签
    QLabel *m_labelArray[num];
    // 显示 Videos、Audios、Images标题的后面分割线
    QLabel *m_imageArray[num];
    // 文件是哪种类型
    bool m_flag[num] = {false};
    // 标签 m_labeleArray[]要显示的标题
    const QString m_itemNames[num] = {tr("Videos"), tr("Audios"), tr("Images")};

    // 标题与对应的下拉列表序号
    QMap<int, QString> m_map;

    // listView中 item的添加到时间线按钮
    // 整个 dock只有一个，而不是每个 item一个
    QPushButton *m_addToTimelineButton;

public slots:
    // listView右键菜单 actionRemove触发的槽函数
    void on_actionRemove_triggered();

private slots:
    // 响应搜索框 lineEdit内容变化的槽函数
    void on_lineEdit_textChanged(const QString& search);
//    void on_actionRemove_triggered();
    // listView右键菜单 actionRemoveAll触发的槽函数
    void on_actionRemoveAll_triggered();
    // listview右键菜单 actionPlay触发的槽函数
    void on_actionPlay_triggered();
    // listView右键菜单 actionProperties触发的槽函数
    // 【无内容】
    void on_actionProperties_triggered();
    // 响应下拉列表 comboBox项被激活的槽函数
    void on_comboBox_activated(const QString &arg1);
    // RecentDock可见（切换到 RecentDock）的槽函数
    void on_RecentDock_visibilityChanged(bool visible);

    // listView被激活的槽函数
    void onListviewActivated(const QModelIndex &index);
    // 按下 listView的槽函数
    void onListviewPressed(const QModelIndex &index);
    // 单击 listView的槽函数
    void onListviewClicked(const QModelIndex &index);
    // listView右键菜单的槽函数
    void onListviewCustomContextMenuRequested(const QPoint &pos);

    // 将当前文件添加到时间线上
    void addToTimeline();
};

#endif // RECENTDOCK_H
