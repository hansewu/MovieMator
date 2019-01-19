#ifndef FILTERDOCK_H
#define FILTERDOCK_H
#include <QDockWidget>
#include <maininterface.h>
#include "filterdock_global.h"

namespace Ui {
    class filterdock;
}



class FILTERDOCKSHARED_EXPORT FilterDock: public QDockWidget
{
    Q_OBJECT

public:
    explicit FilterDock(MainInterface *main=0, QWidget *parent = 0);
    ~FilterDock();


private:
    Ui::filterdock *ui;
    MainInterface *m_mainWindow;
};

#endif // FILTERDOCK_H


//      依赖的外部响应函数

//      获取qml中的metadata数据
//      MetadataModel* getMetadataModel()

//      点击选择滤镜的事件响应
//      int onFilterSelected(QmlMetadata* meta)
