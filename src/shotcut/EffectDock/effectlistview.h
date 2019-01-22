#ifndef EFFECTLISTVIEW_H
#define EFFECTLISTVIEW_H

#include <QListView>

#include "effectlistmodel.h"

class EffectListView : public QListView
{
public:
    EffectListView(QWidget *parent = nullptr);
//    ~EffectListView();
//    void setMimeData(QMimeData *mimeData);
//    void setThumbnail(QImage thumbnail);

    void keyPressEvent(QKeyEvent*);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
//    QModelIndex m_currentIndex;
    QPoint m_dragStart;
    bool m_canStartDrag;

//    QMimeData *m_mimeData;
//    QImage m_thumbnail;
};

#endif // EFFECTLISTVIEW_H
