#ifndef TEXTDOCKWIDGET_H
#define TEXTDOCKWIDGET_H

#include "basedockwidget.h"
#include "resourcedockgenerator_global.h"

class TextDockWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    // 构造函数
    explicit TextDockWidget(MainInterface *main = nullptr, QWidget *parent = nullptr);
    ~TextDockWidget();

protected:
    QMap<QString, QStandardItemModel *> *createAllClassesItemModel();
    bool hasClass();
    void addToTimeline(const QStandardItem *item);
    void preview(const QStandardItem *item);

private:
    MainInterface *m_mainInterface;
};

#endif // TEXTDOCKWIDGET_H
