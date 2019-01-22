#ifndef TEMPLATELISTVIEW_H
#define TEMPLATELISTVIEW_H

#include <QListView>

class TemplateListView : public QListView
{
public:
    TemplateListView(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent*);

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

#endif // TEMPLATELISTVIEW_H
