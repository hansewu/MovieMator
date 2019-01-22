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

#ifndef TEMPLATEEDITORDOCK_H
#define TEMPLATEEDITORDOCK_H

#include "templateeditordock_global.h"
#include "templateeditordockinterface.h"

#include <QDir>
#include <QDockWidget>
#include <QQuickWidget>
#include <maininterface.h>

class QmlMetadata;

class TemplateEditorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit TemplateEditorDock(MainInterface *main=nullptr, QWidget *parent=nullptr);
    ~TemplateEditorDock();

    void setFileList(QList<FILE_HANDLE> fileList);

    Q_INVOKABLE int count() const;
    Q_INVOKABLE QString fileName(int index) const;
    Q_INVOKABLE QString filePath(int index) const;

    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void chooseFile(int index);
    Q_INVOKABLE void replaceFile(int flag, int index, QString url); // 从文件夹中拖放

signals:
    void refreshThumbnail(int index);
    void refreshView();

private:
    void resetQview();
    void replaceFile(int index, FILE_HANDLE destFile);

private:
    MainInterface *m_mainWindow;
    QQuickWidget m_qview;
    QList<FILE_HANDLE> *m_fileList;
};

#endif // TEMPLATEEDITORDOCK_H
