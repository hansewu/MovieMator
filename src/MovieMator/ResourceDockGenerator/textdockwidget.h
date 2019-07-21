#ifndef TEXTDOCKWIDGET_H
#define TEXTDOCKWIDGET_H

#include "basedockwidget.h"
#include "resourcedockgenerator_global.h"

class TextItemModel;

class TextDockWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    // 构造函数
    explicit TextDockWidget(MainInterface *pMainInterface = nullptr, QWidget *pParent = nullptr);
    ~TextDockWidget();

protected:
    QMap<QString, BaseItemModel *> *createAllClassesItemModel();
    bool hasClass();
    void addToTimeline(const QStandardItem *pItem);
    void preview(const QStandardItem *pItem);

private:
    MainInterface *m_pMainInterface;
};

#endif // TEXTDOCKWIDGET_H
