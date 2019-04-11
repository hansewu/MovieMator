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

#include "recentlistview.h"

#include <QKeyEvent>
#include <QDrag>
#include <QApplication>

#include "recentlistmodel.h"

#include <QSortFilterProxyModel>

RecentListView::RecentListView(QWidget *parent)
    : QListView(parent)
    , m_canStartDrag(false)
{

}

void RecentListView::keyPressEvent(QKeyEvent* event)
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

void RecentListView::mousePressEvent(QMouseEvent* event)
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

void RecentListView::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStart).manhattanLength() < QApplication::startDragDistance())
        return;
    if (!m_canStartDrag)
        return;

    QDrag drag(this);

    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(model());
    Q_ASSERT(proxyModel);
    RecentListModel *viewModel = proxyModel ? qobject_cast<RecentListModel*>(proxyModel->sourceModel()) : nullptr;
    Q_ASSERT(viewModel);
    if(viewModel)
    {
        int row = proxyModel->mapToSource(selectedIndexes().first()).row();
        QMimeData *mimeData = viewModel->mimeData(row);
        Q_ASSERT(mimeData);
        if(!mimeData)
        {
            return;
        }
        QImage thumbnail = viewModel->thumbnail(row);

        drag.setMimeData(mimeData);
        drag.setPixmap(QPixmap::fromImage(thumbnail).scaled(80, 45));
        drag.setHotSpot(QPoint(0, 0));
        drag.exec(Qt::MoveAction);

//        QListView::mouseMoveEvent(event);
    }
}

QModelIndexList RecentListView::getSelected()
{
    return selectionModel()->selectedRows();
}
