#ifndef RESOURCEBUTTONDOCKWIDGET_H
#define RESOURCEBUTTONDOCKWIDGET_H

#include <QDockWidget>


namespace Ui {
class ResourceButtonDockWidget;
}

class ResourceButtonDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ResourceButtonDockWidget(QWidget *parent = 0);
    ~ResourceButtonDockWidget();

private:
    Ui::ResourceButtonDockWidget *ui;

public slots:
    void on_showPlaylistDock_clicked();
    void on_showRecentDock_clicked();
    void on_showFilterDock_clicked();
};

#endif // RESOURCEBUTTONDOCKWIDGET_H
