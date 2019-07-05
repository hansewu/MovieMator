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

#include "videofilterlistview.h"
#include "videofilterlistviewmodel.h"

#include <QKeyEvent>
#include <QDrag>
#include <QApplication>
#include <QMimeData>
#include <qdebug.h>

VideoFilterListView::VideoFilterListView(QWidget *parent)
    : QListView(parent)
    , m_canStartDrag(false)
    , m_columnCount(1)
    , m_modelIndex(QModelIndex())
{
    setAcceptDrops(false);
    setMouseTracking(true);     // 可以跟踪鼠标（移动事件）
}

VideoFilterListView::~VideoFilterListView()
{

}

void VideoFilterListView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
    {
        /* make sure we ignore left/right keypresses here so it can bubble its way up to the top
         * level where it's handled as a global keyboard shortcut. There's seemingly no way to keep
         * QListView from using left/right for moving between cells, so this is a slight hack to
         * prevent that behavior. */

        event->ignore();
        return;
    }
    QListView::keyPressEvent(event);
}

void VideoFilterListView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton
            && indexAt(event->pos()).isValid())
    {
        m_dragStart = event->pos();
        m_canStartDrag = true;
    }
    else
        m_canStartDrag = false;

    QListView::mousePressEvent(event);
}

void VideoFilterListView::mouseMoveEvent(QMouseEvent* event)
{
    // 鼠标悬浮时触发的行为
    QModelIndex index = indexAt(event->pos());
    if(index.isValid())
    {
        // 防止在同一个 item里每次移动都发送信号
        if(m_modelIndex != index)
        {
            m_modelIndex = index;
            emit mouseMove();
        }
    }
    else
    {
        m_modelIndex = QModelIndex();
        emit mouseLeave();
    }
    // End

    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStart).manhattanLength() < QApplication::startDragDistance())
        return;
    if (!m_canStartDrag)
        return;

    QDrag drag(this);

    VideoFilterListVideoModel *viewModel = static_cast<VideoFilterListVideoModel *>(model());
    Q_ASSERT(viewModel);
    if(viewModel && (selectedIndexes().count() > 0))
    {
        int selectedRow = selectedIndexes().first().row();
        int dragFilterIndex = viewModel->fileAt(selectedRow)->filterIndex();

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("filter/filterindex", QString::number(dragFilterIndex).toUtf8());
        drag.setMimeData(mimeData);

        QImage thumbnail = viewModel->thumbnail(selectedIndexes().first().row());
        drag.setPixmap(QPixmap::fromImage(thumbnail).scaled(80, 45));

        drag.setHotSpot(QPoint(0, 0));
        drag.exec(Qt::MoveAction);
    }
}

// 鼠标离开时要清除 m_modelIndex
void VideoFilterListView::leaveEvent(QEvent *event)
{
    m_modelIndex = QModelIndex();
    emit mouseLeave();

    QListView::leaveEvent(event);
}

