#include "effectlistview.h"

#include <QKeyEvent>
#include <QDrag>
#include <QApplication>
//#include <QMimeData>
#include <Logger.h>

EffectListView::EffectListView(QWidget *parent)
    : QListView(parent)
    , m_canStartDrag(false)
//    , m_mimeData(new QMimeData())
{
    setAcceptDrops(false);
}

//EffectListView::~EffectListView()
//{
//    delete m_mimeData;
//    m_mimeData = nullptr;
//}

//void EffectListView::setMimeData(QMimeData *mimeData)
//{
//    delete m_mimeData;
//    m_mimeData = mimeData;
//}

//void EffectListView::setThumbnail(QImage thumbnail)
//{
//    m_thumbnail = thumbnail;
//}

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
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStart).manhattanLength() < QApplication::startDragDistance())
        return;
    if (!m_canStartDrag)
        return;

    QDrag drag(this);

    EffectListModel *viewModel = static_cast<EffectListModel *>(model());
    if(viewModel/* && m_mimeData*/)
    {
        QModelIndex first = selectedIndexes().first();
        QMimeData *mimeData = viewModel->mimeData(selectedIndexes());
        drag.setMimeData(mimeData);
        QImage thumbnail = viewModel->thumbnail(first.row());
        drag.setPixmap(QPixmap::fromImage(thumbnail).scaled(80, 45));

//        drag.setMimeData(m_mimeData);
//        if(m_thumbnail.isNull())
//        {
//            m_thumbnail = viewModel->thumbnail(selectedIndexes().first().row());
//        }
//        drag.setPixmap(QPixmap::fromImage(m_thumbnail).scaled(80, 45));

        drag.setHotSpot(QPoint(0, 0));
        drag.exec(Qt::MoveAction);

//        QListView::mouseMoveEvent(event);
    }
}
