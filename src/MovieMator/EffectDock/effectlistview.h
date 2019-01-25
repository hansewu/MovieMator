#ifndef EFFECTLISTVIEW_H
#define EFFECTLISTVIEW_H

#include <QListView>

class EffectListView : public QListView
{
public:
    EffectListView(QWidget *parent = nullptr);
    ~EffectListView();
    void setMimeData(QMimeData *mimeData, const QString &mimeType);

    void keyPressEvent(QKeyEvent*);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    QPoint m_dragStart;
    bool m_canStartDrag;

    QMimeData *m_mimeData;
    QString m_mimeType;
};

#endif // EFFECTLISTVIEW_H
