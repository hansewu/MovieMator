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

#ifndef EFFECTDOCK_H
#define EFFECTDOCK_H

#include "effectdock_global.h"

#include <QDir>
#include <QDockWidget>
#include <QSpacerItem>
#include <maininterface.h>

#include "effectdockinterface.h"
#include "effectlistmodel.h"
#include "effectlistview.h"

namespace Ui {
    class EffectDock;
}

static const QString s_templateDir = "C:/Projects/MovieMator/template";

class EffectDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit EffectDock(MainInterface *main=nullptr, QWidget *parent=nullptr);
    ~EffectDock();

    FILE_HANDLE getEffectFile();
//    void setMimeDataForDrag() const;

private:
    // 样式化 listView
    void formatListView(QListView *listView);
    // 图片添加特效
    void replaceImage(QString effectFile, QString imageFile);
    void setEffectFile();

public slots:
    void on_listView_clicked(const QModelIndex&);
    void on_listView_doubleClicked(const QModelIndex&);
    void on_listView_customContextMenuRequested(const QPoint&);

private slots:
    void on_actionAddToTimeline_triggered();

private:
    Ui::EffectDock *ui;
    MainInterface *m_mainWindow;

    QDir m_dir;
    FILE_HANDLE m_effectFile;
//    QMimeData *m_mimeData;
};

#endif // EFFECTDOCK_H
