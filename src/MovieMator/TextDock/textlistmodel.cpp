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

#include "textlistmodel.h"

#include <settings.h>
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include <QMimeData>
#include <util.h>

TextListModel::TextListModel(MainInterface *main, QObject *parent) :
    QAbstractItemModel(parent),
    m_mainWindow(main)
{
    m_textList = new QList<TextItemInfo *>;

    QString templateDir = Util::resourcesPath() + "/template/";
    TranslationHelper::readJsonFile(templateDir + "textfile_name_translation_info.json", m_textFilesNameTranslateInfo);
}

TextListModel::~TextListModel()
{
    qDeleteAll(*m_textList);
    m_textList->clear();
    delete m_textList;
    m_textList = nullptr;
}

int TextListModel::rowCount(const QModelIndex&) const
{
    Q_ASSERT(m_textList);
    if(!m_textList)
    {
        return 0;
    }
    return m_textList->count();
}

int TextListModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant TextListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(m_textList);
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.row()>=0);
    Q_ASSERT(index.row()<m_textList->count());
    if(!m_textList || !index.isValid() || index.row()<0 || index.row()>=m_textList->count())
    {
        return QVariant();
    }

    TextItemInfo *fileHandle = m_textList->at(index.row());
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

            QString name = Util::baseName(fileHandle->textFilePath()).split(".")[0];
            QString result = TranslationHelper::getTranslationStr(name, m_textFilesNameTranslateInfo);
            return result;
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
            QImage thumb = QImage(fileHandle->thumbnailPath());
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

QMimeData *TextListModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(m_textList);
    Q_ASSERT(indexes.first().isValid());
    Q_ASSERT(indexes.first().row() >= 0);
    Q_ASSERT(indexes.first().row() < m_textList->count());
    if(!m_textList || !indexes.first().isValid() || (indexes.first().row()<0) || (indexes.first().row()>=m_textList->count()))
    {
        return nullptr;
    }
    TextItemInfo *fileHandle = m_textList->at(indexes.first().row());
    Q_ASSERT(fileHandle);
    Q_ASSERT(m_mainWindow);
    if(!fileHandle || !m_mainWindow)
    {
        return nullptr;
    }

    QMimeData *mimeData = new QMimeData;
    FILE_HANDLE file = m_mainWindow->openFile(fileHandle->textFilePath());

    mimeData->setData(m_mainWindow->getXMLMimeTypeForDragDrop(), m_mainWindow->getXmlForDragDrop(file).toUtf8());
    mimeData->setText(QString::number(m_mainWindow->getPlayTime(file)));

    m_mainWindow->destroyFileHandle(file);

    return mimeData;
}

QModelIndex TextListModel::index(int row, int column, const QModelIndex&) const
{
    return createIndex(row, column);
}

QModelIndex TextListModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

void TextListModel::append(TextItemInfo *fileHandle)
{
    Q_ASSERT(fileHandle);
    Q_ASSERT(m_textList);
    if(!fileHandle || !m_textList)
    {
        return;
    }
    int count = m_textList->count();
    beginInsertRows(QModelIndex(), count, count);
    m_textList->append(fileHandle);
    endInsertRows();
}

TextItemInfo *TextListModel::fileAt(int row) const
{
    Q_ASSERT(m_textList);
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < m_textList->count());
    if(!m_textList || row<0 || row>=m_textList->count())
    {
        return nullptr;
    }
    return m_textList->at(row);
}

QImage TextListModel::thumbnail(int row) const
{
    Q_ASSERT(m_textList);
    Q_ASSERT(row>=0);
    Q_ASSERT(row<m_textList->count());
    if(!m_textList || row<0 || row>=m_textList->count())
    {
        return QImage();
    }

    return QImage(m_textList->at(row)->thumbnailPath());
}
