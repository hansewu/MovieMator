/*
 * Copyright (c) 2012-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
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

#include "playlistdock.h"
#include "ui_playlistdock.h"
//#include "dialogs/durationdialog.h"
//#include "mainwindow.h"
#include "settings.h"
#include "shotcut_mlt_properties.h"
#include <commands/playlistcommands.h>
#include <QMenu>
#include <Logger.h>
#include <QScrollBar>
#include "util.h"
#include <mltcontroller.h>

PlaylistDock::PlaylistDock(QWidget *parent) :
    QDockWidget(parent),

    ui(new Ui::PlaylistDock)
{
    LOG_DEBUG() << "begin";
    ui->setupUi(this);

    toggleViewAction()->setIcon(windowIcon());
    ui->tableView->setModel(&m_model);


    ui->tableView->setDragDropMode(QAbstractItemView::DragDrop);
    ui->tableView->setDropIndicatorShown(true);
    ui->tableView->setDragDropOverwriteMode(false);
    ui->tableView->setAcceptDrops(true);
    ui->tableView->setDefaultDropAction(Qt::MoveAction);
    ui->tableView->resizeColumnToContents(0);

    ui->tableView->setStyleSheet("QHeaderView::section { background-color:rgb(82,82,82); color:rgb(241,241,241) };");//QTableView{selection-color: rgb(35,148,229);background-color:rgb(51,51,51);font-color: rgb(255,255,255);color:rgb(214,214,214)}");

    ui->tableView->setStyleSheet("QTableView{selection-color: rgb(255,255,255);background-color:rgb(51,51,51);color:rgb(214,214,214);}");
    ui->tableView->setColumnWidth(2, 250);


    connect(ui->actionRemove, SIGNAL(triggered()), this, SLOT(on_removeButton_clicked()));
    connect(&m_model, SIGNAL(cleared()), this, SLOT(onPlaylistCleared()));
    connect(&m_model, SIGNAL(created()), this, SLOT(onPlaylistCreated()));
    connect(&m_model, SIGNAL(loaded()), this, SLOT(onPlaylistLoaded()));
    connect(&m_model, SIGNAL(modified()), this, SLOT(onPlaylistLoaded()));
    connect(&m_model, SIGNAL(dropped(const QMimeData*,int)), this, SLOT(onDropped(const QMimeData*,int)));
    connect(&m_model, SIGNAL(moveClip(int,int)), SLOT(onMoveClip(int,int)));

    //connect(ui->actionProperty, SIGNAL(triggered()), this->parentWidget(), SLOT(onPropertiesDockTriggered()));
    connect(ui->actionProperty, SIGNAL(triggered()), this, SLOT(on_actionProperty_triggered()));

    m_defaultRowHeight = ui->tableView->verticalHeader()->defaultSectionSize();
    QString thumbs = Settings.playlistThumbnails();
    if (thumbs == "wide") {
        ui->actionLeftAndRight->setChecked(true);
        on_actionLeftAndRight_triggered(true);
    }
    else if (thumbs == "tall") {
        ui->actionTopAndBottom->setChecked(true);
        on_actionTopAndBottom_triggered(true);
    }
    else if (thumbs == "small") {
        ui->actionInOnlySmall->setChecked(true);
        on_actionInOnlySmall_triggered(true);
    }
    else if (thumbs == "large") {
        ui->actionInOnlyLarge->setChecked(true);
        on_actionInOnlyLarge_triggered(true);
    }
    else {
        ui->actionThumbnailsHidden->setChecked(true);
        on_actionThumbnailsHidden_triggered(true);
    }
    LOG_DEBUG() << "end";
    qDebug() << "end";

    //隐藏按钮
    ui->addButton->hide();
    ui->removeButton->hide();
    ui->updateButton->hide();
    ui->menuButton->hide();

    ui->tableView->setColumnHidden(PlaylistModel::COLUMN_IN, true);
    ui->tableView->setColumnHidden(PlaylistModel::COLUMN_START, true);
    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(on_tableView_selectionChanged(QModelIndex,QModelIndex)));
}

PlaylistDock::~PlaylistDock()
{
    delete ui;
}

int PlaylistDock::position()
{
    int result = -1;
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid() && m_model.playlist()) {
        Mlt::ClipInfo* i = m_model.playlist()->clip_info(index.row());
        if (i) result = i->start;
        delete i;
    }
    return result;
}

void PlaylistDock::incrementIndex()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid())
        index = m_model.createIndex(0, 0);
    else
        index = m_model.incrementIndex(index);
    if (index.isValid())
        ui->tableView->setCurrentIndex(index);
}

void PlaylistDock::decrementIndex()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid())
        index = m_model.createIndex(0, 0);
    else
        index = m_model.decrementIndex(index);
    if (index.isValid())
        ui->tableView->setCurrentIndex(index);
}

void PlaylistDock::setIndex(int row)
{
    QModelIndex index = m_model.createIndex(row, 0);
    if (index.isValid())
        ui->tableView->setCurrentIndex(index);
}

void PlaylistDock::moveClipUp()
{
    int row = ui->tableView->currentIndex().row();
    if (row > 0)
//        MAIN.undoStack()->push(new Playlist::MoveCommand(m_model, row, row - 1));
        emit pushCommand(new Playlist::MoveCommand(m_model, row, row - 1));
}

void PlaylistDock::moveClipDown()
{
    int row = ui->tableView->currentIndex().row();
    if (row + 1 < m_model.rowCount())
//        MAIN.undoStack()->push(new Playlist::MoveCommand(m_model, row, row + 1));
        emit pushCommand(new Playlist::MoveCommand(m_model, row, row + 1));
}

void PlaylistDock::on_menuButton_clicked()
{
    QPoint pos = ui->menuButton->mapToParent(QPoint(0, 0));
    QMenu menu(this);
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid() && m_model.playlist()) {
        menu.addAction(ui->actionGoto);
        if (MLT.isClip())
            menu.addAction(ui->actionInsertCut);
        menu.addAction(ui->actionOpen);
        menu.addAction(ui->actionUpdate);
        menu.addAction(ui->actionRemove);
        menu.addSeparator();
    }
    menu.addAction(ui->actionRemoveAll);
    menu.addAction(ui->actionAddToTimeline);
    menu.addSeparator();
    QMenu* subMenu = menu.addMenu(tr("Thumbnails"));
    QActionGroup group(this);
    group.addAction(ui->actionThumbnailsHidden);
    group.addAction(ui->actionInOnlyLarge);
    group.addAction(ui->actionInOnlySmall);
    group.addAction(ui->actionLeftAndRight);
    group.addAction(ui->actionTopAndBottom);
    subMenu->addActions(group.actions());
    menu.exec(mapToGlobal(pos));
}

void PlaylistDock::on_actionInsertCut_triggered()
{
    if (MLT.isClip() || MLT.savedProducer()) {
        QMimeData mimeData;
        mimeData.setData(MLT.MltXMLMimeType(), MLT.XML(
            MLT.isClip()? 0 : MLT.savedProducer()).toUtf8());
        onDropped(&mimeData, ui->tableView->currentIndex().row());
    }
}

void PlaylistDock::on_actionAppendCut_triggered()
{
    if (MLT.producer() && MLT.producer()->is_valid()) {
        if (!MLT.isClip()) {
            emit showStatusMessage(tr("You cannot insert a playlist into a playlist!"));
        }
        else if (MLT.isSeekable())
        {
//            MAIN.undoStack()->push(new Playlist::AppendCommand(m_model, MLT.XML()));
            emit pushCommand(new Playlist::AppendCommand(m_model, MLT.XML()));
            MLT.producer()->set(kPlaylistIndexProperty, m_model.playlist()->count());
            setUpdateButtonEnabled(true);
        }
//        else
//        {
//            DurationDialog dialog(this);
//            dialog.setDuration(MLT.profile().fps() * 5);
//            if (dialog.exec() == QDialog::Accepted) {
//                MLT.producer()->set_in_and_out(0, dialog.duration() - 1);
//                if (MLT.producer()->get("mlt_service") && !strcmp(MLT.producer()->get("mlt_service"), "avformat"))
//                    MLT.producer()->set("mlt_service", "avformat-novalidate");
//                MAIN.undoStack()->push(new Playlist::AppendCommand(m_model, MLT.XML()));
//            }
//        }
    }
    else {
//        MAIN.openVideo();
        emit openVideo();
    }
}

void PlaylistDock::on_actionInsertBlank_triggered()
{
//    DurationDialog dialog(this);
//    dialog.setDuration(MLT.profile().fps() * 5);
//    if (dialog.exec() == QDialog::Accepted) {
//        QModelIndex index = ui->tableView->currentIndex();
//        if (index.isValid())
//            m_model.insertBlank(dialog.duration(), index.row());
//        else
//            m_model.appendBlank(dialog.duration());
//    }
}

void PlaylistDock::on_actionAppendBlank_triggered()
{
//    DurationDialog dialog(this);
//    dialog.setDuration(MLT.profile().fps() * 5);
//    if (dialog.exec() == QDialog::Accepted)
//        m_model.appendBlank(dialog.duration());
}

void PlaylistDock::on_actionUpdate_triggered()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid() || !m_model.playlist()) return;
    Mlt::ClipInfo* info = m_model.playlist()->clip_info(index.row());
    if (!info) return;
    if (MLT.producer()->type() != playlist_type) {
        if (MLT.isSeekable()) {
//            MAIN.undoStack()->push(new Playlist::UpdateCommand(m_model, MLT.XML(), index.row()));
            emit pushCommand(new Playlist::UpdateCommand(m_model, MLT.XML(), index.row()));
            MLT.producer()->set(kPlaylistIndexProperty, index.row() + 1);
            setUpdateButtonEnabled(true);
        }
//        else {
//            // change the duration
//            DurationDialog dialog(this);
//            dialog.setDuration(info->frame_count);
//            if (dialog.exec() == QDialog::Accepted) {
//                MLT.producer()->set_in_and_out(0, dialog.duration() - 1);
//                if (MLT.producer()->get("mlt_service") && !strcmp(MLT.producer()->get("mlt_service"), "avformat"))
//                    MLT.producer()->set("mlt_service", "avformat-novalidate");
//                MAIN.undoStack()->push(new Playlist::UpdateCommand(m_model, MLT.XML(), index.row()));
//            }
//        }
    }
    else {
        emit showStatusMessage(tr("You cannot insert a playlist into a playlist!"));
        setUpdateButtonEnabled(false);
    }
    delete info;
}

void PlaylistDock::on_removeButton_clicked()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid() || !m_model.playlist()) return;
//    MAIN.undoStack()->push(new Playlist::RemoveCommand(m_model, index.row()));
    emit pushCommand(new Playlist::RemoveCommand(m_model, index.row()));
    int count = m_model.playlist()->count();
    if (count == 0) return;
    int i = index.row() >= count? count-1 : index.row();
    QScopedPointer<Mlt::ClipInfo> info(m_model.playlist()->clip_info(i));
    if (info) {
        emit itemActivated(info->start);
        int j = MLT.producer()->get_int(kPlaylistIndexProperty);
        if (j > i + 1) {
           MLT.producer()->set(kPlaylistIndexProperty, j - 1);
        } else if (j == i + 1) {
            // Remove the playlist index property on the producer.
            MLT.producer()->set(kPlaylistIndexProperty, 0, 0);
            setUpdateButtonEnabled(false);
        }
    }
}

void PlaylistDock::setUpdateButtonEnabled(bool modified)
{
    //ui->updateButton->setEnabled(modified);
}

void PlaylistDock::on_actionOpen_triggered()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid() || !m_model.playlist()) return;
    Mlt::ClipInfo* i = m_model.playlist()->clip_info(index.row());
    if (i) {
        QString xml = MLT.XML(i->producer);
        Mlt::Producer* p = new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData());
        p->set_in_and_out(i->frame_in, i->frame_out);
        p->set(kPlaylistIndexProperty, index.row() + 1);
        emit clipOpened(p);
        delete i;
    }
}

void PlaylistDock::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid() && m_model.playlist()) {
        QMenu menu(this);
        menu.addAction(ui->actionGoto);
        if (MLT.isClip())
            menu.addAction(ui->actionInsertCut);
        menu.addAction(ui->actionOpen);
        menu.addAction(ui->actionAddToTimeline);





        QScopedPointer<Mlt::ClipInfo> info(m_model.playlist()->clip_info(index.row()));
        if (info && MLT.producer()->get_int(kPlaylistIndexProperty) == index.row() + 1) {
            if (MLT.producer()->get_in() != info->frame_in || MLT.producer()->get_out() != info->frame_out)
                menu.addAction(ui->actionUpdate);
        }

        menu.addAction(ui->actionRemove);
        menu.addAction(ui->actionRemoveAll);
        menu.addAction(ui->actionProperty);
        menu.exec(mapToGlobal(pos));
    }
}

void PlaylistDock::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (!m_model.playlist()) return;
    Mlt::ClipInfo* i = m_model.playlist()->clip_info(index.row());
    if (i) {
        if (qApp->keyboardModifiers() == Qt::ShiftModifier)
        {
            emit itemActivated(i->start);
        }
        else {
//            MAIN.setPauseAfterOpen(false);
            emit setPauseAfterOpen(false);
            QString xml = MLT.XML(i->producer);

            LOG_DEBUG()<<"on_tableView_doubleClicked is called";
            LOG_DEBUG()<<MLT.profile().description();
            LOG_DEBUG()<<xml;
            LOG_DEBUG()<<i->resource;


            Mlt::Producer* p = new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData());
            p->set_in_and_out(i->frame_in, i->frame_out);
            p->set(kPlaylistIndexProperty, index.row() + 1);
            emit clipOpened(p);
        }
        delete i;
    } else {
//        MAIN.openVideo();
        emit openVideo();
    }
}


void PlaylistDock::on_tableView_selectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (!m_model.playlist()) return;
    Mlt::ClipInfo* i = m_model.playlist()->clip_info(current.row());
    if (i) {
        if (qApp->keyboardModifiers() == Qt::ShiftModifier)
        {
            emit itemActivated(i->start);
        }
        else {
//            MAIN.setPauseAfterOpen(true);
            emit setPauseAfterOpen(true);
            QString xml = MLT.XML(i->producer);
            Mlt::Producer* p = new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData());
            p->set_in_and_out(i->frame_in, i->frame_out);
            p->set(kPlaylistIndexProperty, current.row() + 1);
            emit clipOpened(p);
        }
        delete i;
    }

}



void PlaylistDock::on_actionGoto_triggered()
{
    QModelIndex index = ui->tableView->currentIndex();
    on_tableView_doubleClicked(index);
    Mlt::ClipInfo* i = m_model.playlist()->clip_info(index.row());
    if (i) {
        emit itemActivated(i->start);
        delete i;
    }
}

void PlaylistDock::on_actionRemoveAll_triggered()
{
    //    MAIN.undoStack()->push(new Playlist::ClearCommand(m_model));
    emit pushCommand(new Playlist::ClearCommand(m_model));
}

void PlaylistDock::onPlaylistCreated()
{
    ui->removeButton->setEnabled(true);
    ui->updateButton->setEnabled(false);
    ui->menuButton->setEnabled(true);
    ui->stackedWidget->setCurrentIndex(1);
}

void PlaylistDock::onPlaylistLoaded()
{
    onPlaylistCreated();
//    ui->tableView->resizeColumnsToContents();
}

void PlaylistDock::onPlaylistCleared()
{
    ui->removeButton->setEnabled(false);
    ui->updateButton->setEnabled(false);
    ui->menuButton->setEnabled(false);
//    ui->stackedWidget->setCurrentIndex(0);
}

void PlaylistDock::onDropped(const QMimeData *data, int row)
{
    if (data && data->hasUrls()) {

        QStringList filelist;
        foreach (QUrl url, data->urls())
        {
            filelist.append(Util::removeFileScheme(url));
        }

//        MAIN.openFiles(filelist);
        emit openFiles(filelist);
    }
    else if (data && data->hasFormat(MLT.MltXMLMimeType())) {
        if (MLT.producer() && MLT.producer()->is_valid()) {
            if (MLT.producer()->type() == playlist_type) {
                emit showStatusMessage(tr("You cannot insert a playlist into a playlist!"));
            } else if (MLT.isSeekable()) {
                if (row == -1) {
//                    MAIN.undoStack()->push(new Playlist::AppendCommand(m_model, data->data(MLT.MltXMLMimeType())));
                    emit pushCommand(new Playlist::AppendCommand(m_model, data->data(MLT.MltXMLMimeType())));
                    MLT.producer()->set(kPlaylistIndexProperty, m_model.playlist()->count());
                } else {
//                    MAIN.undoStack()->push(new Playlist::InsertCommand(m_model, data->data(MLT.MltXMLMimeType()), row));
                    emit pushCommand(new Playlist::InsertCommand(m_model, data->data(MLT.MltXMLMimeType()), row));
                    MLT.producer()->set(kPlaylistIndexProperty, row + 1);
                }
                setUpdateButtonEnabled(true);
            } /*else {
                DurationDialog dialog(this);
                dialog.setDuration(MLT.profile().fps() * 5);
                if (dialog.exec() == QDialog::Accepted) {
                    MLT.producer()->set_in_and_out(0, dialog.duration() - 1);
                    if (MLT.producer()->get("mlt_service") && !strcmp(MLT.producer()->get("mlt_service"), "avformat"))
                        MLT.producer()->set("mlt_service", "avformat-novalidate");
                    if (row == -1)
                        MAIN.undoStack()->push(new Playlist::AppendCommand(m_model, MLT.XML()));
                    else
                        MAIN.undoStack()->push(new Playlist::InsertCommand(m_model, MLT.XML(), row));
                }
            }*/
        }
    }
}

void PlaylistDock::onMoveClip(int from, int to)
{
//    MAIN.undoStack()->push(new Playlist::MoveCommand(m_model, from, to));
    emit pushCommand(new Playlist::MoveCommand(m_model, from, to));
}

void PlaylistDock::onPlayerDragStarted()
{
    if (isVisible())
        ui->stackedWidget->setCurrentIndex(1);
}

void PlaylistDock::on_addButton_clicked()
{
    on_actionAppendCut_triggered();
}

void PlaylistDock::on_actionThumbnailsHidden_triggered(bool checked)
{
    if (checked) {
        Settings.setPlaylistThumbnails("hidden");
        ui->tableView->setColumnHidden(PlaylistModel::COLUMN_THUMBNAIL, true);
        ui->tableView->verticalHeader()->setDefaultSectionSize(m_defaultRowHeight);
    }
}

void PlaylistDock::on_actionLeftAndRight_triggered(bool checked)
{
    if (checked) {
        bool refreshThumbs = Settings.playlistThumbnails() != "tall";
        Settings.setPlaylistThumbnails("wide");
        if (refreshThumbs) m_model.refreshThumbnails();
        ui->tableView->setColumnHidden(PlaylistModel::COLUMN_THUMBNAIL, false);
        ui->tableView->verticalHeader()->setDefaultSectionSize(PlaylistModel::THUMBNAIL_HEIGHT);
        ui->tableView->resizeColumnToContents(PlaylistModel::COLUMN_THUMBNAIL);
    }
}

void PlaylistDock::on_actionTopAndBottom_triggered(bool checked)
{
    if (checked) {
        bool refreshThumbs = Settings.playlistThumbnails() != "wide";
        Settings.setPlaylistThumbnails("tall");
        if (refreshThumbs) m_model.refreshThumbnails();
        ui->tableView->setColumnHidden(PlaylistModel::COLUMN_THUMBNAIL, false);
        ui->tableView->verticalHeader()->setDefaultSectionSize(PlaylistModel::THUMBNAIL_HEIGHT * 2);
        ui->tableView->resizeColumnToContents(PlaylistModel::COLUMN_THUMBNAIL);
    }
}

void PlaylistDock::on_actionInOnlySmall_triggered(bool checked)
{
    if (checked) {
        bool refreshThumbs = Settings.playlistThumbnails() == "hidden";
        Settings.setPlaylistThumbnails("small");
        if (refreshThumbs) m_model.refreshThumbnails();
        ui->tableView->setColumnHidden(PlaylistModel::COLUMN_THUMBNAIL, false);
        ui->tableView->verticalHeader()->setDefaultSectionSize(PlaylistModel::THUMBNAIL_HEIGHT);
        ui->tableView->resizeColumnToContents(PlaylistModel::COLUMN_THUMBNAIL);
    }
}

void PlaylistDock::on_actionInOnlyLarge_triggered(bool checked)
{
    if (checked) {
        bool refreshThumbs = Settings.playlistThumbnails() == "hidden";
        Settings.setPlaylistThumbnails("large");
        if (refreshThumbs) m_model.refreshThumbnails();
        ui->tableView->setColumnHidden(PlaylistModel::COLUMN_THUMBNAIL, false);
        ui->tableView->verticalHeader()->setDefaultSectionSize(PlaylistModel::THUMBNAIL_HEIGHT * 2);
        ui->tableView->resizeColumnToContents(PlaylistModel::COLUMN_THUMBNAIL);
    }
}

void PlaylistDock::on_actionAddToTimeline_triggered()
{
    emit addAllTimeline(m_model.playlist());
}

void PlaylistDock::on_updateButton_clicked()
{
    int index = MLT.producer()->get_int(kPlaylistIndexProperty);
    if (index > 0 && index <= m_model.playlist()->count()) {
//        MAIN.undoStack()->push(new Playlist::UpdateCommand(m_model, MLT.XML(), index - 1));
        emit pushCommand(new Playlist::UpdateCommand(m_model, MLT.XML(), index - 1));
        setUpdateButtonEnabled(false);
    }
}

void PlaylistDock::on_actionProperty_triggered()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid() || !m_model.playlist()) return;
    Mlt::ClipInfo* i = m_model.playlist()->clip_info(index.row());
    if (i) {
        QString xml = MLT.XML(i->producer);
        Mlt::Producer* p = new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData());
        p->set_in_and_out(i->frame_in, i->frame_out);
        p->set(kPlaylistIndexProperty, index.row() + 1);
        emit clipOpened(p);
        delete i;

//        MAIN.loadProducerWidget(p);
        emit loadProducerWidget(p);
//        MAIN.onPropertiesDockTriggered();
        emit propertiesDockTriggered();
    }
}
