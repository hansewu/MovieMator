#include "templatelistview.h"

#include <QKeyEvent>
#include <QDrag>
#include <QApplication>

#include "templatelistmodel.h"

TemplateListView::TemplateListView(QWidget *parent)
    : QListView(parent)
    , m_canStartDrag(false)
{

}

void TemplateListView::keyPressEvent(QKeyEvent* event)
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

void TemplateListView::mousePressEvent(QMouseEvent* event)
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

void TemplateListView::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStart).manhattanLength() < QApplication::startDragDistance())
        return;
    if (!m_canStartDrag)
        return;

    QDrag drag(this);

    TemplateListModel *viewModel = static_cast<TemplateListModel *>(model());
    if(viewModel)
    {
        QModelIndex first = selectedIndexes().first();
        QMimeData *mimeData = viewModel->mimeData(selectedIndexes());
        QImage thumbnail = viewModel->thumbnail(first.row());

        drag.setMimeData(mimeData);
        drag.setPixmap(QPixmap::fromImage(thumbnail).scaled(80, 45));
        drag.setHotSpot(QPoint(0, 0));
        drag.exec(Qt::MoveAction);

//        QListView::mouseMoveEvent(event);
    }
}

void TemplateListView::focusInEvent(QFocusEvent *event)
{
    setCurrentIndex(m_currentIndex);

    QListView::focusInEvent(event);
}

void TemplateListView::focusOutEvent(QFocusEvent *event)
{
    // 失去焦点时取消当前被选中状态
    m_currentIndex = currentIndex();
    QModelIndex index = model()->index(-1, 0);
    setCurrentIndex(index);

    QListView::focusOutEvent(event);
}

