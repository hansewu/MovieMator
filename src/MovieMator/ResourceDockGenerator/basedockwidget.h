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

    void addItemToDock(QString strItemClass, QStandardItem *pItem);
    void setupUi();

protected:
    virtual void resizeEvent(QResizeEvent *pEvent);
    virtual void setupOtherUi();
    virtual QMap<QString, BaseItemModel *> *createAllClassesItemModel() = 0;
    virtual bool hasClass() = 0;
    virtual void addToTimeline(const QStandardItem *pItem) = 0;
    virtual void preview(const QStandardItem *pItem) = 0;

private slots:
    virtual void addItemToTimeline(const QModelIndex &index);
    virtual void clickedItem(const QModelIndex &index);

    void onClassComboBoxActivated(int nIndex);

private:
    void setupListView();
    void createAllClassesListView(QMap<QString, BaseItemModel *> *pAllClassesItemModel,
                                  bool bHasClass = true);
    BaseListView *createListView(BaseItemModel *pItemModel);
    void createClassesNameWidget(const QString &strClassName);

protected:
    Ui::BaseDockWidget *ui;

private:
    //保存所有的分类listview，主要用于从combox切换分类时跳转到对应分类的listview，
    //key为分类名，value为对应的listview
    QMap<QString, BaseListView *> *m_pAllClassesListView;
};

#endif // BASEDOCKWIDGET_H
