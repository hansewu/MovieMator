#include "baseitemdelegate.h"
#include "uiuserdef.h"

#include <qpainter.h>
#include <qevent.h>
#include <qdebug.h>

#include <QTimer>

BaseItemDelegate::BaseItemDelegate(QObject *pParent) :
    QStyledItemDelegate(pParent)
{
    qDebug()<<"sll-----BaseItemDelegate构造---start";

    m_bIsAddButton     = false;
    m_bIsDoubleClicked = false;

    m_pClickedTimer    = new QTimer(this);
    m_pClickedTimer->setSingleShot(true);
    connect(m_pClickedTimer, SIGNAL(timeout()), this, SLOT(singleClicked()));

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

    if (pEvent->type() == QEvent::MouseButtonRelease)
    {
        if(mouseEvent->button() == Qt::LeftButton)
        {
            if(m_bIsDoubleClicked)
            {   // 双击
                m_bIsDoubleClicked = false;
            }
            else
            {   // 单击
                if(index.isValid())
                {
                    m_selectedIndex = QPersistentModelIndex(index);
                }

                if (decorationRect.contains(mouseEvent->pos()))
                {
                    m_bIsAddButton = true;
                }
                else
                {
                    m_bIsAddButton = false;
                }

                // 鼠标释放 300ms之后没有触发双击事件，视为单击
                m_pClickedTimer->start(300);
            }
        }
        else if(mouseEvent->button() == Qt::RightButton)
        {   // 右击
            if (!decorationRect.contains(mouseEvent->pos()))
            {
                 emit selectItem(index);
            }

            //显示右键菜单
            emit rightClickItem(index, mouseEvent->pos());
        }
    }
    else if (pEvent->type() == QEvent::MouseButtonDblClick)
    {
        if(mouseEvent->button() == Qt::LeftButton)
        {   // 双击
            m_pClickedTimer->stop();
            m_bIsDoubleClicked = true;
            emit doubleClickItem(index);
            // 双击过后 400ms重置双击状态
            // 视频滤镜双击添加失败后弹出的窗口会阻止下一次的单击
            QTimer::singleShot(400, this, SLOT(resetDoubleClicked()));
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

void BaseItemDelegate::singleClicked()
{
    if(!m_selectedIndex.isValid())
    {
        return;
    }

    if(m_bIsAddButton)
    {
        emit addItem(m_selectedIndex);
    }
    else
    {
        emit selectItem(m_selectedIndex);
    }
}

void BaseItemDelegate::resetDoubleClicked()
{
    m_bIsDoubleClicked = false;
}
