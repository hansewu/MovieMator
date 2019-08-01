#ifndef BASEDOCKWIDGET_H
#define BASEDOCKWIDGET_H

#include <QDockWidget>

class QListView;
class BaseItemModel;
class QStandardItem;
class BaseListView;

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

    //用于项dock顶部工具栏添加控件
    virtual void setupOtherUi();

    //创建所有分类的数据
    virtual QMap<QString, BaseItemModel *> *createAllClassesItemModel() = 0;

    //是否有分类
    virtual bool hasClass() = 0;

    //添加资源到时间线
    virtual void addToTimeline(const QStandardItem *pItem) = 0;

    //预览预览资源
    virtual void preview(const QStandardItem *pItem) = 0;

protected slots:
    //点击item右上角的添加按钮
    virtual void addItemToTimeline(const QModelIndex &index);

    //单击item
    virtual void clickedItem(const QModelIndex &index);

    //显示右键菜单
    virtual void showMenu(const QModelIndex &index);

    //当切换分类时，跳转到对应分类的listview
    virtual void onClassComboBoxActivated(int nIndex);

private slots:
    //切换dock时，自适应调节dock的宽
    void onDockWidgetVisibilityChanged(bool bVisible);

private:
    void setupListView();
    void createAllClassesListView(QMap<QString, BaseItemModel *> *pAllClassesItemModel,
                                  bool bHasClass = true);
    BaseListView *createListView(BaseItemModel *pItemModel);
    void createClassesNameWidget(const QString &strClassName);

protected:
    Ui::BaseDockWidget *ui;

    //保存所有的分类listview，主要用于从combox切换分类时跳转到对应分类的listview，
    //key为分类名，value为对应的listview
    QMap<QString, BaseListView *> *m_pAllClassesListView;
};

#endif // BASEDOCKWIDGET_H
