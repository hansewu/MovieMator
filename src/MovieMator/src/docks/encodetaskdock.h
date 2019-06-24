/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
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
    explicit EncodeTaskDock(QWidget *parent = nullptr);
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
