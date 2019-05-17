/*
 * Copyright (c) 2015 Meltytech, LLC
 * Author: Harald Hvaal <harald.hvaal@gmail.com>
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

#include "playlisttable.h"

#include <QKeyEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QDebug>
//#include "mltcontroller.h"
//#include "mainwindow.h"
//#include "../playlistdock.h"
#include "../models/playlistmodel.h"

PlaylistTable::PlaylistTable(QWidget *parent)
    : QTableView(parent)
    , m_canStartDrag(false)
{

}

void PlaylistTable::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
    {
        /* make sure we ignore left/right keypresses here so it can bubble its way up to the top
         * level where it's handled as a global keyboard shortcut. There's seemingly no way to keep
         * QTableView from using left/right for moving between cells, so this is a slight hack to
         * prevent that behavior. */
        event->ignore();
        return;
    }
    QTableView::keyPressEvent(event);
}

void PlaylistTable::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton
            && indexAt(event->pos()).isValid())
    {
        m_dragStart = event->pos();
        m_canStartDrag = true;
    }
    else
        m_canStartDrag = false;
    QTableView::mousePressEvent(event);
}

void PlaylistTable::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStart).manhattanLength() < QApplication::startDragDistance())
        return;
    if (!m_canStartDrag)
        return;

    QDrag drag(this);

    PlaylistModel *playlistModel = (PlaylistModel *)model();
    QMimeData *mimeData = playlistModel->mimeData(selectedIndexes());
    drag.setMimeData(mimeData);

    if (selectedIndexes().count() <= 0) {
        return;
    }
    QImage thumbnail = playlistModel->thumbnail(selectedIndexes().first().row());
    drag.setPixmap(QPixmap::fromImage(thumbnail));
    drag.setHotSpot(QPoint(0, 0));
    drag.exec(Qt::MoveAction);
    QTableView::mouseMoveEvent(event);
}


//void PlaylistTable::dragEnterEvent(QDragEnterEvent *event)
//{
//    event->ignore();
//}

//void PlaylistTable::dragMoveEvent(QDragMoveEvent *event)
//{
//    event->ignore();
//}
