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

#ifndef VIDEOFILTERLISTVIEWMODEL_H
#define VIDEOFILTERLISTVIEWMODEL_H

#include <QImage>
#include <QAbstractItemModel>
#include "maininterface.h"
#include <qscriptengine.h>

class FilterItemInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filterType READ filterType WRITE setFilterType)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString imageSourcePath READ imageSourcePath WRITE setImageSourcePath)
    Q_PROPERTY(bool visible READ visible WRITE setVisible)
    Q_PROPERTY(bool filterIndex READ filterIndex WRITE setFilterIndex)

public:

    explicit FilterItemInfo(QObject *parent = nullptr){Q_UNUSED(parent);}

    Q_INVOKABLE QString filterType() const { return m_filterType; }
    Q_INVOKABLE void setFilterType(const QString strType){m_filterType = strType;}
    Q_INVOKABLE QString name() const { return m_name; }
    Q_INVOKABLE void setName(const QString strName){m_name = strName;}
    Q_INVOKABLE QString imageSourcePath() const { return m_imageSourcePath; }
    Q_INVOKABLE void setImageSourcePath(const QString strImageSourcePath){m_imageSourcePath = strImageSourcePath;}
    Q_INVOKABLE bool visible() const { return m_bVisible; }
    Q_INVOKABLE void setVisible(const bool visible){m_bVisible = visible;}
    Q_INVOKABLE int filterIndex() const { return m_filterIndex; }
    Q_INVOKABLE void setFilterIndex(const int filterIndex){m_filterIndex = filterIndex;}

private:
    QString m_filterType;
    QString m_name;
    QString m_imageSourcePath;
    bool m_bVisible;
    int m_filterIndex;
};

class VideoFilterListVideoModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    // 缩略图宽度
    static const int THUMBNAIL_WIDTH = 85;      //80;      // 100; 96  95      15  110
    // 缩略图高度
    static const int THUMBNAIL_HEIGHT = 57;     //60;     // 70;  64  90      30  79

    // 构造函数
    explicit VideoFilterListVideoModel(MainInterface *main = nullptr, QObject *parent = nullptr);
    // 析构函数
    ~VideoFilterListVideoModel();

    // model的行数
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    // model的列数
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    // model的数据模型
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    // model的序号
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    // model的父项
    QModelIndex parent(const QModelIndex &child) const;

    // 给 model添加数据 fileHandle
    void append(FilterItemInfo *fileHandle);

    // row行的文件（第 row个数据的内容）
    FilterItemInfo *fileAt(int row) const;
    // row行的缩略图（第 row个数据的缩略图）
    QImage thumbnail(int row) const;

private:
    void readTranslatJsFile(QString jsFilePath);
    QString getQmlDirPath();
private:
    // model的 list
    QList<FilterItemInfo *> *m_effectList;
    // 主界面
    MainInterface *m_mainWindow;

    QScriptEngine m_jsEngine;
};

#endif // VIDEOFILTERLISTVIEWMODEL_H
