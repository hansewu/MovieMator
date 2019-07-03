/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: Dragon-S <15919917852@163.com>
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

#ifndef RECENTLISTVIEW_H
#define RECENTLISTVIEW_H

#include <QListView>

class RecentListView : public QListView
{
public:
    // 构造函数
    RecentListView(QWidget *parent = nullptr);
    // 键盘按下事件
    void keyPressEvent(QKeyEvent*);
    // 获取被选中的多个 model
    QModelIndexList getSelected();
    // 获取 m_columnCount的值
    int getColumnCount() const { return m_columnCount; }
    // 设置 m_columnCount的值
    void setColumnCount(int columnCount) { m_columnCount = columnCount; }

protected:
    // 鼠标按下事件
    void mousePressEvent(QMouseEvent *);
    // 鼠标移动事件（拖动）
    void mouseMoveEvent(QMouseEvent *);

private:
    // 鼠标左键按下时的起点（拖动用）
    QPoint m_dragStart;
    // listView能否被拖动
    bool m_canStartDrag;
    // 保存 listView显示时的每行列数
    int m_columnCount;
};

#endif // RECENTLISTVIEW_H
