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
#include "resourcebuttondockwidget.h"
#include "ui_resourcebuttondockwidget.h"
#include "../mainwindow.h"


ResourceButtonDockWidget::ResourceButtonDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ResourceButtonDockWidget)
{
    ui->setupUi(this);

    connect(ui->m_showPlaylistDock, SIGNAL(clicked()),this,SLOT(on_showPlaylistDock_clicked()));
    connect(ui->m_showRecentDock, SIGNAL(clicked()),this,SLOT(on_showRecentDock_clicked()));
    connect(ui->m_showFilterDock, SIGNAL(clicked()),this,SLOT(on_showFilterDock_clicked()));


    ui->m_showPlaylistDock->setLayoutDirection(Qt::LeftToRight);
    ui->m_showRecentDock->setLayoutDirection(Qt::LeftToRight);
    ui->m_showFilterDock->setLayoutDirection(Qt::LeftToRight);

    setStyleSheet(".QWidget{background-color: rgb(82,82,82);border-radius:5px}");

    ui->m_showRecentDock->setStyleSheet("background-color:rgb(51,51,51);color:rgb(225,225,225)");
    ui->m_showPlaylistDock->setStyleSheet("background-color:rgb(82,82,82);color:rgb(225,225,225)");
    ui->m_showFilterDock->setStyleSheet("background-color:rgb(82,82,82);color:rgb(225,225,225)");

    ui->m_showPlaylistDock->setIcon(QIcon(":/icons/light/32x32/show-playlist.png"));
    ui->m_showRecentDock->setIcon(QIcon(":/icons/light/32x32/show-recent-highlight.png"));
    ui->m_showFilterDock->setIcon(QIcon(":/icons/light/32x32/show-filters.png"));


    ui->m_showPlaylistDock->setIconSize(QSize(20,20));
    ui->m_showRecentDock->setIconSize(QSize(20,20));
    ui->m_showFilterDock->setIconSize(QSize(20,20));

    ui->m_showPlaylistDock->hide();
    ui->m_showFilterDock->hide();
 //   setBackgroundRole();

}

ResourceButtonDockWidget::~ResourceButtonDockWidget()
{
    delete ui;
}

void ResourceButtonDockWidget::on_showPlaylistDock_clicked()
{
    ui->m_showPlaylistDock->setStyleSheet("background-color:rgb(51,51,51);color:rgb(225,225,225)");
    ui->m_showRecentDock->setStyleSheet("background-color:rgb(82,82,82);color:rgb(225,225,225)");
    ui->m_showFilterDock->setStyleSheet("background-color:rgb(82,82,82);color:rgb(225,225,225)");

    ui->m_showPlaylistDock->setIcon(QIcon(":/icons/light/32x32/show-playlist-highlight.png"));
    ui->m_showRecentDock->setIcon(QIcon(":/icons/light/32x32/show-recent.png"));
    ui->m_showFilterDock->setIcon(QIcon(":/icons/light/32x32/show-filters.png"));

    MAIN.showPlaylistDock();

}

void ResourceButtonDockWidget::on_showRecentDock_clicked()
{
    ui->m_showRecentDock->setStyleSheet("background-color:rgb(51,51,51);color:rgb(225,225,225)");
    ui->m_showPlaylistDock->setStyleSheet("background-color:rgb(82,82,82);color:rgb(225,225,225)");
    ui->m_showFilterDock->setStyleSheet("background-color:rgb(82,82,82);color:rgb(225,225,225)");

    ui->m_showPlaylistDock->setIcon(QIcon(":/icons/light/32x32/show-playlist.png"));
    ui->m_showRecentDock->setIcon(QIcon(":/icons/light/32x32/show-recent-highlight.png"));
    ui->m_showFilterDock->setIcon(QIcon(":/icons/light/32x32/show-filters.png"));

    MAIN.showRecentDock();
}

void ResourceButtonDockWidget::on_showFilterDock_clicked()
{
    ui->m_showFilterDock->setStyleSheet("background-color:rgb(51,51,51);color:rgb(225,225,225)");
    ui->m_showPlaylistDock->setStyleSheet("background-color:rgb(82,82,82);color:rgb(225,225,225)");
    ui->m_showRecentDock->setStyleSheet("background-color:rgb(82,82,82);color:rgb(225,225,225)");

    ui->m_showPlaylistDock->setIcon(QIcon(":/icons/light/32x32/show-playlist.png"));
    ui->m_showRecentDock->setIcon(QIcon(":/icons/light/32x32/show-recent.png"));
    ui->m_showFilterDock->setIcon(QIcon(":/icons/light/32x32/show-filters-highlight.png"));

    MAIN.showFilterDock();
}
