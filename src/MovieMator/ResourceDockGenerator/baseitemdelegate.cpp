#include "baseitemdelegate.h"
#include "uiuserdef.h"

#include <qpainter.h>
#include <qevent.h>
#include <qdebug.h>

BaseItemDelegate::BaseItemDelegate(QObject *pParent) :
    QStyledItemDelegate(pParent)
{
    qDebug()<<"sll-----BaseItemDelegate构造---start";
    qDebug()<<"sll-----BaseItemDelegate构造---end";
}

void BaseItemDelegate::paint(QPainter *pPainter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    QStyleOptionViewItem viewOption(option);

    QStyledItemDelegate::paint(pPainter, viewOption, index);

    //绘制鼠标停留在item上的添加按钮
    if (option.state.testFlag(QStyle::State_MouseOver))
    {
        QPixmap addBtnPixmap    = QPixmap(":/icons/light/32x32/filter_add.png");
        QRect addBtnRect        = QRect(viewOption.rect.left() + viewOption.rect.width() - LISTVIEW_ITEM_ADDBTNSIZE,
                                        viewOption.rect.top(),
                                        LISTVIEW_ITEM_ADDBTNSIZE,
                                        LISTVIEW_ITEM_ADDBTNSIZE);

        pPainter->save();
        pPainter->drawPixmap(addBtnRect, addBtnPixmap);
        pPainter->restore();
    }
}

bool BaseItemDelegate::editorEvent(QEvent *pEvent,
                                   QAbstractItemModel *pModel,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
    Q_UNUSED(pModel);

    qDebug()<<"sll-----editorEvent---start";

    QRect decorationRect    = QRect(option.rect.left() + option.rect.width() - LISTVIEW_ITEM_ADDBTNSIZE,
                                    option.rect.top(),
                                    LISTVIEW_ITEM_ADDBTNSIZE,
                                    LISTVIEW_ITEM_ADDBTNSIZE);
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(pEvent);

    if (pEvent->type() == QEvent::MouseButtonPress)
    {
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (decorationRect.contains(mouseEvent->pos()))
            {
                emit addItem(index);
                return true;
            }
            else
            {
                emit selectItem(index);
            }
        }
        else if (mouseEvent->button() == Qt::RightButton)
        {
            if (!decorationRect.contains(mouseEvent->pos()))
            {
                 emit selectItem(index);
            }

            //显示右键菜单
            emit rightClickItem(index, mouseEvent->pos());
        }
    }

    qDebug()<<"sll-----editorEvent---end";

    return QStyledItemDelegate::editorEvent(pEvent, pModel, option, index);
}

QSize BaseItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    qDebug()<<"sll-----sizeHint---start";

    QSize itemSize = QSize(LISTVIEW_GRIDSIZE_WIDTH - LISTVIEW_GRID_SPACING * 2,
                           LISTVIEW_GRIDSIZE_HEIGHT - LISTVIEW_GRID_SPACING * 2);

    qDebug()<<"sll-----sizeHint---end";

    return itemSize;
}
