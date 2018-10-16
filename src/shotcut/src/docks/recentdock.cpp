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

#include "docks/recentdock.h"
#include "settings.h"
#include "ui_recentdock.h"
#include "util.h"
#include "../securitybookmark/transport_security_bookmark.h"

#include <QDir>
#include <Logger.h>
#include <QMenu>

#include <QScrollBar>
static const int MaxItems = 20;

RecentDock::RecentDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::RecentDock)
{
    LOG_DEBUG() << "begin";
    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());
    m_recent = Settings.recent();

    ui->listWidget->setDragEnabled(true);
    ui->listWidget->setDragDropMode(QAbstractItemView::DragOnly);


//    ui->listWidget->verticalScrollBar()->setStyleSheet("QScrollBar::vertical{background-color:#515151;width:10px}""QScrollBar::handle:vertical{background:#aaaaaa;border:0px;border-radius:5px;}""QScrollBar::add-page:vertical{background:grey;}""QScrollBar::sub-page:vertical{background:grey}");
    ui->listWidget->setStyleSheet("QListView {selection-color: rgb(255,255,255);background-color:rgb(51,51,51);color:rgb(214,214,214);}");
    QString style1 = "QLineEdit {padding-right: 20px; background-image: url(:/icons/light/32x32/search-icon.png); background-repeat: norepeat; background-position: center; border: 1px; border-radius: 3px; background-color: rgb(100,100,100) ; }";
    QString style2= "QLineEdit {padding-right: 20px; border: 1px; border-radius: 3px; background-color: rgb(100,100,100) ; }";

    ui->lineEdit->setStyleSheets(style1, style2);

    foreach (QString s, m_recent) {
        QStandardItem* item = new QStandardItem(Util::baseName(s));
        item->setToolTip(s);
        m_model.appendRow(item);
    }
    m_proxyModel.setSourceModel(&m_model);
    m_proxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->listWidget->setModel(&m_proxyModel);
    LOG_DEBUG() << "end";
}

RecentDock::~RecentDock()
{
    delete ui;
}

void RecentDock::add(const QString &s)
{
    if (s.startsWith(QDir::tempPath())) return;
    QString name = remove(s);
    QStandardItem* item = new QStandardItem(name);
    item->setToolTip(s);
    m_model.insertRow(0, item);

    //int ret = create_security_bookmark(s.toUtf8().constData());


    m_recent.prepend(s);
    while (m_recent.count() > MaxItems)
        m_recent.removeLast();
    Settings.setRecent(m_recent);
}

void RecentDock::on_listWidget_activated(const QModelIndex& i)
{
    ui->listWidget->setCurrentIndex(QModelIndex());
    emit itemActivated(m_proxyModel.itemData(i)[Qt::ToolTipRole].toString());
}

QString RecentDock::remove(const QString &s)
{
    m_recent.removeOne(s);
    Settings.setRecent(m_recent);

    QString name = Util::baseName(s);
    QList<QStandardItem*> items = m_model.findItems(name);
    if (items.count() > 0)
        m_model.removeRow(items.first()->row());
    return name;
}

void RecentDock::on_lineEdit_textChanged(const QString& search)
{
    m_proxyModel.setFilterFixedString(search);
}

void RecentDock::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->listWidget->currentIndex();
    if (index.isValid() && m_model.rowCount()) {
        QMenu menu(this);
        menu.addAction(ui->actionRemove);
        menu.addAction(ui->actionRemoveAll);
        menu.addAction(ui->actionPlay);

        menu.exec(mapToGlobal(pos));
    }

}

void RecentDock::on_actionRemove_triggered()
{
    QModelIndex index = ui->listWidget->currentIndex();
    if (index.isValid() && m_model.rowCount()) {

        m_model.removeRow(index.row());
        m_recent.removeAt(index.row());
        Settings.setRecent(m_recent);
    }


}

void RecentDock::on_actionRemoveAll_triggered()
{
    int count = m_model.rowCount();

    m_model.removeRows(0,count);
    m_recent.clear();
    Settings.setRecent(m_recent);


}

void RecentDock::on_actionPlay_triggered()
{

    QModelIndex index = ui->listWidget->currentIndex();
    on_listWidget_activated(index);

}

void RecentDock::on_actionProperties_triggered()
{


}
