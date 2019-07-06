/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
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

#ifndef TEXTLISTVIEW_H
#define TEXTLISTVIEW_H

#include <QListView>

class TextListView : public QListView
{
    Q_OBJECT
public:
    // 构造函数
    TextListView(QWidget *parent = nullptr);
    // 析构函数
    ~TextListView();
    // 给 m_mimeData设置数据 mimeData，mime类型为 mimeType
    // 这个是在拖放时使用的
    void setMimeData(QMimeData *mimeData, const QString &mimeType);

    // 按键事件
    void keyPressEvent(QKeyEvent*);

    // 设置 m_columnCount的值
    void setColumnCount(int columnCount) { m_columnCount = columnCount; }
    // 获取 m_columnCount的值
    int getColumnCount() const { return m_columnCount;}
    //  获取鼠标悬浮时的 index
    QModelIndex getModelIndex() const { return m_modelIndex; }

signals:
    void mouseMove();
    void mouseLeave();

protected:
    // 鼠标按下事件
    void mousePressEvent(QMouseEvent *);
    // 鼠标移动事件（拖放）
    void mouseMoveEvent(QMouseEvent *);

    void leaveEvent(QEvent *);
private:
    // 鼠标左键按下时的起点（拖动用）
    QPoint m_dragStart;
    // listView能否被拖动
    bool m_canStartDrag;

    // 保存动画文件的 mimeData，供拖放使用
    QMimeData *m_mimeData;
    // 保存动画文件 mimeData类型
    QString m_mimeType;
    // 保存 listView显示的时候每行的列数
    int m_columnCount;
    // 鼠标悬浮时的 modelIndex
    QModelIndex m_modelIndex;
};

#endif // TEXTLISTVIEW_H
