#ifndef ENCODETASKDOCK_H
#define ENCODETASKDOCK_H

#include <QDockWidget>

namespace Ui {
class EncodeTaskDock;
}

class EncodeTaskDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit EncodeTaskDock(QWidget *parent = 0);
    ~EncodeTaskDock();

private:
    Ui::EncodeTaskDock *ui;


private slots:
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void on_actionStopTask_triggered();
    void on_actionRemove_triggered();
    void on_stopButton_clicked();
    void on_closeButton_clicked();
};

#endif // ENCODETASKDOCK_H
