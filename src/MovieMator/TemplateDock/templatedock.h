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

#ifndef TEMPLATEDOCK_H
#define TEMPLATEDOCK_H

#include "templatedock_global.h"

#include <QDir>
#include <QDockWidget>
#include <maininterface.h>

#include "templatedockinterface.h"
#include "templatelistmodel.h"
#include "templatelistview.h"

namespace Ui {
    class TemplateDock;
}

class TemplateDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit TemplateDock(MainInterface *main=nullptr, QWidget *parent=nullptr);
    ~TemplateDock();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    // 根据文件信息创建listView
    void createFileListView(QFileInfoList &fileList, QObject *parent=nullptr);

private:
    Ui::TemplateDock *ui;
    MainInterface *m_mainWindow;

    QModelIndex m_currentIndex;
    TemplateListModel *m_model;
    TemplateListView *m_currentListView;
    QList<TemplateListView*> *m_listviewList;

private slots:
    void on_listView_customContextMenuRequested(const QPoint &pos);
    void on_listView_clicked(const QModelIndex &index);
    void on_listView_doubleClicked(const QModelIndex &index);
    void on_actionAddToTimeline_triggered();
    void on_comboBox_currentIndexChanged(int index);
};

#endif // TEMPLATEDOCK_H