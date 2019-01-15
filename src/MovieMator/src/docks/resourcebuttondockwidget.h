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
