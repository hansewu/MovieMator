/*
 * Copyright (c) 2014-2016 Meltytech, LLC
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

#include "shotcut_mlt_properties.h"
#include "filtercontroller.h"
#include <QQmlEngine>
#include <QDir>
#include <QtWidgets>
#include <Logger.h>
#include <QQmlComponent>
#include <QTimerEvent>
#include "mltcontroller.h"
#include "settings.h"
#include "qmlmetadata.h"
#include <qmlutilities.h>
#include "qmltypes/qmlfilter.h"
#include <MltFilter.h>
#include <map>
#include <filterdockinterface.h>
#include "mainwindow.h"

FilterController::FilterController(QObject* parent) : QObject(parent),
 m_metadataModel(this),
 m_attachedModel(this),
 m_currentFilterIndex(-1)
{
    startTimer(0);
    connect(&m_attachedModel, SIGNAL(changed()), this, SLOT(handleAttachedModelChange()));
    connect(&m_attachedModel, SIGNAL(modelAboutToBeReset()), this, SLOT(handleAttachedModelAboutToReset()));
    connect(&m_attachedModel, SIGNAL(rowsRemoved(const QModelIndex&,int,int)), this, SLOT(handleAttachedRowsRemoved(const QModelIndex&,int,int)));
    connect(&m_attachedModel, SIGNAL(rowsInserted(const QModelIndex&,int,int)), this, SLOT(handleAttachedRowsInserted(const QModelIndex&,int,int)));
    connect(&m_attachedModel, SIGNAL(duplicateAddFailed(int)), this, SLOT(handleAttachDuplicateFailed(int)));
}

void FilterController::loadFilterMetadata() {
    QDir dir = QmlUtilities::qmlDir();
    dir.cd("filters_pro");
    foreach (QString dirName, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Executable)) {
        if (dirName == "frei0r")
        {
            loadFrei0rFilterMetadata();
            continue;
        }

        QDir subdir = dir;
        subdir.cd(dirName);
        subdir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
        subdir.setNameFilters(QStringList("meta*.qml"));
        foreach (QString fileName, subdir.entryList()) {
            LOG_DEBUG() << "reading filter metadata" << dirName << fileName;
            QQmlComponent component(QmlUtilities::sharedEngine(), subdir.absoluteFilePath(fileName));
            QmlMetadata *meta = qobject_cast<QmlMetadata*>(component.create());
            if (meta) {
                // Check if mlt_service is available.
                if (MLT.repository()->filters()->get_data(meta->mlt_service().toLatin1().constData())) {
//                    LOG_DEBUG() << "added filter" << meta->name();
                    meta->loadSettings();
                    meta->setPath(subdir);
                    meta->setParent(0);
                    addMetadata(meta);
                }
            } else if (!meta) {
                LOG_WARNING() << component.errorString();
            }
        }
    };
}

void FilterController::readFilterTypeFromFile(QString &pFilePath, std::map<QString, QString> &filterTypes)
{
    QFile file(pFilePath);
    if(!file.open(QIODevice::ReadOnly))  return;

    QTextStream in(&file);
    while( !in.atEnd())
    {
        QString lineString = in.readLine();
        QStringList filterType = lineString.split(':');
        filterTypes.insert(std::make_pair(filterType.at(0), filterType.at(1)));
    }
    file.close();
}

void FilterController::loadFrei0rFilterMetadata() {
    QDir dir = QmlUtilities::qmlDir();
    dir.cd("filters_pro");
    QDir subdir = dir;
    subdir.cd("frei0r");
    subdir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    subdir.setNameFilters(QStringList("meta*.qml"));
    foreach (QString fileName, subdir.entryList())
    {
//        QString filePath = qApp->applicationDirPath() + "/../Resources/frei0r.txt";
//        std::map<QString, QString> filterTypes;
//        readFilterTypeFromFile(filePath, filterTypes);

        QDir applicationDir(qApp->applicationDirPath());
        applicationDir.cd("lib");
        applicationDir.cd("frei0r-1");
        QDir frei0rDir = applicationDir;
        frei0rDir.setNameFilters(QStringList("*.so"));
        foreach (QString libName, frei0rDir.entryList(QDir::NoFilter))
        {
            QQmlComponent component(QmlUtilities::sharedEngine(), subdir.absoluteFilePath(fileName));
            QmlMetadata *meta = qobject_cast<QmlMetadata*>(component.create());
            if (meta)
            {
//                std::map<QString, QString>::iterator iter = filterTypes.find(libName.mid(0, libName.length() - 3));
//                if (iter != filterTypes.end())
//                {
//                    QString filterType = iter->second;
//                    meta->setFilterType(filterType);
//                }
                std::string string = libName.toStdString();
                if(string.find("alpha") != std::string::npos )
                    continue;

                QString mlt_service_s       = "frei0r." + libName.mid(0, libName.length() - 3);

//                    Mlt::Properties *metadata = MLT.repository()->metadata(filter_type, mlt_service_s.toUtf8().constData());

                mlt_repository repository   = mlt_factory_repository();
                mlt_properties metadata     = mlt_repository_metadata( repository, filter_type, mlt_service_s.toUtf8().constData());

                QString producerType = QString::fromUtf8(mlt_properties_get( metadata, "type"));
                if (producerType == "filter")         //frei0r只开放出filter的库
                    meta->setType(QmlMetadata::Filter);
                else
                    continue;

                QString name        = QString::fromUtf8(mlt_properties_get( metadata, "title"));
                QString description = QString::fromUtf8(mlt_properties_get( metadata, "description"));

                meta->setName(tr("%1").arg(name.toUtf8().constData()));
                meta->set_mlt_service(mlt_service_s);
                meta->keyframes()->clearParameter();
                //parameters info
                if ( metadata )
                {
                    mlt_properties params = (mlt_properties) mlt_properties_get_data(metadata, "parameters", NULL);
                    if ( params )
                    {
                        int n = mlt_properties_count( params );
                        for ( int nIndex = 0; nIndex < n; nIndex++ )
                        {
                            mlt_properties param_pro = (mlt_properties) mlt_properties_get_data( params, mlt_properties_get_name(params, nIndex), NULL );

                            QmlKeyframesParameter * param = new QmlKeyframesParameter();

                            QString parmType = QString::fromUtf8(mlt_properties_get(param_pro, "type"));
                            if (parmType == "boolean")
                            {
                                param->setParaType("bool");
                                param->setMinimum(QString::fromUtf8(mlt_properties_get(param_pro, "minimum")).toDouble());
                                param->setMaximum(QString::fromUtf8(mlt_properties_get(param_pro, "maximum")).toDouble());
                                param->setDefaultValue(QString::number(mlt_properties_get_int(param_pro, "default")));
//                                LOG_DEBUG() << "param F0R_PARAM_BOOL" << meta->name() << QString::number(mlt_properties_get_int(param_pro, "default"));
                            }
                            else if(parmType == "float")
                                {
                                    param->setParaType("double");
                                    param->setMinimum(QString::fromUtf8(mlt_properties_get(param_pro, "minimum")).toDouble());
                                    param->setMaximum(QString::fromUtf8(mlt_properties_get(param_pro, "maximum")).toDouble());
                                    param->setDefaultValue(QString::number(mlt_properties_get_double(param_pro, "default")));

//                                    LOG_DEBUG() << "param F0R_PARAM_DOUBLE" << meta->name() << QString::number(mlt_properties_get_double(param_pro, "default"));
                                }
                            else if(parmType == "color")
                                {
                                    param->setParaType("color");
                                    mlt_rect rect = mlt_properties_get_rect(param_pro, "default");
                                    QString sValue = QString("%1 %2 %3 %4 %5").arg(rect.x).arg(rect.y).arg(rect.w).arg(rect.h).arg(rect.o);
                                    param->setDefaultValue(sValue);

//                                    LOG_DEBUG() << "param F0R_PARAM_COLOR" << meta->name() << sValue;
                                }
                            else if(parmType == "string")
                            {
                                param->setParaType("string");
                                param->setDefaultValue(QString::fromUtf8(mlt_properties_get(param_pro, "default")));

//                              LOG_DEBUG() << "param F0R_PARAM_STRING" << meta->name() << QString::fromUtf8(mlt_properties_get(param_pro, "default"));
                            }
                            else if(parmType == "position")
                            {
                                param->setParaType("position");
                            }

                            param->setName(QString::fromUtf8(mlt_properties_get(param_pro, "title")));
                            param->setExplanation(QString::fromUtf8(mlt_properties_get(param_pro, "description")));
//                          param->setControlType(QString::fromUtf8(mlt_properties_get(param_pro, "widget")));

                            meta->keyframes()->appendParameter(param);
                        }
                    }
                }

                // Check if mlt_service is available.
                if (MLT.repository()->filters()->get_data(meta->mlt_service().toLatin1().constData())) {
//                    LOG_DEBUG() << "added filter 2" << meta->name();

                    meta->loadSettings();
                    meta->setPath(subdir);
                    meta->setParent(0);
                    addMetadata(meta);
                }
            } else if (!meta) {
                LOG_WARNING() << component.errorString();
            }
        }
    }

}

void FilterController::getFrei0rPluginInfo(Mlt::Filter *filter, f0r_plugin_info_t &info)
{
    mlt_service service = filter->get_service();
    mlt_properties prop = MLT_SERVICE_PROPERTIES(service);

    if(!prop) return;

    void (*f0r_get_plugin_info)(f0r_plugin_info_t*) = (void (*)(f0r_plugin_info_t*))mlt_properties_get_data( prop, "f0r_get_plugin_info" ,NULL);
    f0r_get_plugin_info(&info);
}

void FilterController::getFrei0rParamInfo(Mlt::Filter *filter, int index, f0r_param_info_t &info)
{
    mlt_service service = filter->get_service();
    mlt_properties prop = MLT_SERVICE_PROPERTIES(service);
    void (*f0r_get_param_info)(f0r_param_info_t*, int) = (void (*)(f0r_param_info_t*, int))mlt_properties_get_data( prop, "f0r_get_param_info" ,NULL);

    f0r_get_param_info(&info, index);
}

QmlMetadata *FilterController::metadataForService(Mlt::Service *service)
{
    QmlMetadata* meta = 0;
    int rowCount = m_metadataModel.rowCount();
    QString uniqueId = service->get(kShotcutFilterProperty);

    // Fallback to mlt_service for legacy filters
    if (uniqueId.isEmpty()) {
        uniqueId = service->get("mlt_service");
    }

    for (int i = 0; i < rowCount; i++) {
        QmlMetadata* tmpMeta = m_metadataModel.get(i);
        if (tmpMeta->uniqueId() == uniqueId) {
            meta = tmpMeta;
            break;
        }
    }

    return meta;
}

void FilterController::updateFilterDock()
{
    int nFilterCount = m_metadataModel.rowCount();

    Filter_Info filterInfos[200];
    for (int nIndex = 0; nIndex < nFilterCount; nIndex++)
    {
        QmlMetadata* metadataModel          = m_metadataModel.get(nIndex);

        strcpy(filterInfos[nIndex].name, metadataModel->name().toStdString().c_str());
        strcpy(filterInfos[nIndex].type, metadataModel->filterType().toStdString().c_str());

        QString imageSourcePath             = "qrc:///icons/filters/" + metadataModel->mlt_service()  + ".png";
        if(metadataModel->mlt_service().toStdString().find("frei0r.") != std::string::npos)
            imageSourcePath             = "qrc:///icons/filters/Common.png";

        if(metadataModel->isAudio() == true)
            imageSourcePath             = "qrc:///icons/filters/Audio.png";

        strcpy(filterInfos[nIndex].imageSourcePath, imageSourcePath.toStdString().c_str());

        bool bVisible = true;
        if (metadataModel->isHidden()) bVisible = false;
        if (metadataModel->needsGPU() && !Settings.playerGPU()) bVisible = false;
        if (!metadataModel->needsGPU() && Settings.playerGPU() && !metadataModel->gpuAlt().isEmpty()) bVisible = false;

        filterInfos[nIndex].visible = bVisible;
    }

    setFiltersInfo(filterInfos, nFilterCount);
}

void FilterController::timerEvent(QTimerEvent* event)
{
    loadFilterMetadata();
    updateFilterDock();
    killTimer(event->timerId());
}

MetadataModel* FilterController::metadataModel()
{
    return &m_metadataModel;
}

AttachedFiltersModel* FilterController::attachedModel()
{
    return &m_attachedModel;
}

void FilterController::setProducer(Mlt::Producer *producer)
{
    if (producer && producer->get_int("meta.fx_cut"))
        return;
    m_attachedModel.setProducer(producer);
    if (producer && producer->is_valid()) {
        mlt_service_type service_type = producer->type();
        m_metadataModel.setIsClipProducer(service_type != tractor_type
            && service_type != playlist_type);
    }

}

void FilterController::setCurrentFilter(int attachedIndex)
{
    Q_ASSERT(attachedIndex <= m_attachedModel.rowCount());

    if (attachedIndex == m_currentFilterIndex) {
        return;
    }
    m_currentFilterIndex = attachedIndex;

    QmlMetadata* meta = m_attachedModel.getMetadata(m_currentFilterIndex);
    QmlFilter* filter = 0;
    if (meta) {
        Mlt::Filter* mltFilter = m_attachedModel.getFilter(m_currentFilterIndex);

        filter = new QmlFilter(mltFilter, meta);

    }

    emit currentFilterAboutToChange();
    emit currentFilterChanged(filter, meta, m_currentFilterIndex);
    m_currentFilter.reset(filter);
}

void FilterController::refreshCurrentFilter(Mlt::Filter *filter)
{
    if(m_currentFilterIndex == -1) return;

    QmlFilter *qmlFilter = m_currentFilter.data();
    if(!qmlFilter) return;

    Mlt::Filter* mltFilter = qmlFilter->getMltFilter();
    if(mltFilter->get_filter() != filter->get_filter())
    {
        return;
    }

    QmlMetadata* meta = m_attachedModel.getMetadata(m_currentFilterIndex);
 /*   QmlFilter* qfilter = 0;
    if (meta)
    {
        Mlt::Filter* mltFilter = m_attachedModel.getFilter(m_currentFilterIndex);
        qfilter = new QmlFilter(mltFilter, meta);
    }
*/
 //   emit currentFilterAboutToChange();
    emit currentFilterChanged(m_currentFilter.data(), meta, m_currentFilterIndex);

//    m_currentFilter.reset(qfilter);
}

void FilterController::refreshKeyFrame(Mlt::Filter *filter, const QVector<key_frame_item> &listKeyFrame)
{
    if(m_currentFilterIndex == -1) return;

    QmlFilter *qmlFilter = m_currentFilter.data();
    if(!qmlFilter) return;

    Mlt::Filter* mltFilter = qmlFilter->getMltFilter();
    if(mltFilter->get_filter() != filter->get_filter())
    {
        return;
    }

    qmlFilter->refreshKeyFrame(listKeyFrame);
}

void FilterController::handleAttachedModelChange()
{
    MLT.refreshConsumer();
}

void FilterController::handleAttachedModelAboutToReset()
{
    setCurrentFilter(-1);
}

void FilterController::handleAttachedRowsRemoved(const QModelIndex&, int first, int)
{
    int newFilterIndex = first;
    if (newFilterIndex >= m_attachedModel.rowCount()) {
        newFilterIndex = m_attachedModel.rowCount() - 1;
    }
    m_currentFilterIndex = -2; // Force update
    setCurrentFilter(newFilterIndex);
}

void FilterController::handleAttachedRowsInserted(const QModelIndex&, int first, int)
{
    m_currentFilterIndex = first;
    Mlt::Filter* mltFilter = m_attachedModel.getFilter(m_currentFilterIndex);
    QmlMetadata* meta = m_attachedModel.getMetadata(m_currentFilterIndex);
    QmlFilter* filter = new QmlFilter(mltFilter, meta);
    filter->setIsNew(true);
    emit currentFilterAboutToChange();
    emit currentFilterChanged(filter, meta, m_currentFilterIndex);
    m_currentFilter.reset(filter);
}

void FilterController::handleAttachDuplicateFailed(int index)
{
    Q_ASSERT(index < m_attachedModel.rowCount());

    const QmlMetadata* meta = m_attachedModel.getMetadata(index);
    Q_ASSERT(meta);
    emit statusChanged(tr("Only one %1 filter is allowed.").arg(meta->name()));
    setCurrentFilter(index);
}

void FilterController::addMetadata(QmlMetadata* meta)
{
    Q_ASSERT(meta);
    m_metadataModel.add(meta);
}


void FilterController::addPositionAndSizeFilter()
{

    QmlMetadata *meta = metadataForUniqueId("affineSizePosition");
    Q_ASSERT(meta);
    m_attachedModel.add(meta);

}

void FilterController::addRotateFilter()
{

    QmlMetadata *meta = metadataForUniqueId("affineRotate");
    Q_ASSERT(meta);
    m_attachedModel.add(meta);
}

void FilterController::addCropFilter()
{

    QmlMetadata *meta = metadataForUniqueId("crop");
    Q_ASSERT(meta);
    m_attachedModel.add(meta);
}

void FilterController::addVolumeFilter()
{

    QmlMetadata *meta = metadataForUniqueId("volume");
    Q_ASSERT(meta);
    m_attachedModel.add(meta);
}


void FilterController::addFadeInAudioFilter()
{

    QmlMetadata *meta = metadataForUniqueId("fadeInVolume");
    Q_ASSERT(meta);
    m_attachedModel.add(meta);

}

void FilterController::addFadeOutAudioFilter()
{
    QmlMetadata *meta = metadataForUniqueId("fadeOutVolume");
    Q_ASSERT(meta);
    m_attachedModel.add(meta);

}


void FilterController::addFadeInVideoFilter()
{
    QmlMetadata *meta = metadataForUniqueId("fadeInBrightness");
    Q_ASSERT(meta);
    m_attachedModel.add(meta);


}

void FilterController::addFadeOutVideoFilter()
{
    QmlMetadata *meta = metadataForUniqueId("fadeOutBrightness");
    Q_ASSERT(meta);
    m_attachedModel.add(meta);


}

void FilterController::addTextFilter()
{
    QmlMetadata *meta = metadataForUniqueId("dynamicText");
    Q_ASSERT(meta);
    m_attachedModel.add(meta);
}


QmlMetadata *FilterController::metadataForUniqueId(const char *uniqueId)
{
    QmlMetadata* meta = 0;
    int rowCount = m_metadataModel.rowCount();
    QString qstrUniqueId(uniqueId);

    for (int i = 0; i < rowCount; i++) {
        QmlMetadata* tmpMeta = m_metadataModel.get(i);
        if (tmpMeta->uniqueId() == qstrUniqueId) {
            meta = tmpMeta;
            break;
        }
    }

    return meta;
}

void FilterController::addFilter(int nFilterIndex)
{
    MAIN.onShowFilterDock();
    int nCurrentFilter = nFilterIndex;
    if(nCurrentFilter == -1)
        nCurrentFilter = getCurrentSelectedFilterIndex();

    QmlMetadata *meta = m_metadataModel.get(nCurrentFilter);

    if (meta)
        m_attachedModel.add(meta);
}

void FilterController::addFilter(const QString &filterID)
{
    QmlMetadata *meta = metadataForUniqueId(filterID.toUtf8().constData());
    if (meta)
        m_attachedModel.add(meta);
}

void FilterController::removeFilter(int row)
{
    m_attachedModel.remove(row);
}

