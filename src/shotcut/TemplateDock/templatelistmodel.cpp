#include "templatelistmodel.h"

//#include <maininterface.h>
#include <settings.h>
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include <QMimeData>
#include <util.h>

TemplateListModel::TemplateListModel(MainInterface *main, QObject *parent) :
    QAbstractItemModel(parent),
    m_mainWindow(main)
{
    m_templateList = new QList<FILE_HANDLE>;
}

TemplateListModel::~TemplateListModel()
{
    qDeleteAll(*m_templateList);
    m_templateList->clear();
    delete m_templateList;
    m_templateList = nullptr;
}

int TemplateListModel::rowCount(const QModelIndex&) const
{
    return m_templateList->count();
}

int TemplateListModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant TemplateListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.row() < m_templateList->count());

    FILE_HANDLE fileHandle = m_templateList->at(index.row());
    switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            QString result = Util::baseName(m_mainWindow->getFileName(fileHandle));
            return result;
        }
        case Qt::DecorationRole: {
            int width = THUMBNAIL_WIDTH;
            int height = THUMBNAIL_HEIGHT;
            QString setting = Settings.playlistThumbnails();
            QImage image;

            image = QImage(width, height, QImage::Format_ARGB32);

            QImage thumb = m_mainWindow->getThumbnail(fileHandle);
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
        default:
            break;
    }
    return QVariant();
}

QMimeData *TemplateListModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(indexes.first().row() < m_templateList->count());

    QMimeData *mimeData = new QMimeData;

    FILE_HANDLE fileHandle = m_templateList->at(indexes.first().row());

    mimeData->setData(m_mainWindow->getXMLMimeTypeForDragDrop(), m_mainWindow->getXmlForDragDrop(fileHandle).toUtf8());
//    mimeData->setText(m_mainWindow->getDuration(fileHandle));
    mimeData->setText(QString::number(m_mainWindow->getPlayTime(fileHandle)));

    return mimeData;
}

QModelIndex TemplateListModel::index(int row, int column, const QModelIndex&) const
{
    return createIndex(row, column);
}

QModelIndex TemplateListModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

void TemplateListModel::append(FILE_HANDLE fileHandle)
{
    int count = m_templateList->count();
    beginInsertRows(QModelIndex(), count, count);
    m_templateList->append(fileHandle);
    endInsertRows();
}

void TemplateListModel::clear()
{
    if (rowCount()) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        m_templateList->clear();
        endRemoveRows();
    }
}

FILE_HANDLE TemplateListModel::getTemplateFile(int row) const
{
    Q_ASSERT(row >= 0 && row < m_templateList->count());
    return m_templateList->at(row);
}

QImage TemplateListModel::thumbnail(int row) const
{
    return m_mainWindow->getThumbnail(m_templateList->at(row));
}
