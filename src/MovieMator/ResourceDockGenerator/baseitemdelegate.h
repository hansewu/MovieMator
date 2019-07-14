#ifndef BASEITEMDELEGATE_H
#define BASEITEMDELEGATE_H

#include <qstyleditemdelegate.h>

class BaseItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit BaseItemDelegate(QObject *parent = nullptr);
    ~BaseItemDelegate();

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

protected:
    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);


signals:
    void addItem(const QModelIndex &index);
    void selectItem(const QModelIndex &index);
};

#endif // BASEITEMDELEGATE_H
