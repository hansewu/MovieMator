#ifndef BASEITEMDELEGATE_H
#define BASEITEMDELEGATE_H

#include <qstyleditemdelegate.h>

class BaseItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit BaseItemDelegate(QObject *pParent = nullptr);

    // painting
    void paint(QPainter *pPainter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

protected:
    bool editorEvent(QEvent *pEvent,
                     QAbstractItemModel *pModel,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);


signals:
    void addItem(const QModelIndex &index);
    void selectItem(const QModelIndex &index);
    void rightClickItem(const QModelIndex &index, const QPoint &position);
    // 双击信号
    void doubleClickItem(const QModelIndex &index);

private slots:
    // 单击槽函数，根据鼠标位置发送添加/选中信号
    void singleClicked();
    // 重置 m_bIsDoubleClicked为 false
    void resetDoubleClicked();

private:
    bool                  m_bIsAddButton;       // 是否添加按钮
    bool                  m_bIsDoubleClicked;   // 是否双击
    QTimer                *m_pClickedTimer;     // 定时器，判断是单击/双击
    QPersistentModelIndex m_selectedIndex;      // 选中的 index，用于单击事件
};

#endif // BASEITEMDELEGATE_H
