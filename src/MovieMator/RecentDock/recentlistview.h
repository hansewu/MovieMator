#ifndef RECENTLISTVIEW_H
#define RECENTLISTVIEW_H

#include <QListView>

class RecentListView : public QListView
{
public:
    RecentListView(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent*);
    QModelIndexList getSelected();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);

private:
    QModelIndex m_currentIndex;
    QPoint m_dragStart;
    bool m_canStartDrag;
};

#endif // RECENTLISTVIEW_H
