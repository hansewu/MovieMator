#ifndef BASEDOCKWIDGET_H
#define BASEDOCKWIDGET_H

#include <QDockWidget>

class QListView;
class QStandardItemModel;
class QStandardItem;
class BaseListView;

//typedef QMap<QString, QStandardItemModel *> ItemModelMap;

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
    void setupListView();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual QMap<QString, QStandardItemModel *> *createAllClassesItemModel() = 0;
    virtual bool hasClass() = 0;
    virtual void addToTimeline(const QStandardItem *item) = 0;
    virtual void preview(const QStandardItem *item) = 0;

private slots:
    virtual void addItemToTimeline(const QModelIndex &index);
    virtual void clickedItem(const QModelIndex &index);

private:
    void createAllClassesListView(QMap<QString, QStandardItemModel *> *allClassesItemModel,
                                  bool hasClass = true);
    BaseListView *createListView(QStandardItemModel *itemModel);
    void createClassesNameWidget(const QString &className);
    void createClassCombox();

protected:
    Ui::BaseDockWidget *ui;

private:
    //保存所有的分类listview，主要用于切换分类时跳转到对应分类的listview，
    //key为分类名，value为对应的listview
    QMap<QString, BaseListView *> *m_allClassesListView;
};

#endif // BASEDOCKWIDGET_H
