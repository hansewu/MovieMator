#ifndef HTMLEFFECTDOCKWIDGET_H
#define HTMLEFFECTDOCKWIDGET_H

#include "basedockwidget.h"
#include "resourcedockgenerator_global.h"

class HtmlEffectDockWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    // 构造函数
    explicit HtmlEffectDockWidget(MainInterface *pMainInterface = nullptr, QWidget *pParent = nullptr);
    ~HtmlEffectDockWidget();

protected:
    UnsortMap<QString, BaseItemModel *> *createAllClassesItemModel();
    void addItemToTimeline(const QStandardItem *pItem);
    void preview(const QStandardItem *pItem);

private:
    MainInterface *m_pMainInterface;
};

#endif // HTMLEFFECTDOCKWIDGET_H
