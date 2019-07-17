#include "baseitemdelegate.h"
#include "uiuserdef.h"

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
        QRect addBtnRect = QRect(viewOption.rect.left() + viewOption.rect.width() - LISTVIEW_ITEM_ADDBTNSIZE,
                                 viewOption.rect.top(), LISTVIEW_ITEM_ADDBTNSIZE, LISTVIEW_ITEM_ADDBTNSIZE);
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
    QRect decorationRect = QRect(option.rect.left() + option.rect.width() - LISTVIEW_ITEM_ADDBTNSIZE,
                                 option.rect.top(), LISTVIEW_ITEM_ADDBTNSIZE, LISTVIEW_ITEM_ADDBTNSIZE);
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (event->type() == QEvent::MouseButtonPress &&
        decorationRect.contains(mouseEvent->pos())) {//点击item右上角的添加按钮
        emit addItem(index);
        return true;
    } else if (event->type() == QEvent::MouseButtonPress &&
               !decorationRect.contains(mouseEvent->pos())) {//点击item右上角以外的其他位置
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

    QSize itemSize = QSize(LISTVIEW_GRIDSIZE_WIDTH - LISTVIEW_GRID_SPACING * 2,
                           LISTVIEW_GRIDSIZE_HEIGHT - LISTVIEW_GRID_SPACING * 2);

    return itemSize;
}
