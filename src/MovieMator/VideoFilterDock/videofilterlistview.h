#ifndef VIDEOFILTERLISTVIEW_H
#define VIDEOFILTERLISTVIEW_H

#include <qlistview.h>

class VideoFilterListView : public QListView
{
    Q_OBJECT
public:
    // 构造函数
    VideoFilterListView(QWidget *parent = nullptr);
    // 析构函数
    ~VideoFilterListView();

    // 按键事件
    void keyPressEvent(QKeyEvent*);

    // 设置 m_columnCount的值
    void setColumnCount(int columnCount) { m_columnCount = columnCount; }
    // 获取 m_columnCount的值
    int getColumnCount() const { return m_columnCount;}
    //  获取鼠标悬浮时的 index
    QModelIndex getModelIndex() const { return m_modelIndex; }

signals:
    void mouseMove();
    void mouseLeave();

protected:
    // 鼠标按下事件
    void mousePressEvent(QMouseEvent *);
    // 鼠标移动事件（拖放）
    void mouseMoveEvent(QMouseEvent *);

    void leaveEvent(QEvent *);
private:
    // 鼠标左键按下时的起点（拖动用）
    QPoint m_dragStart;
    // listView能否被拖动
    bool m_canStartDrag;

    // 保存 listView显示的时候每行的列数
    int m_columnCount;
    // 鼠标悬浮时的 modelIndex
    QModelIndex m_modelIndex;
};

#endif // VIDEOFILTERLISTVIEW_H
