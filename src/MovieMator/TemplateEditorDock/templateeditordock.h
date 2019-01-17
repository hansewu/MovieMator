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

#include <QDir>
#include <QDockWidget>
#include <maininterface.h>
#include "templateeditordockinterface.h"

namespace Ui {
    class TemplateEditorDock;
}

class TemplateEditorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit TemplateEditorDock(MainInterface *main=nullptr, QWidget *parent=nullptr);
    ~TemplateEditorDock();

    void chooseFile();
    void remove();
    void replaceFile(FILE_HANDLE file);
    void setFileList(QList<FILE_HANDLE> list);

private:
    Ui::TemplateEditorDock *ui;
    MainInterface *m_mainWindow;

    QList<FILE_HANDLE> m_fileList;

};

#endif // TEMPLATEEDITORDOCK_H
