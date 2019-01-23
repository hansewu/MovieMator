/*
 * Copyright (c) 2012 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: WanYuanCN <ebthon@hotmail.com>
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
#include <QSortFilterProxyModel>
#include <maininterface.h>
#include "recentdockinterface.h"
#include "recentlistmodel.h"
#include "recentlistview.h"

#include <QMap>
#include <QLabel>
#include <QSpacerItem>

namespace Ui {
    class RecentDock;
}

class RecentDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit RecentDock(MainInterface *main = 0, QWidget *parent = 0);
    ~RecentDock();
    static const int num = 3;

public slots:
    void add(const QString&);
    QString remove(const QString& s);
    QList<FILE_HANDLE> getSelected();

private:
    void resizeEvent(QResizeEvent* event);

private:
    Ui::RecentDock *ui;
    QStringList m_recent;
    MainInterface *m_mainWindow;

    QModelIndex m_currentIndex;
    RecentListView *m_currentListView;

    QList<RecentListView*> *m_listviewList;
    QList<RecentListModel*> *m_modelList;
    QSortFilterProxyModel *m_proxyArray[num];
    QLabel *m_labelArray[num];
    QLabel *m_imageArray[num];
    bool m_flag[num] = {false};
    const QString m_itemNames[num] = {tr("Videos"), tr("Audios"), tr("Images")};

    QMap<int, QString> m_map;
    QSpacerItem *m_verticalSpacerItem;

private slots:
    void on_lineEdit_textChanged(const QString& search);
    void on_actionRemove_triggered();
    void on_actionRemoveAll_triggered();
    void on_actionPlay_triggered();
    void on_actionProperties_triggered();
    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_listView_activated(const QModelIndex &index);
    void on_listView_clicked(const QModelIndex &index);
    void on_listView_doubleClicked(const QModelIndex &index);
    void on_listView_customContextMenuRequested(const QPoint &pos);
};

#endif // RECENTDOCK_H
