/*
 * Copyright (c) 2012 Meltytech, LLC
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

#ifndef RECENTDOCK_H
#define RECENTDOCK_H

#include "recentdock_global.h"

#include <QDockWidget>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <maininterface.h>
#include "recentdockinterface.h"
#include "recenttablemodel.h"


namespace Ui {
    class RecentDock;
}

class RecentDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit RecentDock(MainInterface *main = 0, QWidget *parent = 0);
    ~RecentDock();

public slots:
    void add(const QString&);
    QString remove(const QString& s);

private:
    Ui::RecentDock *ui;
    QStringList m_recent;
    RecentTableModel *m_model;
    QSortFilterProxyModel m_proxyModel;
    MainInterface *m_mainWindow;

private slots:
    void on_tableView_activated(const QModelIndex& i);
    void on_tableView_customContextMenuRequested(const QPoint &pos);
    void on_lineEdit_textChanged(const QString& search);
    void on_actionRemove_triggered();
    void on_actionRemoveAll_triggered();
    void on_actionPlay_triggered();
    void on_actionProperties_triggered();
};

#endif // RECENTDOCK_H
