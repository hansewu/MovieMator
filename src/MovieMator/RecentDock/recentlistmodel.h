/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: Dragon-S <15919917852@163.com>
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

#ifndef RECENTLISTMODEL_H
#define RECENTLISTMODEL_H

#include <QImage>
#include <QAbstractItemModel>
#include "maininterface.h"

class FileItemInfo : public QObject
{
    Q_OBJECT

public:

    explicit FileItemInfo(QObject *parent = nullptr){Q_UNUSED(parent);}

    QString filePath() const {return  m_filePath;}
    void setFilePath(const QString filePath) {m_filePath = filePath;}
    FILE_TYPE fileType() const {return  m_fileType;}
    void setFileType(const FILE_TYPE fileType) {m_fileType = fileType;}
    QImage fileThumbnail() {return  m_fileThumbnail;}
    void setFileThumbnail(const QImage thumbnail) {m_fileThumbnail = thumbnail;}

private:
    QString m_filePath;
    FILE_TYPE m_fileType;
    QImage m_fileThumbnail;
};

class RecentListModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    // 缩略图的宽
    static const int THUMBNAIL_WIDTH = 80;      // 100;
    // 缩略图的高
    static const int THUMBNAIL_HEIGHT = 60;     // 70;

    // 构造函数
    explicit RecentListModel(MainInterface *main=nullptr, QObject *parent=nullptr);
    // 析构函数
    ~RecentListModel();

    // model的行数
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    // model的列数
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    // model存放的数据
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
//    // model的 mimeData
//    QMimeData *mimeData(const QModelIndexList &indexes) const;
//    // 第 index个model的 mimeData
//    QMimeData *mimeData(const int index) const;
    // model的 index
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    // model的父项
    QModelIndex parent(const QModelIndex &child) const;

    // 向 model所在的 listView添加数据 fileHandle
    void append(FileItemInfo *fileItemInfo);
    // 在 row行向 model所在的 listView添加数据 fileHandle
    void insert(FileItemInfo *fileItemInfo, int row);
    // 移除 row行 model的数据
    void remove(int row);
    // 清空 model
    void clear();

    // 第 row行的数据
    FileItemInfo *fileAt(int row) const;

    QMimeData *getMimeData(const int index) const;

private:
    // 存放 model的列表
    QList<FileItemInfo *> *m_recentList;
    // 主界面
    MainInterface *m_mainWindow;
};

#endif // RECENTLISTMODEL_H
