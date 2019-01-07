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

#include "recentdock.h"
#include "settings.h"
#include "ui_recentdock.h"
#include "util.h"

#include <QDir>
#include <Logger.h>
#include <QMenu>

#include <QScrollBar>
static const int MaxItems = 20;


RecentDock::RecentDock(MainInterface *main, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::RecentDock),
    m_mainWindow(main)
{
    LOG_DEBUG() << "begin";
    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());
    m_recent = Settings.recent();

    m_model = new RecentTableModel(m_mainWindow, this);

    ui->tableView->setDragDropMode(QAbstractItemView::DragOnly);
    ui->tableView->setDragEnabled(true);
    ui->tableView->setDropIndicatorShown(true);
    ui->tableView->setDragDropOverwriteMode(false);
    ui->tableView->setAcceptDrops(false);
    ui->tableView->setDefaultDropAction(Qt::MoveAction);
    ui->tableView->setStyleSheet("QHeaderView::section { background-color:rgb(82,82,82); color:rgb(241,241,241) };");//QTableView{selection-color: rgb(35,148,229);background-color:rgb(51,51,51);font-color: rgb(255,255,255);color:rgb(214,214,214)}");
    //ui->tableView->setStyleSheet("QTableView{selection-background-color:rgb(192,72,44); selection-color: rgb(255,255,255);background-color:rgb(51,51,51);color:rgb(214,214,214);}");
    ui->tableView->setStyleSheet("QTableView{selection-color: rgb(255,255,255);background-color:rgb(51,51,51);color:rgb(214,214,214);}");

    QString style1 = "QLineEdit {padding-right: 20px; background-image: url(:/icons/light/32x32/search-icon.png); background-repeat: norepeat; background-position: center; border: 1px; border-radius: 3px; background-color: rgb(100,100,100) ; }";
    QString style2= "QLineEdit {padding-right: 20px; border: 1px; border-radius: 3px; background-color: rgb(100,100,100) ; }";
    ui->lineEdit->setStyleSheets(style1, style2);

    foreach (QString s, m_recent) {
        // 工程文件不添加到历史记录里
        if(!s.endsWith(".mmp"))
        {
            FILE_HANDLE fileHandle = m_mainWindow->openFile(s);
            if(fileHandle)
                m_model->append(fileHandle);
        }
    }

    m_proxyModel.setSourceModel(m_model);
    m_proxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel.setFilterKeyColumn(2);

    ui->tableView->setModel(&m_proxyModel);
    ui->tableView->resizeColumnToContents(0);
    ui->tableView->setColumnWidth(2, 100);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setDefaultSectionSize(50);
    LOG_DEBUG() << "end";
}

RecentDock::~RecentDock()
{
    delete ui;
}

void RecentDock::add(const QString &s)
{
    if (s.startsWith(QDir::tempPath()))
        return;
    if (m_recent.contains(s))
        return;

    FILE_HANDLE fileHandle = m_mainWindow->openFile(s);
    if(fileHandle)
    {
        m_model->insert(fileHandle, 0);
        m_recent.prepend(s);
    }
    while (m_recent.count() > MaxItems)
        m_recent.removeLast();
    Settings.setRecent(m_recent);
}

void RecentDock::on_tableView_activated(const QModelIndex& i)
{
    //ui->tableView->setCurrentIndex(QModelIndex());
    FILE_HANDLE fileHandle = m_model->fileAt(i.row());
    m_mainWindow->playFile(fileHandle);
    //ui->tableView->selectRow(0);
}

QString RecentDock::remove(const QString &s)
{
    m_recent.removeOne(s);
    Settings.setRecent(m_recent);

    QString name = Util::baseName(s);

    for(int i=0; i<m_model->rowCount(); i++)
    {
        if(QString::compare(s, m_model->fileName(i))==0)
        {
            m_model->remove(i);
            break;
        }
    }

    return name;
}

void RecentDock::on_lineEdit_textChanged(const QString& search)
{
    m_proxyModel.setFilterFixedString(search);
}

void RecentDock::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid() && m_model->rowCount()) {
        QMenu menu(this);
        menu.addAction(ui->actionRemove);
        menu.addAction(ui->actionRemoveAll);
        menu.addAction(ui->actionPlay);

        menu.exec(mapToGlobal(pos));
    }
}

void RecentDock::on_actionRemove_triggered()
{
    QModelIndex index = ui->tableView->currentIndex();
    int idx = m_proxyModel.itemData(index.sibling(index.row(), 0))[Qt::ToolTipRole].toInt() -1;
    if(idx>=0 && m_model->rowCount()) {
        m_model->remove(idx);
        m_recent.removeAt(idx);
//        // 以文件名删除，m_recent的 id和 m_model的 id不一定对应
//        QString s = m_proxyModel.itemData(index.sibling(index.row(), 2))[Qt::ToolTipRole].toString();
//        m_recent.removeOne(s);
        Settings.setRecent(m_recent);
    }
}

void RecentDock::on_actionRemoveAll_triggered()
{
    m_model->clear();
    m_recent.clear();
    Settings.setRecent(m_recent);
}

void RecentDock::on_actionPlay_triggered()
{
    QModelIndex index = ui->tableView->currentIndex();
    on_tableView_activated(index);
}

void RecentDock::on_actionProperties_triggered()
{

}

QList<FILE_HANDLE> RecentDock::getSelected()
{
    QList<FILE_HANDLE> seleted;
    QModelIndexList seletedIndexes =  ui->tableView->getSeleted();
    foreach(QModelIndex index, seletedIndexes)
    {
        QModelIndex sourceIndex = m_proxyModel.mapToSource(index);
        FILE_HANDLE fileHandle = m_model->fileAt(sourceIndex.row());
        seleted.append(fileHandle);
    }
    return seleted;
}

static RecentDock *instance = 0;
//初始化模块
//参数，main 主程序接口对象
//返回界面对象
QDockWidget *RecentDock_initModule(MainInterface *main)
{
    if (instance == NULL)
        instance = new RecentDock(main);
    return instance;
}

//销毁模块
void RecentDock_destroyModule()
{

}

//获取选中的文件列表
QList<FILE_HANDLE> RecentDock_getSelectedFiles()
{
    return instance->getSelected();
}

//添加文件
void RecentDock_add(QString filePath)
{
    instance->add(filePath);
}

//删除文件
void RecentDock_remove(QString filePath)
{
    instance->remove(filePath);
}

