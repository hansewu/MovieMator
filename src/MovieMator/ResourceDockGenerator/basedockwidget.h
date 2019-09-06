#ifndef BASEDOCKWIDGET_H
#define BASEDOCKWIDGET_H

#include <QDockWidget>
#include "unsortmap.h"

class QListView;
class BaseItemModel;
class QStandardItem;
class BaseListView;
class QHBoxLayout;

namespace Ui {
    class BaseDockWidget;
}

class BaseDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit BaseDockWidget(QWidget *pParent = nullptr);
    ~BaseDockWidget();

    //模板方法，构建UI的流程
    virtual void setupUi() final;

protected:
    //用于listview中的项目个数变化时，更新Dock的宽。此函数为重载基类QDockWidget的函数
    virtual void resizeEvent(QResizeEvent *pEvent);

    //用于向dock顶部工具栏添加控件
    virtual void setupTopBarUi();

    //创建所有分类的数据
    virtual UnsortMap<QString, BaseItemModel*> *createAllClassesItemModel() = 0;

    //添加资源到时间线
    virtual void addItemToTimeline(const QStandardItem *pItem) = 0;

    //预览预览资源
    virtual void preview(const QStandardItem *pItem) = 0;

    //显示右键菜单
    virtual void showMeun(const QStandardItem *pItem, const QPoint &position);

    virtual void clearSelectionOfNotCurrentListView(const QModelIndex &index);

protected slots:
    //左键单击item右上角的添加按钮
    virtual void onLeftClickedAddButtonInItem(const QModelIndex &index);

    //左键单击item
    virtual void onLeftClickedItem(const QModelIndex &index);

    //当切换分类时，跳转到对应分类的listview
    virtual void onClassComboBoxActivated(int nIndex);

    //右键点击item
    virtual void onRightClickedItem(const QModelIndex &index, const QPoint &position);

    // 双击 item
    virtual void onDoubleClickedItem(const QModelIndex &index);

private slots:
    //切换dock时，自适应调节dock的宽
    void onDockWidgetVisibilityChanged(bool bVisible);

private:
    //创建所有分类的UI
    void setupAllClassesUi(UnsortMap<QString, BaseItemModel *> *pAllClassesItemModel);

    //创建一个分类的listview
    BaseListView *createClassListView(BaseItemModel *pItemModel);

    //创建一个分类的标签
    QHBoxLayout *createClassLabel(const QString &strClassName);

protected:
    Ui::BaseDockWidget *ui;

    //保存所有的分类listview，主要用于从combox切换分类时跳转到对应分类的listview，
    //key为分类名，value为对应的listview
    QMap<QString, BaseListView *> *m_pAllClassesListView;
};

#endif // BASEDOCKWIDGET_H
