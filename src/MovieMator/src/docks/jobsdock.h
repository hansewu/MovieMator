/*
 * Copyright (c) 2012-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JOBSDOCK_H
#define JOBSDOCK_H

#include <QDockWidget>

class AbstractJob;

namespace Ui {
    class JobsDock;
}

class JobsDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit JobsDock(QWidget *parent = nullptr);
    ~JobsDock();
    AbstractJob* currentJob() const;

    void highlightCurrentJobs(int index);

private:
    Ui::JobsDock *ui;

private slots:
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void on_actionStopJob_triggered();
    void on_actionViewLog_triggered();
    void on_pauseButton_toggled(bool checked);
    void on_actionRun_triggered();
    void onMenuButtonClicked();
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_actionRemove_triggered();
    void on_stopButton_clicked();
    void on_closeButton_clicked();
};

#endif // JOBSDOCK_H
