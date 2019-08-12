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
};

#endif // BASEITEMDELEGATE_H
