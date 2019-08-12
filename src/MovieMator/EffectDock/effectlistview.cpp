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

#include "effectlistview.h"
#include "effectlistmodel.h"

#include <QKeyEvent>
#include <QDrag>
#include <QApplication>
#include <QMimeData>

EffectListView::EffectListView(QWidget *parent)
    : QListView(parent)
    , m_canStartDrag(false)
    , m_mimeData(new QMimeData())
    , m_columnCount(1)
    , m_modelIndex(QModelIndex())
{
    setAcceptDrops(false);
    setMouseTracking(true);     // 可以跟踪鼠标（移动事件）
}

EffectListView::~EffectListView()
{
    delete m_mimeData;
    m_mimeData = nullptr;
}

void EffectListView::setMimeData(QMimeData *mimeData, const QString &mimeType)
{
    Q_ASSERT(mimeData);
    Q_ASSERT(m_mimeData);
    if(!mimeData || !m_mimeData)
    {
        return;
    }
    m_mimeData->setData(mimeType, mimeData->data(mimeType));
    m_mimeData->setText(mimeData->text());
    m_mimeType = mimeType;
}

void EffectListView::keyPressEvent(QKeyEvent* event)
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

void EffectListView::mousePressEvent(QMouseEvent* event)
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

void EffectListView::mouseMoveEvent(QMouseEvent* event)
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

    EffectListModel *viewModel = static_cast<EffectListModel *>(model());
    Q_ASSERT(viewModel);
    Q_ASSERT(m_mimeData);
    if(viewModel && m_mimeData && (selectedIndexes().count() > 0))
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setData(m_mimeType, m_mimeData->data(m_mimeType));
        mimeData->setText(m_mimeData->text());
        drag.setMimeData(mimeData);
        EffectListItemInfo *itemInfo = viewModel->fileAt(selectedIndexes().first().row());
        Q_ASSERT(itemInfo);
        QImage thumbnail = itemInfo->thumbnail();//QImage(itemInfo->effectImagePath());
//        QImage thumbnail = viewModel->thumbnail(selectedIndexes().first().row());
        drag.setPixmap(QPixmap::fromImage(thumbnail).scaled(80, 45));

        drag.setHotSpot(QPoint(0, 0));
        drag.exec(Qt::MoveAction);

//        QListView::mouseMoveEvent(event);
    }
}

// 鼠标离开时要清除 m_modelIndex
void EffectListView::leaveEvent(QEvent *event)
{
    m_modelIndex = QModelIndex();
    emit mouseLeave();

    QListView::leaveEvent(event);
}

