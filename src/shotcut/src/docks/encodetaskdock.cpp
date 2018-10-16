#include "encodetaskdock.h"
#include "ui_encodetaskdock.h"
#include "encodetaskqueue.h"
#include <QMenu>
#include <QMessageBox>
#include "iratetransport.h"

EncodeTaskDock::EncodeTaskDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::EncodeTaskDock)
{
    ui->setupUi(this);
    ui->treeView->setModel(&ENCODETASKS);

    QHeaderView* header = ui->treeView->header();
    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
}

EncodeTaskDock::~EncodeTaskDock()
{
    ENCODETASKS.cleanup();
    delete ui;
}


void EncodeTaskDock::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;
    QMenu menu(this);
    AbstractTask* task = ENCODETASKS.taskFromIndex(index);
    if (task) {
        if (!task->stopped())
            menu.addAction(ui->actionStopTask);

        if (!task->ran() || task->stopped())
            menu.addAction(ui->actionRemove);
    }
    menu.exec(mapToGlobal(pos));
}


void EncodeTaskDock::on_actionStopTask_triggered()
{
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;
    AbstractTask* task = ENCODETASKS.taskFromIndex(index);
    if (task) task->stop();
}

void EncodeTaskDock::on_actionRemove_triggered()
{
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;
    ENCODETASKS.remove(index);
}

void EncodeTaskDock::on_closeButton_clicked()
{
        bool showDialog = false;
        if (ENCODETASKS.hasIncomplete())
            showDialog = true;

        if(showDialog){
            QMessageBox dialog(QMessageBox::Question,
                           tr("Tips"),
                           tr("The tasks will be stop, continue to close?"),
                           QMessageBox::No | QMessageBox::Yes,
                           this);
#if MOVIEMATOR_PRO
            dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));
#else
            dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif
            dialog.setDefaultButton(QMessageBox::Yes);
            dialog.setEscapeButton(QMessageBox::No);
            int result = dialog.exec();
            if (result == QMessageBox::Yes) {
                ENCODETASKS.stopTasks();
                this->hide();
            }

        }
        else
        {
            this->hide();
    #if SHARE_VERSION
    #else
    //        promotForRating();
    #endif
        }
}

void EncodeTaskDock::on_stopButton_clicked()
{
    on_actionStopTask_triggered();
}
