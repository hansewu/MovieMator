#ifndef BASELISTVIEW_H
#define BASELISTVIEW_H

#include <qlistview.h>

class BaseListView : public QListView
{
    Q_OBJECT

public:
    BaseListView(QWidget *pParent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *pEvent);

    void mouseMoveEvent(QMouseEvent *pEvent);

private:
    QPoint m_dragStart;

    bool   m_bCanStartDrag;
};

#endif // BASELISTVIEW_H
