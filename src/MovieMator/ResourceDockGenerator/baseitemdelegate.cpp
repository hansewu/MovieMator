#include "baseitemdelegate.h"

#include <qpainter.h>
#include <qevent.h>
#include <qdebug.h>
#include <qpoint.h>
#include <qapplication.h>

BaseItemDelegate::BaseItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent) {
    m_canStartDrag = false;
    m_dragStart = QPoint(0, 0);
}

BaseItemDelegate::~BaseItemDelegate() {
}

void BaseItemDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const {
    QStyleOptionViewItem viewOption(option);

    QStyledItemDelegate::paint(painter, viewOption, index);

    if (option.state.testFlag(QStyle::State_MouseOver)) {//绘制鼠标停留在item上的添加按钮
        //绘制添加按钮
        QPixmap addBtnPixmap = QPixmap(":/icons/light/32x32/filter_add.png");
        QRect addBtnRect = QRect(viewOption.rect.left() + viewOption.rect.width() - 27,
                                 viewOption.rect.top(), 27, 26);
        painter->save();
        painter->drawPixmap(addBtnRect, addBtnPixmap);
        painter->restore();
    }
}

bool BaseItemDelegate::editorEvent(QEvent *event,
                                   QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) {
    Q_UNUSED(model);

    QRect decorationRect = QRect(option.rect.left() + option.rect.width() - 27,
                                 option.rect.top(), 27, 26);
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (event->type() == QEvent::MouseButtonPress &&
        decorationRect.contains(mouseEvent->pos())) {
        emit addItem(index);
        return true;
    } else if (event->type() == QEvent::MouseButtonPress &&
               !decorationRect.contains(mouseEvent->pos())) {
        emit selectItem(index);
    }

    if ((mouseEvent->buttons() & Qt::LeftButton)) {
        m_dragStart = mouseEvent->pos();
        m_canStartDrag = true;
    } else {
        m_canStartDrag = false;
    }

    if (event->type() == QEvent::MouseMove &&
            m_canStartDrag &&
            (mouseEvent->pos() - m_dragStart).manhattanLength() < QApplication::startDragDistance()) {
        qDebug()<<"sll------event->type() = "<<event->type();
        qDebug()<<"sll------drag";
        emit dragItem(index);
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize BaseItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const {
    Q_UNUSED(option);
    Q_UNUSED(index);

    return QSize(120, 80);
}
