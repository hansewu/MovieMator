/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: WanYuanCN <ebthon@hotmail.com>
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

#include "videofilterlistviewmodel.h"

#include <settings.h>
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include <util.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qdir.h>
#include <qstandardpaths.h>

VideoFilterListVideoModel::VideoFilterListVideoModel(MainInterface *main, QObject *parent) :
    QAbstractItemModel(parent),
    m_mainWindow(main)
{
    m_effectList = new QList<FilterItemInfo *>;
    QString jsFilePath = getQmlDirPath() + "/views/filter/translateTool.js";
    readTranslatJsFile(jsFilePath);
}

VideoFilterListVideoModel::~VideoFilterListVideoModel()
{
    qDeleteAll(*m_effectList);
    m_effectList->clear();
    delete m_effectList;
    m_effectList = nullptr;
}

int VideoFilterListVideoModel::rowCount(const QModelIndex&) const
{
    Q_ASSERT(m_effectList);
    if(!m_effectList)
    {
        return 0;
    }
    return m_effectList->count();
}

int VideoFilterListVideoModel::columnCount(const QModelIndex&) const
{
    return 1;
}

void VideoFilterListVideoModel::readTranslatJsFile(QString jsFilePath) {
    QString result = "";
    QFile scriptFile(jsFilePath);
    if (!scriptFile.open(QIODevice::ReadOnly)) {
        result.clear();
        qDebug()<<"sll-------------js file open error!!!";
        return;
    }

    QTextStream out(&scriptFile);
    out.setCodec("UTF-8");
    QString contents = out.readAll();
    scriptFile.close();

    m_jsEngine.evaluate(contents);
}

QString VideoFilterListVideoModel::getQmlDirPath() {
    QDir dir(qApp->applicationDirPath());

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
        dir.cdUp();
    #elif defined(Q_OS_MAC)
        dir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
    #endif

    dir.cd("share");
    dir.cd("moviemator");
    dir.cd("qml");
    return dir.absolutePath();
}

QVariant VideoFilterListVideoModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(m_effectList);
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.row()>=0);
    Q_ASSERT(index.row()<m_effectList->count());
    if(!m_effectList || !index.isValid() || index.row()<0 || index.row()>=m_effectList->count())
    {
        return QVariant();
    }

    FilterItemInfo *fileHandle = m_effectList->at(index.row());
    Q_ASSERT(fileHandle);
    if(!fileHandle)
    {
        return QVariant();
    }
    switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            Q_ASSERT(m_mainWindow);
            if(!m_mainWindow)
            {
                return QString();
            }
            QString result = Util::baseName(fileHandle->name());
            if (Settings.language() == "zh_CN") {
                QScriptValue transEn2ChFunc = m_jsEngine.globalObject().property("transEn2Ch");
                QScriptValueList args;
                args << QScriptValue(result);
                result = transEn2ChFunc.call(QScriptValue(), args).toString();
            } else {
                QScriptValue transEn2ShortFunc = m_jsEngine.globalObject().property("transEn2Short");
                QScriptValueList args;
                args << QScriptValue(result);
                result = transEn2ShortFunc.call(QScriptValue(), args).toString();
            }
            return result.split(".")[0];
        }
        case Qt::DecorationRole: {
            int width = THUMBNAIL_WIDTH;
            int height = THUMBNAIL_HEIGHT;
            QString setting = Settings.playlistThumbnails();
            QImage image;

            image = QImage(width, height, QImage::Format_ARGB32);
            Q_ASSERT(m_mainWindow);
            if(!m_mainWindow)
            {
                return image;
            }
            QImage thumb = QImage(fileHandle->imageSourcePath());//m_mainWindow->getThumbnail(fileHandle);
            if (!thumb.isNull()) {
                QPainter painter(&image);
                image.fill(QApplication::palette().base().color().rgb());

                // draw the in thumbnail
                QRect rect = thumb.rect();
                if (setting != "large") {
                    rect.setWidth(width);
                    rect.setHeight(height);
                }
                painter.drawImage(rect, thumb);

                painter.end();
            }
            else {
                image.fill(QApplication::palette().base().color().rgb());
            }
            return image;
        }
        case Qt::TextAlignmentRole: {   // 设置标题文本对齐方式
            return Qt::AlignLeft;
        }
        default:
            break;
    }
    return QVariant();
}


QModelIndex VideoFilterListVideoModel::index(int row, int column, const QModelIndex&) const
{
    return createIndex(row, column);
}

QModelIndex VideoFilterListVideoModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

void VideoFilterListVideoModel::append(FilterItemInfo *fileHandle)
{
    Q_ASSERT(fileHandle);
    Q_ASSERT(m_effectList);
    if(!fileHandle || !m_effectList)
    {
        return;
    }
    int count = m_effectList->count();
    beginInsertRows(QModelIndex(), count, count);
    m_effectList->append(fileHandle);
    endInsertRows();
}

FilterItemInfo *VideoFilterListVideoModel::fileAt(int row) const
{
    Q_ASSERT(m_effectList);
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < m_effectList->count());
    if(!m_effectList || row<0 || row>=m_effectList->count())
    {
        return nullptr;
    }
    return m_effectList->at(row);
}

QImage VideoFilterListVideoModel::thumbnail(int row) const
{
    Q_ASSERT(m_effectList);
    Q_ASSERT(row>=0);
    Q_ASSERT(row<m_effectList->count());
    Q_ASSERT(m_mainWindow);
    if(!m_effectList || row<0 || row>=m_effectList->count() || !m_mainWindow)
    {
        return QImage();
    }
    FilterItemInfo *filterItemInfo = m_effectList->at(row);
    QImage thumb = QImage(filterItemInfo->imageSourcePath());
    return  thumb;
//    return m_mainWindow->getThumbnail(m_effectList->at(row));
}
