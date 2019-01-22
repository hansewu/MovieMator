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
{
    setAcceptDrops(false);
}

EffectListView::~EffectListView()
{
    delete m_mimeData;
    m_mimeData = nullptr;
}

void EffectListView::setMimeData(QMimeData *mimeData, const QString &mimeType)
{
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
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStart).manhattanLength() < QApplication::startDragDistance())
        return;
    if (!m_canStartDrag)
        return;

    QDrag drag(this);

    EffectListModel *viewModel = static_cast<EffectListModel *>(model());
    if(viewModel && m_mimeData)
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setData(m_mimeType, m_mimeData->data(m_mimeType));
        mimeData->setText(m_mimeData->text());
        drag.setMimeData(mimeData);
        QImage thumbnail = viewModel->thumbnail(selectedIndexes().first().row());
        drag.setPixmap(QPixmap::fromImage(thumbnail).scaled(80, 45));

        drag.setHotSpot(QPoint(0, 0));
        drag.exec(Qt::MoveAction);

//        QListView::mouseMoveEvent(event);
    }
}

void EffectListView::focusInEvent(QFocusEvent *event)
{
    setCurrentIndex(m_currentIndex);

    QListView::focusInEvent(event);
}

void EffectListView::focusOutEvent(QFocusEvent *event)
{
    // 失去焦点时取消当前被选中状态
    m_currentIndex = currentIndex();
    QModelIndex index = model()->index(-1, 0);
    setCurrentIndex(index);

    QListView::focusOutEvent(event);
}
