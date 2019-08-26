#include "baselistview.h"
#include "uiuserdef.h"
#include "qdebug.h"

#include <QDrag>
#include <QMouseEvent>
#include <QApplication>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

BaseListView::BaseListView(QWidget *pParent) :
    QListView (pParent)
{
    //FIXME:mac上字体显示太小
    // Mac下字体显示太小，使用默认字体大小，Windows和 Mac下看上去效果都很好
//    setFont(QFont(font().family(), LISTVIEW_ITEM_FONTSIZE));//设置listview的字体大小，主要作用与item上的文字
    setViewMode(QListView::IconMode);//使用大尺寸的图标及名字显示item
    setGridSize(QSize(LISTVIEW_GRIDSIZE_WIDTH, LISTVIEW_GRIDSIZE_HEIGHT));//设置网格中每个item的宽高
    setFocusPolicy(Qt::NoFocus);//listview不接受焦点事件
    setIconSize(QSize(LISTVIEW_ITEMICONSIZE_WIDTH, LISTVIEW_ITEMICONSIZE_HEIGHT));//listview中item上的图标的大小
//    setSpacing(8);//设置了GridSize后，spacing将不会起作用
    setResizeMode(QListView::Adjust);//listview大小改变时动态调整item在listview中的布局
    setUniformItemSizes(true);//将lsitview中的item设置为统一大小
    setWordWrap(true);//listview中item中的文字过长时将会换行
    setTextElideMode(Qt::ElideRight);//listview中item中的文字过长时将会以“...”省略
    setDragDropMode(QAbstractItemView::DragOnly);//listview只支持拖拽，不支持放下
    setEditTriggers(QAbstractItemView::NoEditTriggers);//禁用编辑，即禁止双击时编辑item中的文字
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//关闭竖直方向滚动条
#ifdef Q_OS_MAC
    setStyleSheet(//设置listview选中、滑过以及背景颜色
            "QListView::item:selected{background-color:rgb(192,72,44); color:rgb(255,255,255);border-radius:4px;}"
            "QListView::item:hover{background-color:rgb(192,72,44); color:rgb(255,255,255);border-radius:4px;}"
            "QListView{background-color:transparent;color:rgb(214,214,214);}"
            "QToolTip{background:rgb(205, 205, 205);}");  // 单独设置 tooltip的样式
#else
    setStyleSheet(//设置listview选中、滑过以及背景颜色
            "QListView::item:selected{background-color:rgb(192,72,44); color:rgb(255,255,255);border-radius:4px;}"
            "QListView::item:hover{background-color:rgb(192,72,44); color:rgb(255,255,255);border-radius:4px;}"
            "QListView{background-color:transparent;color:rgb(214,214,214);}");
#endif
}

void BaseListView::mousePressEvent(QMouseEvent *pEvent)
{
    if ((pEvent->button() == Qt::LeftButton)
            && indexAt(pEvent->pos()).isValid())
    {
        m_dragStart     = pEvent->pos();
        m_bCanStartDrag = true;
    }
    else
    {
        m_bCanStartDrag = false;
    }

    QListView::mousePressEvent(pEvent);
}

void BaseListView::mouseMoveEvent(QMouseEvent *pEvent)
{
    if (!(pEvent->buttons() & Qt::LeftButton))
    {
        return;
    }
    if ((pEvent->pos() - m_dragStart).manhattanLength() < QApplication::startDragDistance())
    {
        return;
    }
    if (!m_bCanStartDrag)
    {
        return;
    }

    if(model())
    {
        QPixmap pixmap;
        QString strSuperClassName = model()->metaObject()->superClass()->className();
        if(strSuperClassName == "BaseItemModel")
        {
            QStandardItemModel *pItemModel
                    = qobject_cast<QStandardItemModel *>(model());
            if(pItemModel)
            {
                pixmap = pItemModel->itemFromIndex(selectedIndexes().first())->icon().pixmap(80, 45);
            }
        }
        else if(strSuperClassName == "QAbstractProxyModel")
        {
            QSortFilterProxyModel *pProxyModel
                    = qobject_cast<QSortFilterProxyModel *>(model());
            if(pProxyModel)
            {
                QModelIndex modelIndex
                        = pProxyModel->mapToSource(selectedIndexes().first());
                QStandardItemModel *pItemModel
                        = qobject_cast<QStandardItemModel *>(pProxyModel->sourceModel());
                if(pItemModel && modelIndex.isValid())
                {
                    pixmap = pItemModel->itemFromIndex(modelIndex)->icon().pixmap(80, 45);
                }
            }
        }

        QDrag drag(this);

        drag.setPixmap(pixmap);
        drag.setHotSpot(QPoint(0, 0));
        drag.setMimeData(model()->mimeData(selectedIndexes()));

        drag.exec(Qt::MoveAction);
    }
}
