#ifndef RECENTTABLEVIEW_H
#define RECENTTABLEVIEW_H
#include <QTableView>
#include <QModelIndex>
class RecentTableView : public QTableView
{
public:
    RecentTableView(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent*);
    QModelIndexList getSeleted();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    QPoint m_dragStart;
    bool m_canStartDrag;
};

#endif // RECENTTABLEVIEW_H
