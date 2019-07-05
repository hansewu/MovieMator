#ifndef VIDEOFILTERDOCK_H
#define VIDEOFILTERDOCK_H

#include "videofilterdock_global.h"

#include <QDir>
#include <QLabel>
#include <QDockWidget>
#include <QSpacerItem>
#include <maininterface.h>

#include "videofilterdockinterface.h"
#include "videofilterlistviewmodel.h"
#include "videofilterlistview.h"

#include <QPushButton>

namespace Ui {
    class VideoFilterDock;
}

// 添加到时间线按钮，重写鼠标移入移出事件
class AddButton : public QPushButton
{
    Q_OBJECT
public:
    AddButton(QWidget *parent) : QPushButton(parent) { }
    ~AddButton(){}

signals:
    void mouseEnter();
    void mouseLeave();

protected:
    void enterEvent(QEvent *event)
    {
        emit mouseEnter();
        QPushButton::enterEvent(event);
    }
    void leaveEvent(QEvent *event)
    {
        emit mouseLeave();
        QPushButton::leaveEvent(event);
    }
};

class VideoFilterDock : public QDockWidget
{
    Q_OBJECT

public:
    // 构造函数
    explicit VideoFilterDock(MainInterface *main = nullptr, QWidget *parent = nullptr);
    // 析构函数
    ~VideoFilterDock();

    void updateVideoFilters(VideoFilter_Info *filterInfos, int nFilterCount);

protected:
    // 界面的尺寸变化事件
    void resizeEvent(QResizeEvent *event);

private:
    // 使用图片 imageFile替换动画 effectFile中的图片,并根据类型设置sizeandposition滤镜参数
    void resetImage(QString effectFile, QString imageFile);

    // 根据 model创建显示的列表，并显示名称 itemName
    void appendListViewAndLabel(VideoFilterListVideoModel *model, QString itemName);

    void addFilter(int filterIndex);
    void previewFilter(int filterIndex);

private slots:
    // 响应 listView按下的槽函数
    void onListviewPressed(const QModelIndex&);
    // 响应 listView单击的槽函数
    void onListviewClicked(const QModelIndex&);
    // 响应 listView右键菜单的槽函数
    void onListviewCustomContextMenuRequested(const QPoint&);
    // 响应 listView右键菜单 actionAddToTimeline的槽函数
    void on_actionAddToTimeline_triggered();
    // 响应 comboBox_2（图片列表）的选项激活的槽函数
    void on_comboBox_2_activated(int index);
    // 响应 EffectDock显示（切换到 EffectDock）的槽函数
    void on_EffectDock_visibilityChanged(bool visible);

    // 设置 m_addToTimelineButton的位置
    void positionAddButton();
    // 隐藏 m_addToTimelineButton
    void hideAddButton();
    // 添加到时间线
    void addToTimeline();
    // 设置 listView的选中 item
    // 主要是为了在鼠标进入按钮时不清除 item的背景
    // 鼠标进入按钮后会清除 item的悬浮效果
    void setSelection();
    // 重置 listView的选中 item
    // 鼠标离开按钮后重置 item的背景
    void resetSelection();

private:
    // 界面 ui
    Ui::VideoFilterDock *ui;
    // 主界面
    MainInterface *m_mainWindow;

    // 图片列表的列表
    // 每个文件夹下的图片存放在一个列表里，这些列表也存放在一个列表里
    QList<VideoFilterListView *> *m_imageList;
    // 当前选中的 listView项
    QModelIndex m_currentIndex;
    // 当前被激活的图片文件列表
    VideoFilterListView *m_currentListView;

    // 将当前动画添加到时间线的按钮
    // 不为 model的每个 item都添加一个，只使用一个
    AddButton *m_addButton;

    QMap<QString, VideoFilterListVideoModel *> m_filtersInfoMap;
};

#endif // VIDEOFILTERDOCK_H
