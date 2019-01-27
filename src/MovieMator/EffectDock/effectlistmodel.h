/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: WanYuanCN <ebthon@hotmail.com>
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

#ifndef EFFECTLISTMODEL_H
#define EFFECTLISTMODEL_H

#include <QImage>
#include <QAbstractItemModel>
#include "maininterface.h"

class EffectListModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    static const int THUMBNAIL_WIDTH = 80;      // 100;
    static const int THUMBNAIL_HEIGHT = 60;     // 70;

    explicit EffectListModel(MainInterface *main=nullptr, QObject *parent=nullptr);
    ~EffectListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    void append(FILE_HANDLE fileHandle);

    FILE_HANDLE fileAt(int row) const;
    QImage thumbnail(int row) const;

private:
    QList<FILE_HANDLE> *m_effectList;
    MainInterface *m_mainWindow;
};

#endif // EFFECTLISTMODEL_H
