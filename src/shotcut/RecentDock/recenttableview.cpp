#include "recenttableview.h"
#include <QKeyEvent>
#include <QDrag>
#include <QApplication>
#include <QSortFilterProxyModel>
#include "recenttablemodel.h"

RecentTableView::RecentTableView(QWidget *parent)
    : QTableView(parent)
    , m_canStartDrag(false)
{

}

void RecentTableView::keyPressEvent(QKeyEvent* event)
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

void RecentTableView::mousePressEvent(QMouseEvent* event)
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

void RecentTableView::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStart).manhattanLength() < QApplication::startDragDistance())
        return;
    if (!m_canStartDrag)
        return;

    QDrag drag(this);

    // tableView的 model现在是一个 QSortFilterProxyModel对象，
    // QSortFilterProxyModel对象的 sourceModel才是 RecentlistModel对象
    QSortFilterProxyModel *proxyModel = static_cast<QSortFilterProxyModel *>(model());
    RecentTableModel *recentlistModel = proxyModel ? static_cast<RecentTableModel *>(proxyModel->sourceModel()) : nullptr;
    if(recentlistModel)
    {
        // 在过滤的时候获得正确的 model id
        QModelIndex first = selectedIndexes().first();
        int idx = proxyModel->itemData(first.sibling(first.row(), 0))[Qt::ToolTipRole].toInt() - 1;
        QMimeData *mimeData = recentlistModel->mimeData(idx);
        QImage thumbnail = recentlistModel->thumbnail(idx);

        drag.setMimeData(mimeData);
        drag.setPixmap(QPixmap::fromImage(thumbnail));
        drag.setHotSpot(QPoint(0, 0));
        drag.exec(Qt::MoveAction);

//        QTableView::mouseMoveEvent(event);
    }
}

QModelIndexList RecentTableView::getSeleted()
{
    return selectionModel()->selectedRows();
}

