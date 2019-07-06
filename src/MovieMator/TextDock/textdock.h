#ifndef TEXTDOCK_H
#define TEXTDOCK_H

#include "textdock_global.h"

#include <QDir>
#include <QLabel>
#include <QMimeData>
#include <QDockWidget>
#include <QSpacerItem>
#include <maininterface.h>
#include <QJsonObject>

#include "textdockinterface.h"
#include "textlistmodel.h"
#include "textlistview.h"

#include <QPushButton>

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
    // 使用图片 imageFile替换动画 effectFile中的图片,并根据类型设置sizeandposition滤镜参数
//    void resetImage(QString effectFile, QString imageFile);
    // 存放拖动动画的 MimeData
    void setMimeDataForDrag();
    // 根据动画和图片的下拉列表内容（选中即确定）创建动画文件
    void createEffectFile();

    // 创建图片列表 fileList，并保存文件夹名称 folderName
    // folderName传递给 appendListViewAndLabel()函数用来创建列表的显示名称
    void createImageFileList(QFileInfoList &fileList, QString folderName);
    // 根据 model创建显示的列表，并显示名称 itemName
    void appendListViewAndLabel(TextListModel *model, QString itemName);
    // 读取 json文件 filePath，主要是用来翻译文件夹名称的
    // jsonObj是给函数 getTranslationStr()使用的
    void readJsonFile(QString filePath, QJsonObject &jsonObj);
    // 根据字符串 srcStr从 translationInfo中寻获取对应的翻译内容
    QString getTranslationStr(QString srcStr, QJsonObject translationInfo);
    // 根据字符串 srcStr从 propertyInfo中寻获取对应的翻译内容
    QString getImageClassType(QString srcStr, QJsonObject propertyInfo);

private slots:
    // 响应 listView按下的槽函数
    void onListviewPressed(const QModelIndex&);
    // 响应 listView单击的槽函数
    void onListviewClicked(const QModelIndex&);
    // 响应 listView右键菜单的槽函数
    void onListviewCustomContextMenuRequested(const QPoint&);
    // 响应 listView右键菜单 actionAddToTimeline的槽函数
    void on_actionAddToTimeline_triggered();
//    // 响应 comboBox（动画列表）的选项被激活的槽函数
//    void on_comboBox_activated(int index);
//    // 响应 comboBox（动画列表）的选项发生变化的槽函数
//    void on_comboBox_currentIndexChanged(int index);
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

    // （带图片的）动画文件
    TextItemInfo *m_effectFile;
//    // 动画文件列表
//    QList<QString> *m_effectList;
    // 存放当前动画文件的 MimeData（供拖放到时间线使用）
    QMimeData *m_mimeData;

    // 图片列表的列表
    // 每个文件夹下的图片存放在一个列表里，这些列表也存放在一个列表里
    QList<TextListView *> *m_imageList;
    // 当前选中的 listView项
    QModelIndex m_currentIndex;
    // 当前被激活的图片文件列表
    TextListView *m_currentListView;

    // 动画文件名称翻译信息
    QJsonObject m_animationNameTranslateInfo;
    // 图片文件名称翻译信息
    QJsonObject m_imageClassNameTranslateInfo;
    // 图片文件名称翻译信息
    QJsonObject m_imageClassPropertyInfo;

    // 将当前动画添加到时间线的按钮
    // 不为 model的每个 item都添加一个，只使用一个
    AddButton *m_addButton;
};

#endif // TEXTDOCK_H
