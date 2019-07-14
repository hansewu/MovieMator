#ifndef BASELISTVIEW_H
#define BASELISTVIEW_H

#include <qlistview.h>

class BaseListView : public QListView
{
    Q_OBJECT

public:
    BaseListView(QWidget *parent = nullptr);
    ~BaseListView();

private:
};

#endif // BASELISTVIEW_H
