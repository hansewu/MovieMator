#ifndef BASELISTVIEW_H
#define BASELISTVIEW_H

#include <qlistview.h>

class BaseListView : public QListView {
    Q_OBJECT

public:
    BaseListView(QWidget *parent = nullptr);
};

#endif // BASELISTVIEW_H
