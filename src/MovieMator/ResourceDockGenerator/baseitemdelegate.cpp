#include "baseitemdelegate.h"

#include <qpainter.h>
#include <qevent.h>
#include <qdebug.h>

BaseItemDelegate::BaseItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent) {
    qDebug()<<"sll-----BaseItemDelegate构造---start";
    qDebug()<<"sll-----BaseItemDelegate构造---end";
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

    qDebug()<<"sll-----editorEvent---start";
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

    qDebug()<<"sll-----editorEvent---end";

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize BaseItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const {
    Q_UNUSED(option);
    Q_UNUSED(index);
    qDebug()<<"sll-----sizeHint---start";
    qDebug()<<"sll-----sizeHint---end";

    return QSize(120, 80);
}
