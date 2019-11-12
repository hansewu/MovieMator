#ifndef AUDIODOCKWIDGET_H
#define AUDIODOCKWIDGET_H

#include "basedockwidget.h"
#include "resourcedockgenerator_global.h"

class AudioItemModel;

class AudioDockWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    // 构造函数
    explicit AudioDockWidget(MainInterface *pMainInterface = nullptr, QWidget *pParent = nullptr);
    ~AudioDockWidget();

protected:
    UnsortMap<QString, BaseItemModel *> *createAllClassesItemModel();
    void addItemToTimeline(const QStandardItem *pItem);
    void preview(const QStandardItem *pItem);

private:
    MainInterface *m_pMainInterface;
};


#endif // AUDIODOCKWIDGET_H
