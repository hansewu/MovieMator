#ifndef BASELISTVIEW_H
#define BASELISTVIEW_H

#include <qlistview.h>

class BaseListView : public QListView
{
    Q_OBJECT

public:
    BaseListView(QWidget *pParent = nullptr);
};

#endif // BASELISTVIEW_H
