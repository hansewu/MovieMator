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

#include "templateeditordock.h"
#include "settings.h"
#include "ui_templateeditordock.h"
#include "util.h"

#include <Logger.h>


TemplateEditorDock::TemplateEditorDock(MainInterface *main, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TemplateEditorDock),
    m_mainWindow(main)
{
    LOG_DEBUG() << "begin";
    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());

}

TemplateEditorDock::~TemplateEditorDock()
{

}

void TemplateEditorDock::chooseFile()
{

}

void TemplateEditorDock::remove()
{

}

void TemplateEditorDock::replaceFile(FILE_HANDLE file){

}

void TemplateEditorDock::setFileList(QList<FILE_HANDLE> list)
{
    for(int i=0; i<list.count(); i++)
    {
        m_fileList.append(list.at(i));
    }
}

static TemplateEditorDock *instance = nullptr;
//初始化模块
//参数，main 主程序接口对象
//返回界面对象
QDockWidget *TemplateEditorDock_initModule(MainInterface *main)
{
    if (instance == nullptr)
        instance = new TemplateEditorDock(main);
    return instance;
}
// 销毁模块
void TemplateEditorDock_destroyModule()
{

}
// 初始化界面
void TemplateEditorDock_initTemplateEditor(QList<FILE_HANDLE> fileList)
{
    instance->setFileList(fileList);
}

