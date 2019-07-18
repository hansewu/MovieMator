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
    explicit BaseDockWidget(QWidget *parent = nullptr);
    ~BaseDockWidget();

    void addItemToDock(QString itemClass, QStandardItem *item);
    void setupUi();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void setupOtherUi();
    virtual QMap<QString, BaseItemModel *> *createAllClassesItemModel() = 0;
    virtual bool hasClass() = 0;
    virtual void addToTimeline(const QStandardItem *item) = 0;
    virtual void preview(const QStandardItem *item) = 0;

private slots:
    virtual void addItemToTimeline(const QModelIndex &index);
    virtual void clickedItem(const QModelIndex &index);

    void onClassComboBoxActivated(int index);

private:
    void setupListView();
    void createAllClassesListView(QMap<QString, BaseItemModel *> *allClassesItemModel,
                                  bool hasClass = true);
    BaseListView *createListView(BaseItemModel *itemModel);
    void createClassesNameWidget(const QString &className);

protected:
    Ui::BaseDockWidget *ui;

private:
    //保存所有的分类listview，主要用于从combox切换分类时跳转到对应分类的listview，
    //key为分类名，value为对应的listview
    QMap<QString, BaseListView *> *m_allClassesListView;
};

#endif // BASEDOCKWIDGET_H
