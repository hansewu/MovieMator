#ifndef TEXTDOCK_H
#define TEXTDOCK_H

#include "textdock_global.h"

#include <QDir>
#include <QDockWidget>
#include <QPushButton>
#include <QJsonObject>
#include <maininterface.h>

#include "textdockinterface.h"
#include "textlistmodel.h"
#include "textlistview.h"

namespace Ui {
    class TextDock;
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

class TextDock : public QDockWidget
{
    Q_OBJECT

public:
    // 构造函数
    explicit TextDock(MainInterface *main=nullptr, QWidget *parent=nullptr);
    // 析构函数
    ~TextDock();

protected:
    // 界面的尺寸变化事件
    void resizeEvent(QResizeEvent *event);

private:
    // 获取当前的 FILE_HANDLE
    void genCurrentTextFile();
    // 创建文本模板 fileList，并保存文件夹名称 folderName
    // folderName传递给 appendListViewAndLabel()函数用来创建列表的显示名称
    void createTextFileList(QFileInfoList &fileList, QString folderName);
    // 根据 model创建显示的列表，并显示名称 itemName
    void appendListViewAndLabel(TextListModel *model, QString itemName);

private slots:
    // 响应 listView按下的槽函数
    void onListviewPressed(const QModelIndex&);
    // 响应 listView右键菜单的槽函数
    void onListviewCustomContextMenuRequested(const QPoint&);
    // 响应 listView右键菜单 actionAddToTimeline的槽函数
    void on_actionAddToTimeline_triggered();
    // 响应 comboBox_2（图片列表）的选项激活的槽函数
    void on_comboBox_2_activated(int index);
    // 响应 TextDock显示（切换到 TextDock）的槽函数
    void on_TextDock_visibilityChanged(bool visible);

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
    Ui::TextDock *ui;
    // 主界面
    MainInterface *m_mainWindow;
    // 每个文件夹下的文字模板存放在一个列表里，这些列表也存放在一个列表里
    QList<TextListView *> *m_textList;
    // 当前选中的 listView项
    QModelIndex m_currentIndex;
    // 当前被激活的图片文件列表
    TextListView *m_currentListView;
    // 当前的 FILE_HANDLE
    // 每次点击同一个文件时都不用重新生成
    FILE_HANDLE m_currentFile;

    // 文字模板文件夹（分类）名称翻译信息
    QJsonObject m_textClassNameTranslateInfo;

    // 将当前文字模板添加到时间线的按钮
    // 不为 model的每个 item都添加一个，只使用一个
    AddButton *m_addButton;
};

#endif // TEXTDOCK_H
