/*
 * Copyright (c) 2013-2016 Meltytech, LLC
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

#include "attachedfiltersmodel.h"
#include "mltcontroller.h"
#include "mainwindow.h"
#include "controllers/filtercontroller.h"
#include "qmlmetadata.h"
#include "shotcut_mlt_properties.h"
#include "util.h"
#include "commands/timelinecommands.h"
#include <QTimer>
#include <Logger.h>
#include <QUndoCommand>
#include <QtWidgets>

static bool sortIsLess (const QmlMetadata* lhs, const QmlMetadata* rhs) {
    // Sort order is: GPU, Video, Audio
    // If there is no metadata, assume the filter is video.
    if (!lhs && !rhs) {
        return false;
    } else if (!lhs) {
        if (rhs->needsGPU()) {
            return true;
        }
    } else if (!rhs) {
        if (lhs->isAudio()) {
            return true;
        }
    } else if (rhs->needsGPU() && !lhs->needsGPU()) {
        return true;
    } else if (!rhs->isAudio() && lhs->isAudio()) {
        return true;
    }
    return false;
}

AttachedFiltersModel::AttachedFiltersModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_dropRow(-1)
    , m_filter(VideoFilter)
{
}

bool AttachedFiltersModel::isReady()
{
    return m_producer;
}

Mlt::Filter* AttachedFiltersModel::getFilter(int row) const
{
    Q_ASSERT(m_producer);

    Mlt::Filter* result = nullptr;
    if (m_producer && m_producer->is_valid() && row < m_mltIndexMap.count() && row >= 0) {
        result = m_producer->filter(m_mltIndexMap[row]);
    }
    return result;
}

QmlMetadata* AttachedFiltersModel::getMetadata(int row) const
{
    if (row < m_metaList.count() && row >= 0) {
        return m_metaList[row];
    }
    return nullptr;
}

void AttachedFiltersModel::setProducer(Mlt::Producer* producer)
{
    Q_ASSERT(producer);

    if (!producer) return;

    // 当前时间线轨道上的 clip跟切换前的是同一个
    Mlt::Producer *pSelectedProducer = MAIN.timelineDock()->model()->selectedProducer();
    if(pSelectedProducer && (pSelectedProducer->get_producer() == producer->get_producer()))
    {
        return;
    }

//    Q_ASSERT(!producer || !m_producer || (producer->get_parent() != m_producer->get_parent()));
//    if (!producer || !m_producer || (producer->get_parent() != m_producer->get_parent())) {
        reset(producer);
//    }
}

QString AttachedFiltersModel::producerTitle() const
{
//    Q_ASSERT(m_producer);
//    Q_ASSERT(m_producer->is_valid());

    if (m_producer && m_producer->is_valid()) {
        if (m_producer->get(kShotcutTransitionProperty))
            return tr("Transition");
        if (m_producer->get(kTrackNameProperty))
            return tr("for Track: %1").arg(QString::fromUtf8(m_producer->get(kTrackNameProperty)));
        if (m_producer->get("moviemator:imageName"))
            return QString(m_producer->get("moviemator:imageName"));
        if (tractor_type == m_producer->type())
            return tr("for Timeline");
        if (m_producer->get(kShotcutCaptionProperty))
            return QString::fromUtf8(m_producer->get(kShotcutCaptionProperty));
        if (m_producer->get("resource"))
            return Util::baseName(QString::fromUtf8(m_producer->get("resource")));
    }
    return QString();
}

bool AttachedFiltersModel::isProducerSelected() const
{
//    Q_ASSERT(m_producer);
    if(!m_producer) return false;

    return !m_producer.isNull();
}

int AttachedFiltersModel::rowCount(const QModelIndex &) const
{
    if (m_producer && m_producer->is_valid())
        return m_metaList.count();
    else
        return 0;
}

Qt::ItemFlags AttachedFiltersModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractListModel::flags(index) | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled;
    else
        return QAbstractListModel::flags(index) | Qt::ItemIsDropEnabled;
}

QVariant AttachedFiltersModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(m_producer);
 //   Q_ASSERT(!m_producer || !m_producer->is_valid()
   //          || index.row() >= m_producer->filter_count());

    if ( !m_producer || !m_producer->is_valid()
        || index.row() >= m_producer->filter_count())
        return QVariant();
    switch (role ) {
    case Qt::DisplayRole: {
            QVariant result;

            const QmlMetadata* meta = m_metaList[index.row()];

            Q_ASSERT(meta);

            if (meta) {
                result = meta->name();
            } else {
                // Fallback is raw mlt_service name
                Mlt::Filter* filter = getFilter(index.row());
                Q_ASSERT(filter);
                if (filter && filter->is_valid() && filter->get("mlt_service")) {
                    result = QString::fromUtf8(filter->get("mlt_service"));
                }
                delete filter;
            }
            return result;
        }
    case Qt::CheckStateRole: {
            Mlt::Filter* filter = getFilter(index.row());
            Q_ASSERT(filter);
            QVariant result = Qt::Unchecked;
            if (filter && filter->is_valid() && !filter->get_int("disable"))
                result = Qt::Checked;
            delete filter;
            return result;
        }
//        break;
    case TypeDisplayRole: {
            QVariant result;
            const QmlMetadata* meta = m_metaList[index.row()];
            Q_ASSERT(meta);
            if (meta && meta->isAudio()) {
                result = tr("Audio");
            } else if (meta && meta->needsGPU()) {
                result = tr("GPU");
            } else {
                result = tr("Video");
            }
            return result;
        }
//        break;
    case ThumbnailRole: {
            QVariant result;
            const QmlMetadata* meta = m_metaList[index.row()];
            Q_ASSERT(meta);
            if (meta) {
                result = meta->thumbnail();
            }
            return result;
        }
//        break;
    default:
        break;
    }
    return QVariant();
}

bool AttachedFiltersModel::setData(const QModelIndex& index, const QVariant& , int role)
{
    if (role == Qt::CheckStateRole) {
        Mlt::Filter* filter = getFilter(index.row());
        Q_ASSERT(filter);
        if (filter && filter->is_valid()) {
            filter->set("disable", !filter->get_int("disable"));
            emit changed();
            emit dataChanged(createIndex(index.row(), 0), createIndex(index.row(), 0));
        }
        delete filter;
        return true;
    }
    return false;
}

QHash<int, QByteArray> AttachedFiltersModel::roleNames() const {
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[Qt::CheckStateRole] = "checkState";
    roles[TypeDisplayRole] = "typeDisplay";
    roles[ThumbnailRole] = "thumbnail";
    return roles;
}

Qt::DropActions AttachedFiltersModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool AttachedFiltersModel::insertRows(int row, int , const QModelIndex &)
{
    Q_ASSERT(m_producer);
    Q_ASSERT(m_producer->is_valid());
    if (m_producer && m_producer->is_valid()) {
        if (m_dropRow == -1)
            m_dropRow = row;
        return true;
    } else {
        return false;
    }
}

bool AttachedFiltersModel::removeRows(int row, int , const QModelIndex &parent)
{
     Q_ASSERT(m_producer);
    if (m_producer && m_producer->is_valid() && m_dropRow >= 0 && row != m_dropRow) {
        bool result = moveRows(parent, row, 1, parent, m_dropRow);
        m_dropRow = -1;
        return result;
    } else {
        return false;
    }
}

bool AttachedFiltersModel::moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationRow)
{
    Q_ASSERT(m_producer);
    Q_ASSERT(m_producer->is_valid());
    Q_ASSERT(m_event);
    Q_ASSERT(sourceParent == destinationParent);
    Q_ASSERT(count == 1);

    if (!m_producer || !m_producer->is_valid() || sourceParent != destinationParent || count != 1) {
        return false;
    }

    QModelIndex fromIndex = createIndex(sourceRow, 0);
    QModelIndex toIndex = createIndex(destinationRow, 0);

    if (fromIndex.isValid() && toIndex.isValid()) {
        if (beginMoveRows(sourceParent, sourceRow, sourceRow, destinationParent, destinationRow)) {
            if (destinationRow > sourceRow) {
                // Moving down: Convert to "post move" indexing
                destinationRow--;
            }
            int mltSrcIndex = m_mltIndexMap[sourceRow];
            int mltDstIndex = m_mltIndexMap[destinationRow];
            m_event->block();
            m_producer->move_filter(mltSrcIndex, mltDstIndex);
            m_event->unblock();
            // Adjust MLT index map for indices that just changed.
            m_mltIndexMap.removeAt(sourceRow);
            for (int i = 0; i < m_mltIndexMap.count(); i++) {
                if (m_mltIndexMap[i] > mltSrcIndex) {
                    m_mltIndexMap[i] = m_mltIndexMap[i] - 1;
                }
                if (m_mltIndexMap[i] >= mltDstIndex) {
                    m_mltIndexMap[i] = m_mltIndexMap[i] + 1;
                }
            }
            m_mltIndexMap.insert(destinationRow, mltDstIndex);
            m_metaList.move(sourceRow, destinationRow);
            endMoveRows();
            emit changed();
            return true;
        }
    }
    return false;
}

void AttachedFiltersModel::setDefaultValueForAllParemeters(Mlt::Filter* pFilter,QmlMetadata* pMetadata)
{
    for (int j=0;j<pMetadata->keyframes()->parameterCount();j++)
    {
        QString strProperty = pMetadata->keyframes()->parameter(j)->property();
        //frei0r滤镜参数的property都是“0”、“1”、“2”、“3”。。。规则固定的，而生成QmlMetadata的时候有可能并没有设置。所以需要在用的时候设置。
        if(strProperty == "")
            strProperty = QString::number(j);

        QString strValue = pMetadata->keyframes()->parameter(j)->defaultValue();
        //主要针对webvfx相关的几个滤镜会引用到.html文件，meta里面存的是相对路径，而使用的时候需要绝对路径
        if(strValue.contains(".html"))
            strValue = pMetadata->path().absolutePath().append('/') + strValue;
        pFilter->set(strProperty.toUtf8().constData(),strValue.toUtf8().constData());
    }

    if (pMetadata->objectName() == "dynamicText")
    {
        mlt_rect rectGeometry = pFilter->get_rect("geometry");
        int nSize = static_cast<int>(MLT.profile().height() * rectGeometry.h + 0.5);
        pFilter->set("size",nSize);
    }
}

void AttachedFiltersModel::add(QmlMetadata* meta, bool bFromUndo)
{
    if (MAIN.timelineDock()->selection().isEmpty() )
    {
        QMessageBox::warning(nullptr, tr("Tips"), tr("Tips\n\nTo apply the filter, please first select the clip by clicking on it on the timeline."), QMessageBox::Ok);
        return;
    }

    Q_ASSERT(meta);
    Q_ASSERT(m_producer);


    if (!m_producer || !m_producer->is_valid())
        return;

    if (!meta->allowMultiple()) {
        for (int i = 0; i < m_metaList.count(); i++) {
            const QmlMetadata* attachedMeta = m_metaList[i];
             Q_ASSERT(attachedMeta);
            if (attachedMeta && meta->uniqueId() == attachedMeta->uniqueId()) {
                emit duplicateAddFailed(i);
                return;
            }
        }
    }

    int insertIndex = -1;
    int mltIndex = -1;
    Mlt::Filter* filter = new Mlt::Filter(MLT.profile(), meta->mlt_service().toUtf8().constData());
    Q_ASSERT(filter);
    Q_ASSERT(filter->is_valid());

    if (filter->is_valid()) {
        if (!meta->objectName().isEmpty())
            filter->set(kShotcutFilterProperty, meta->objectName().toUtf8().constData());

        setDefaultValueForAllParemeters(filter,meta);

        insertIndex = 0;
        for (int i = m_metaList.count() - 1; i >= 0; i--)
        {
            if (!sortIsLess(m_metaList[i], meta))
            {
                insertIndex = i + 1;
                break;
            }
        }

        // Calculate the MLT index for the new filter.
        if (m_mltIndexMap.count() == 0)
        {
            mltIndex = m_producer->filter_count();
        } else if (insertIndex == 0)
        {
            mltIndex = m_mltIndexMap[0];
        } else
        {
            mltIndex = m_mltIndexMap[insertIndex -1] + 1;
        }

        beginInsertRows(QModelIndex(), insertIndex, insertIndex);
        if (MLT.isSeekable())
            MLT.pause();

        m_event->block();
        m_producer->attach(*filter);
        m_producer->move_filter(m_producer->filter_count() - 1, mltIndex);
        m_event->unblock();

        // Adjust MLT index map for indices that just changed.
        for (int i = 0; i < m_mltIndexMap.count(); i++)
        {
            if (m_mltIndexMap[i] >= mltIndex)
            {
                m_mltIndexMap[i] = m_mltIndexMap[i] + 1;
            }
        }
        m_mltIndexMap.insert(insertIndex, mltIndex);
        m_metaList.insert(insertIndex, meta);

        endInsertRows();

        emit filterAdded(insertIndex);
        emit changed();

        if(!bFromUndo)
        {
            int nCount = MAIN.undoStack()->count();
             QUndoCommand *attachCommand = new Timeline::FilterAttachCommand(*(MAIN.timelineDock()->model()), meta, m_metaList.count()-1, insertIndex, true,true);

             MAIN.pushCommand(attachCommand);
             nCount = MAIN.undoStack()->count();
            LOG_DEBUG() << "MAIN.undoStack()->count() " <<  nCount;
        }
        MAIN.timelineDock()->AttachedfilterChanged();
    }
    else LOG_WARNING() << "Failed to load filter" << meta->mlt_service();

    delete filter;
}

void AttachedFiltersModel::remove(int row, bool bFromUndo)
{
    Q_ASSERT(m_producer);
    Q_ASSERT(m_producer->is_valid());
    Q_ASSERT(m_event);

    if (row >= m_metaList.count()) {
        LOG_WARNING() << "Invalid index:" << row;
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    int mltIndex = m_mltIndexMap[row];
    Mlt::Filter* filter = m_producer->filter(mltIndex);

    Q_ASSERT(filter);

    m_event->block();
    m_producer->detach(*filter);
    m_event->unblock();
    // Adjust MLT index map for indices that just changed.
    m_mltIndexMap.removeAt(row);
    for (int i = 0; i < m_mltIndexMap.count(); i++) {
        if (m_mltIndexMap[i] > mltIndex) {
            m_mltIndexMap[i] = m_mltIndexMap[i] - 1;
        }
    }

    QmlMetadata *metaData = (m_metaList.at(row));
    Q_ASSERT(metaData);
    m_metaList.removeAt(row);

    endRemoveRows();

    emit filterRemoved(row);
    emit changed();

    delete filter;

    if(!bFromUndo)
    {
        MAIN.pushCommand(new Timeline::FilterAttachCommand(*(MAIN.timelineDock()->model()), metaData, row, row, false,true));
    }

    MAIN.timelineDock()->AttachedfilterChanged();
}

bool AttachedFiltersModel::move(int fromRow, int toRow, bool bFromUndo)
{
    QModelIndex parent = QModelIndex();

    if( fromRow == -1)
        fromRow = m_metaList.count() - 1;

    int toRowOld = toRow;

    if (fromRow < 0 || toRow < 0 || fromRow > m_metaList.length() || toRow >  m_metaList.length())
    {
        return false;
    }

    if (toRow > fromRow) {
        // Moving down: put it under the destination index
        toRow++;
    }

    bool bRet = moveRows(parent, fromRow, 1, parent, toRow);

    if(!bFromUndo && bRet)
    {
        MAIN.pushCommand(new Timeline::FilterMoveCommand(*(MAIN.timelineDock()->model()), fromRow, toRowOld,true));
    }
    return bRet;
}

void AttachedFiltersModel::reset(Mlt::Producer* producer)
{
    Q_ASSERT(producer);
//    Q_ASSERT(m_producer);
//    Q_ASSERT(m_event);

    beginResetModel();
    m_event.reset();

    //only load filter ui when selected the timeline item
    if (producer && producer->is_valid() && producer->get_int(kMultitrackItemProperty))
        m_producer.reset(new Mlt::Producer(producer));
//    else if (MLT.isClip())
//        m_producer.reset(new Mlt::Producer(MLT.producer()));
    else
        m_producer.reset();
    m_metaList.clear();
    m_mltIndexMap.clear();

//    int selectIndex = -1;

   //  Q_ASSERT(producer && producer->is_valid());
    if (m_producer && m_producer->is_valid()) {
        Mlt::Event* event = m_producer->listen("service-changed", this, reinterpret_cast<mlt_listener>(AttachedFiltersModel::producerChanged));
        Q_ASSERT(event);
        m_event.reset(event);
        int count = m_producer->filter_count();
        for (int i = 0; i < count; i++) {
            Mlt::Filter* filter = m_producer->filter(i);
            Q_ASSERT(filter);
            if (filter && filter->is_valid() && !filter->get_int("_loader")) {
                QmlMetadata* newMeta = MAIN.filterController()->metadataForService(filter);
                Q_ASSERT(newMeta);
                int newIndex = m_metaList.count();
                for (int j = newIndex - 1; j >= 0; j--) {
                    const QmlMetadata* prevMeta = m_metaList[j];
                    Q_ASSERT(prevMeta);
                    if (sortIsLess(prevMeta, newMeta)) {
                        newIndex = j;
                    } else {
                        break;
                    }
                }
                m_metaList.insert(newIndex, newMeta);
                m_mltIndexMap.insert(newIndex, i);

                //如果当前的producer是模板，且其中有sizeAndProducer滤镜时，自动选中sizeAndProducer滤镜
                QString templateFlag = QString(m_producer->get("moviemator:template"));
                if (templateFlag == "template")
                {
                    char* filter_name = filter->get("moviemator:filter");
                    if (QString(filter_name) == "affineSizePosition")
                    {
                        MAIN.onShowPropertiesVideoFilterDock();
                        MAIN.timelineDock()->selectSizeAndPositionFilter(newIndex);
//                        selectIndex = newIndex;
                    }
                }
            }
            delete filter;
        }
    }

    endResetModel();

    emitFiltersLoaded();

    emit trackTitleChanged();
    emit isProducerSelectedChanged();
    emit readyChanged();
}

void AttachedFiltersModel::producerChanged(mlt_properties, AttachedFiltersModel* model)
{
    Q_UNUSED(model);
//    model->reset(model->m_producer.data());
}


void AttachedFiltersModel::setFilter(AttachedMetadataFilter filter)
{
    beginResetModel();
    m_filter = filter;
    emit filterChanged();
    endResetModel();
}

bool AttachedFiltersModel::isVisible(int row) const
{
    if(row < 0)
        return false;

    if(m_metaList.count() <= row)
        return false;

    QmlMetadata* meta = m_metaList.at(row);
    Q_ASSERT(meta);
    if(!meta)
        return false;

    return !meta->isAudio();
}

void AttachedFiltersModel::emitFiltersLoaded()
{
    if (!m_producer || !m_producer->is_valid())
        return;

    QString strProducerType = QString::fromUtf8(m_producer->get(kProducerTypeProperty));

    if (strProducerType.compare("text-template", Qt::CaseInsensitive) == 0)
    {
        for (int i = 0; i < m_metaList.count(); i++)
        {
            QmlMetadata *pMetaData = m_metaList.at(i);
            QString strMltService = pMetaData->mlt_service();
            if (strMltService.compare("dynamictext", Qt::CaseInsensitive) == 0)
            {
                emit filtersLoadedForTextTemplateProducer(i);
                break;
            }
        }
    }
    else
    {
        emit filtersLoaded();
    }

    return;
}
