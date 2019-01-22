/*
 * Copyright (c) 2014 Meltytech, LLC
 * Author: Brian Matherly <code@brianmatherly.com>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: wyl <wyl@pylwyl.local>
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

#ifndef FILTERCONTROLLER_H
#define FILTERCONTROLLER_H

#include <QObject>
#include <QScopedPointer>
#include <QFuture>
#include "models/metadatamodel.h"
#include "models/attachedfiltersmodel.h"
#include "qmlmetadata.h"
#include "qmltypes/qmlfilter.h"
#include <../frei0r.h>

class QTimerEvent;

class FilterController : public QObject
{
    Q_OBJECT

public:
    explicit FilterController(QObject* parent = 0);
    MetadataModel* metadataModel();
    AttachedFiltersModel* attachedModel();

    QmlMetadata* metadataForService(Mlt::Service *service);
    QmlMetadata* metadataForUniqueId(const char *uniqueId);

protected:
    void timerEvent(QTimerEvent*);

signals:
    void currentFilterAboutToChange();
    void currentFilterChanged(QObject* filter, QmlMetadata* meta, int index);
    void statusChanged(QString);

public slots:
    void setProducer(Mlt::Producer *producer = 0);
    void setCurrentFilter(int attachedIndex);

    //add positon&size filter
    void addPositionAndSizeFilter();
    void addRotateFilter();
    void addCropFilter();
    void addFadeInVideoFilter();
    void addFadeOutVideoFilter();
    void addFadeInAudioFilter();
    void addFadeOutAudioFilter();
    void addVolumeFilter();
    void addTextFilter();

    void addFilter(const QString& filterID);
    void addFilter(int nFilterIndex = -1);
    void removeFilter(int row);

    void refreshCurrentFilter(Mlt::Filter *filter); //wzq
    void refreshKeyFrame(Mlt::Filter *filter, const QVector<key_frame_item> &listKeyFrame); //wzq

private slots:
    void handleAttachedModelChange();
    void handleAttachedModelAboutToReset();
    void addMetadata(QmlMetadata*);
    void handleAttachedRowsRemoved(const QModelIndex & parent, int first, int last);
    void handleAttachedRowsInserted(const QModelIndex & parent, int first, int last);
    void handleAttachDuplicateFailed(int index);

private:
    void loadFilterMetadata();
    void loadFrei0rFilterMetadata();
    void readFilterTypeFromFile(QString &pFilePath, std::map<QString, QString> &filterTypes);
    void getFrei0rPluginInfo(Mlt::Filter *filter, f0r_plugin_info_t &info);
    void getFrei0rParamInfo(Mlt::Filter *filter, int index, f0r_param_info_t &info);

    void updateFilterDock();

    QFuture<void> m_future;
    QScopedPointer<QmlFilter> m_currentFilter;
    MetadataModel m_metadataModel;
    AttachedFiltersModel m_attachedModel;
    int m_currentFilterIndex;
};

#endif // FILTERCONTROLLER_H
