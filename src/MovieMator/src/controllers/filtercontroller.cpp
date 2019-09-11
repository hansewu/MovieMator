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
#include "mainwindow.h"
#include "util.h"
#include <assert.h>

FilterController::FilterController(QObject* parent) : QObject(parent),
 m_metadataModel(this),
 m_attachedModel(this),
 m_currentFilterIndex(-1)
{
    startTimer(0);

    connect(&m_attachedModel, SIGNAL(changed()), this, SLOT(handleAttachedModelChange()));
    connect(&m_attachedModel, SIGNAL(modelAboutToBeReset()), this, SLOT(handleAttachedModelAboutToReset()));
    connect(&m_attachedModel, SIGNAL(rowsRemoved(const QModelIndex&,int,int)), this, SLOT(handleAttachedRowsRemoved(const QModelIndex&,int,int)));
    connect(&m_attachedModel, SIGNAL(rowsInserted(const QModelIndex&,int,int)), this, SLOT(handleAttachedRowsInserted(const QModelIndex&,int,int)), Qt::QueuedConnection);
    connect(&m_attachedModel, SIGNAL(duplicateAddFailed(int)), this, SLOT(handleAttachDuplicateFailed(int)));
}

void FilterController::timerEvent(QTimerEvent* event)
{
    loadFilterMetadata();

    killTimer(event->timerId());
}

QList<QString> strToList(QString strSrc)
{
    strSrc = strSrc.replace("'","");
    strSrc = strSrc.replace("[","");
    strSrc = strSrc.replace("]","");
    return strSrc.split(",");
}

void FilterController::loadFilterParameter(QmlMetadata *pMetadata)
{
    bool bFrei0r = false;
    if(pMetadata->mlt_service().contains("frei0r"))
        bFrei0r = true;

    mlt_repository repository   = mlt_factory_repository();
    mlt_properties metadata = nullptr;
    if((pMetadata->mlt_service() == "affine")||(pMetadata->mlt_service() == "size_position"))
        metadata = mlt_repository_metadata( repository, transition_type, pMetadata->mlt_service().toUtf8().constData());
    else
        metadata = mlt_repository_metadata( repository, filter_type, pMetadata->mlt_service().toUtf8().constData());

    Q_ASSERT(metadata);
    if(metadata == nullptr) return;

    mlt_properties params = static_cast<mlt_properties>(mlt_properties_get_data(metadata, "parameters", nullptr));
    if ( params )
    {
        int nPropertyCount = mlt_properties_count( params );
        for ( int nIndex = 0; nIndex < nPropertyCount; nIndex++ )
        {
            mlt_properties paramProperties = static_cast<mlt_properties>(mlt_properties_get_data( params, mlt_properties_get_name(params, nIndex), nullptr ));

            QmlKeyframesParameter * pParameter = new QmlKeyframesParameter();
            Q_ASSERT(pParameter);

            QString strAdjustedByControls = QString::fromUtf8(mlt_properties_get(paramProperties, "bAdjustedByControls"));
            if((strAdjustedByControls != "0")&&(!bFrei0r)) continue;

            QString strParmType = QString::fromUtf8(mlt_properties_get(paramProperties, "type"));
            if (strParmType == "boolean")
            {
                pParameter->setParaType("bool");
                pParameter->setMinimum(QString::fromUtf8(mlt_properties_get(paramProperties, "minimum")).toDouble());
                pParameter->setMaximum(QString::fromUtf8(mlt_properties_get(paramProperties, "maximum")).toDouble());
                pParameter->setDefaultValue(QString::number(mlt_properties_get_int(paramProperties, "default")));
            }
            else if(strParmType == "float" || strParmType == "integer")
            {
                pParameter->setParaType("double");
                pParameter->setMinimum(QString::fromUtf8(mlt_properties_get(paramProperties, "minimum")).toDouble());
                pParameter->setMaximum(QString::fromUtf8(mlt_properties_get(paramProperties, "maximum")).toDouble());
                pParameter->setDefaultValue(QString::number(mlt_properties_get_double(paramProperties, "default")));
            }
            else if(strParmType == "color")
            {
                pParameter->setParaType("color");
                mlt_rect rect = mlt_properties_get_rect(paramProperties, "default");
                QString strValue = QString("%1 %2 %3 %4 %5").arg(rect.x).arg(rect.y).arg(rect.w).arg(rect.h).arg(rect.o);
                pParameter->setDefaultValue(strValue);
            }
            else if(strParmType == "string")
            {
                pParameter->setParaType("string");
                pParameter->setDefaultValue(QString::fromUtf8(mlt_properties_get(paramProperties, "default")));
            }
            else if(strParmType == "position")
            {
                pParameter->setParaType("position");
            }
            else if(strParmType == "rect")
            {
                pParameter->setParaType("rect");
                pParameter->setDefaultValue(QString::number(mlt_properties_get_double(paramProperties, "default")));
            }

            pParameter->setProperty(QString::fromUtf8(mlt_properties_get(paramProperties, "identifier")));
            if((pMetadata->mlt_service() == "affine")||(pMetadata->mlt_service() == "size_position"))
                pParameter->setProperty("transition."+pParameter->property());
            pParameter->setName(QString::fromUtf8(mlt_properties_get(paramProperties, "title")));
            pParameter->setExplanation(QString::fromUtf8(mlt_properties_get(paramProperties, "description")));

            pParameter->setObjectName(QString::fromUtf8(mlt_properties_get(paramProperties, "objectName")));
            pParameter->setControlType(QString::fromUtf8(mlt_properties_get(paramProperties, "controlType")));
            pParameter->setFactorFunc(strToList(QString::fromUtf8(mlt_properties_get(paramProperties, "factorFunc"))));

            pMetadata->keyframes()->appendParameter(pParameter);
        }
    }
}

void FilterController::loadFilterMetadata()
{
    QDir dir = QmlUtilities::qmlDir();
    dir.cd("filters_pro");

    foreach (QString strDirName, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Executable))
    {
        if (strDirName == "frei0r")
        {
            loadFrei0rFilterMetadata();
            continue;
        }

        QDir subdir = dir;
        subdir.cd(strDirName);
        subdir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
        subdir.setNameFilters(QStringList("meta*.qml"));

        foreach (QString strFileName, subdir.entryList())
        {
            LOG_DEBUG() << "reading filter metadata" << strDirName << strFileName;
            QQmlComponent component(QmlUtilities::sharedEngine(), subdir.absoluteFilePath(strFileName));
            QmlMetadata *pMetadata = qobject_cast<QmlMetadata*>(component.create());
            if (pMetadata)
            {
                // Check if mlt_service is available.
                if (MLT.repository()->filters()->get_data(pMetadata->mlt_service().toLatin1().constData()))
                {
                    QString strThumbnail = getFilterThumbnailPath(pMetadata->name(), pMetadata->isAudio());
                    pMetadata->setThumbnail(strThumbnail);
                    pMetadata->loadSettings();
                    pMetadata->setPath(subdir);
                    pMetadata->setParent(nullptr);

                    loadFilterParameter(pMetadata);

                    addMetadata(pMetadata);
                }
            }
            else if (!pMetadata)
            {
                LOG_WARNING() << component.errorString();
            }
        }
    }

    emit filtersInfoLoaded();
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

void FilterController::loadFrei0rFilterMetadata()
{
    QDir dir = QmlUtilities::qmlDir();
    dir.cd("filters_pro");
    QDir subdir = dir;
    subdir.cd("frei0r");
    subdir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    subdir.setNameFilters(QStringList("meta*.qml"));

    foreach (QString strFileName, subdir.entryList())
    {
        QString strFilePath = Util::resourcesPath() + "/filters/frei0r.txt";

        std::map<QString, QString> filterTypes;
        readFilterTypeFromFile(strFilePath, filterTypes);

        QDir applicationDir(qApp->applicationDirPath());
        applicationDir.cd("lib");
        applicationDir.cd("frei0r-1");
        QDir frei0rDir = applicationDir;
#ifdef Q_OS_WIN
        frei0rDir.setNameFilters(QStringList("*.dll"));
#else
        frei0rDir.setNameFilters(QStringList("*.so"));
#endif

        foreach (QString strLibName, frei0rDir.entryList(QDir::NoFilter))
        {
            QQmlComponent component(QmlUtilities::sharedEngine(), subdir.absoluteFilePath(strFileName));
            QmlMetadata *pMetadata = qobject_cast<QmlMetadata*>(component.create());
            if (pMetadata)
            {
                std::map<QString, QString>::iterator iter = filterTypes.find(strLibName.mid(0, strLibName.length() - 3));
#ifdef Q_OS_WIN
                iter       = filterTypes.find(strLibName.mid(0, strLibName.length() - 4));
#endif
                if (iter != filterTypes.end())
                {
                    QString filterType = iter->second;
                    pMetadata->setFilterType(filterType);
                }
                std::string sLibName = strLibName.toStdString();
                if(sLibName.find("alpha") != std::string::npos )   //remove alpha
                    continue;
                if(sLibName.find("curves") != std::string::npos )  //remove curves
                    continue;

                QString strMltService       = "frei0r." + strLibName.mid(0, strLibName.length() - 3);
#ifdef Q_OS_WIN
                strMltService       = "frei0r." + strLibName.mid(0, strLibName.length() - 4);
#endif

//                    Mlt::Properties *metadata = MLT.repository()->metadata(filter_type, mlt_service_s.toUtf8().constData());

                mlt_repository repository   = mlt_factory_repository();
                mlt_properties metadata     = mlt_repository_metadata( repository, filter_type, strMltService.toUtf8().constData());

                if (metadata)
                {
                QString strProducerType = QString::fromUtf8(mlt_properties_get( metadata, "type"));
                if (strProducerType == "filter")         //frei0r只开放出filter的库
                    pMetadata->setType(QmlMetadata::Filter);
                else
                    continue;

                QString strName        = QString::fromUtf8(mlt_properties_get( metadata, "title"));
                QString strDescription = QString::fromUtf8(mlt_properties_get( metadata, "description"));

                pMetadata->setName(tr("%1").arg(strName.toUtf8().constData()));
                pMetadata->set_mlt_service(strMltService);
                pMetadata->setObjectName(strMltService.replace(QRegExp("\\."), "_"));
                pMetadata->keyframes()->clearParameter();
                //parameters info
                //if ( metadata )
                //{
                loadFilterParameter(pMetadata);
                //}

                // Check if mlt_service is available.
                if (MLT.repository()->filters()->get_data(pMetadata->mlt_service().toLatin1().constData()))
                {
//                    LOG_DEBUG() << "added filter 2" << meta->name();
                    QString strThumbnail = getFilterThumbnailPath(pMetadata->name(), pMetadata->isAudio());
                    pMetadata->setThumbnail(strThumbnail);

                    pMetadata->loadSettings();
                    pMetadata->setPath(subdir);
                    pMetadata->setParent(nullptr);
                    addMetadata(pMetadata);
                }
                }
            }
            else if (!pMetadata)
            {
                LOG_WARNING() << component.errorString();
            }
        }
    }

}

//void FilterController::getFrei0rPluginInfo(Mlt::Filter *filter, f0r_plugin_info_t &info)
//{
//    Q_ASSERT(filter);

//    mlt_service service = filter->get_service();
//    mlt_properties prop = MLT_SERVICE_PROPERTIES(service);

//    if(!prop) return;

//    void (*f0r_get_plugin_info)(f0r_plugin_info_t*) = (void (*)(f0r_plugin_info_t*))mlt_properties_get_data( prop, "f0r_get_plugin_info" ,NULL);
//    f0r_get_plugin_info(&info);
//}

//void FilterController::getFrei0rParamInfo(Mlt::Filter *filter, int index, f0r_param_info_t &info)
//{
//    Q_ASSERT(filter);

//    mlt_service service = filter->get_service();
//    mlt_properties prop = MLT_SERVICE_PROPERTIES(service);
//    void (*f0r_get_param_info)(f0r_param_info_t*, int) = (void (*)(f0r_param_info_t*, int))mlt_properties_get_data( prop, "f0r_get_param_info" ,NULL);

//    f0r_get_param_info(&info, index);
//}

QmlMetadata *FilterController::metadataForService(Mlt::Service *service)
{
    Q_ASSERT(service);

    QmlMetadata* meta = nullptr;
    int rowCount = m_metadataModel.rowCount();
    QString uniqueId = service->get(kShotcutFilterProperty);

    // Fallback to mlt_service for legacy filters
    if (uniqueId.isEmpty()) {
        uniqueId = service->get("mlt_service");
    }

    for (int i = 0; i < rowCount; i++) {
        QmlMetadata* tmpMeta = m_metadataModel.get(i);
        Q_ASSERT(tmpMeta);
        if (tmpMeta->uniqueId() == uniqueId) {
            meta = tmpMeta;
            break;
        }
    }

    return meta;
}

QString FilterController::getFilterThumbnailPath(QString filterName, bool isAudio)
{
    const QMap<QString, QString> filterNameMap = {
        {"Common", "常用"},
        {"Size and Position", "变换"},
        {"Rotate", "旋转"},
        {"Mirror", "镜像"},
        {"Wave", "波形"},
        {"3D Text", "3D文字"},
        {"Text", "文字"},
        {"Sepia Tone", "棕褐色调"},
        {"Blur", "模糊"},
        {"Crop", "裁剪"},
        {"Stabilize", "防抖动"},
        {"Brightness", "亮度"},
        {"Vignette", "光晕"},
        {"Contrast", "对比度"},
        {"White Balance", "白平衡"},
        {"Color Grading", "色彩分级"},
        {"Saturation", "饱和度"},
        {"Invert Colors", "反色"},
        {"Glow", "发光"},
        {"Overlay", "叠加"},
        {"Circular Frame", "环形边框"},
        {"Old Film-Grain", "老电影：颗粒"},
        {"Fade In Video", "视频淡入"},
        {"Fade Out Video", "视频淡出"},
        {"Opacity", "透明度"},
        {"Sharpen", "锐化"},
        {"Reduce Noise", "降噪"},
    };

//    QLocale ql;
//    if(ql.language() == QLocale::Chinese)
    QString strLanguage = Settings.language();
    if((strLanguage == "zh") || (strLanguage == "zh_CN"))
    {
        QMap<QString, QString>::const_iterator iter;
        for (iter = filterNameMap.constBegin(); iter != filterNameMap.constEnd(); ++iter )
        {
            if(iter.value() == filterName)
                filterName = iter.key();
        }
    }

    QString imageSourcePath     = "qrc:///icons/filters/" + filterName  + ".jpg";

    QString path = imageSourcePath.right(imageSourcePath.length() - 3);
    QFile file(path);
    if(!file.exists())
        imageSourcePath         = "qrc:///icons/filters/Common.jpg";

    if(isAudio == true)
        imageSourcePath         = "qrc:///icons/filters/Audio.png";

    return imageSourcePath;
}

QString FilterController::getFilterClassNameZH(QString strFilterType)
{
    const QMap<QString, QString> filterTypeMap =
    {
        {"1 Basic Processing", "1 常用"},
        {"2 Text", "2 文字"},
        {"3 Basic Coloring Tool",  "3 基础调色"},
        {"4 Denoise and Blur", "4 降噪和模糊"},
        {"5 Advanced Coloring Tool",  "5 高级调色"},
        {"6 Light",  "6 光效"},
        {"7 Distort",  "7 扭曲"},
        {"8 Art",  "8 艺术"},
        {"9 Effect",  "9 特效1"},
        {"A Effect2",  "A 特效2"},
        {"B Black & White",  "B 黑白"},
        {"C Other",  "C 其它"},
    };

    if (Settings.language() == "zh_CN")
    {
        if (filterTypeMap.contains(strFilterType))
        {
            strFilterType = filterTypeMap.value(strFilterType);
        }
    }

    return strFilterType;
}

QString FilterController::getFilterType(QString filterType)
{
    const QMap<QString, QString> filterTypeMap =
    {
//        {"Common", "常用"},
//        {"Distortion", "扭曲"},
//        {"Art", "艺术"},
//        {"Color Adjustment", "调色"},
//        {"Transform", "变换"},
//        {"Black & White", "黑 & 白"},
//        {"Time Dimension Effect", "时间维度特效"},
//        {"Color Extraction", "色彩提取"},
//        {"Blur", "模糊"},
//        {"Color Depth", "颜色深度"},
//        {"Material", "材质"},
//        {"Other", "其它"},
//        {"Effect 2", "特效 2"},
//        {"Effect", "特效"},
//        {"Crop", "裁剪"},

        {"1 Basic Processing", "常用"},
        {"2 Text", "文字"},
        {"3 Basic Coloring Tool",  "基础调色"},
        {"4 Denoise and Blur", "降噪和模糊"},
        {"5 Advanced Coloring Tool",  "高级调色"},
        {"6 Light",  "光效"},
        {"7 Distort",  "扭曲"},
        {"8 Art",  "艺术"},
        {"9 Effect",  "特效1"},
        {"A Effect2",  "特效2"},
        {"B Black & White",  "黑白"},
        {"C Other",  "其它"},

    };

//    QLocale ql;
//    if(ql.language() == QLocale::Chinese)
    QString strLanguage = Settings.language();
    if((strLanguage == "zh") || (strLanguage == "zh_CN"))
    {
        QMap<QString, QString>::const_iterator iter;
        for (iter = filterTypeMap.constBegin(); iter != filterTypeMap.constEnd(); ++iter )
        {
            if(iter.key() == filterType)
                filterType = iter.value();
        }
    }

    return filterType;
}
QmlMetadata* FilterController::getQmlMetadata(int nIndex)
{
    if(nIndex < m_metadataModel.rowCount())
    {
        QmlMetadata *pMeta = m_metadataModel.get(nIndex);
        return pMeta;
    }
    else
    {
        return nullptr;
    }

}

QList<FilterInfo> FilterController::getAudioFiltersInfo()
{
    return getFiltersInfo(1);
}

QList<FilterInfo> FilterController::getVideoFiltersInfo()
{
    return getFiltersInfo(0);
}

QList<FilterInfo> FilterController::getFiltersInfo(int nFilterType)
{
    qDebug()<<"sll---------getFiltersInfo---start";

    QList<FilterInfo> videoFiltersInfo = QList<FilterInfo>();

    for (int nIndex = 0; nIndex < m_metadataModel.rowCount(); nIndex++)
    {
        QmlMetadata *pMetadata = m_metadataModel.get(nIndex);
        Q_ASSERT(pMetadata);
        if (pMetadata == nullptr)
        {
            continue;
        }

        FilterInfo filterInfo = {};

        filterInfo.strName              = pMetadata->name();
        filterInfo.strClassification    = getFilterClassNameZH(pMetadata->filterType());

        QString strTempPath                    = pMetadata->thumbnail();
        filterInfo.strThumbnailFilePath     = strTempPath.right(strTempPath.length() - 3);
        filterInfo.nIndexOfMetadataModel    = nIndex;

        QString strObjectName = pMetadata->objectName();
        QDir commonFileDir = QDir(Util::resourcesPath() + "/template/filters/preview/");
        filterInfo.strPerviewSettingFilePath = commonFileDir.absoluteFilePath(strObjectName + ".mlt");

        bool bVisible = true;
        if (pMetadata->isHidden()) bVisible = false;
        if (pMetadata->needsGPU() && !Settings.playerGPU()) bVisible = false;
        if (!pMetadata->needsGPU() && Settings.playerGPU() && !pMetadata->gpuAlt().isEmpty()) bVisible = false;

        filterInfo.bVisible = bVisible;

        if (nFilterType == 0)//视频滤镜
        {
            if ((filterInfo.bVisible == true) && (filterInfo.strClassification != ""))
            {
                videoFiltersInfo.append(filterInfo);
            }
        }
        else if (nFilterType == 1)//音频滤镜
        {
            if ((filterInfo.bVisible == true) && (filterInfo.strClassification == ""))
            {
                videoFiltersInfo.append(filterInfo);
            }
        }

    }

    qDebug()<<"sll---------getFiltersInfo---end";

    return videoFiltersInfo;
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
    //only load filter ui when selected the timeline item
    if (!producer)
        return;
    if (producer && !producer->get_int(kMultitrackItemProperty))
        return;

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
    if (attachedIndex >= m_attachedModel.rowCount()) {
       return;
    }

    Q_ASSERT(attachedIndex < m_attachedModel.rowCount());

    if (attachedIndex == m_currentFilterIndex) {
        return;
    }
    m_currentFilterIndex = attachedIndex;

    QmlMetadata* meta = nullptr;
    if(m_currentFilterIndex >= 0)
        meta = m_attachedModel.getMetadata(m_currentFilterIndex);
    QmlFilter* filter = nullptr;
    if (meta) {
        Mlt::Filter* mltFilter = m_attachedModel.getFilter(m_currentFilterIndex);
        Q_ASSERT(mltFilter);
        filter = new QmlFilter(mltFilter, meta);
        Q_ASSERT(filter);

    }

    emit currentFilterAboutToChange(m_currentFilterIndex);
    emit currentFilterChanged(filter, meta, m_currentFilterIndex);
    m_currentFilter.reset(filter);
}

void FilterController::refreshCurrentFilter(int filterIndex)
{
    //setCurrentFilter(filterIndex);
    if(m_currentFilterIndex == -1) return;

    QmlMetadata* meta = m_attachedModel.getMetadata(m_currentFilterIndex);
    Q_ASSERT(meta);

    emit currentFilterChanged(m_currentFilter.data(), meta, m_currentFilterIndex);
}

void FilterController::refreshKeyFrame(Mlt::Filter *filter, const QVector<key_frame_item> &listKeyFrame)
{
    if(m_currentFilterIndex == -1)          return;

    QmlFilter *qmlFilter = m_currentFilter.data();
    if(!qmlFilter)                          return;

    Mlt::Filter* mltFilter = qmlFilter->getMltFilter();
    Q_ASSERT(mltFilter);
    if(mltFilter->get_filter() != filter->get_filter())     return;

    qmlFilter->refreshKeyFrame(listKeyFrame);
}

void FilterController::refreshNoAnimation(Mlt::Filter *filter, const QVector<key_frame_item> &listKeyFrame)
{
    if(m_currentFilterIndex == -1)          return;

    QmlFilter *qmlFilter = m_currentFilter.data();
    if(!qmlFilter)                          return;

    Mlt::Filter* mltFilter = qmlFilter->getMltFilter();
    Q_ASSERT(mltFilter);
    if(mltFilter->get_filter() != filter->get_filter())     return;

    int nKeyNumber = qmlFilter->cache_getKeyFrameNumber();
    if (nKeyNumber <= 1)  //if (nKeyNumber <= 0) //考虑mlt底层会在0的位置自动加上关键帧
        qmlFilter->refreshNoAnimation(listKeyFrame, true);
}

void FilterController::insertKeyFrame(Mlt::Filter *filter, const QVector<key_frame_item> &listKeyFrame)
{
    if(m_currentFilterIndex == -1)          return;

    QmlFilter *qmlFilter = m_currentFilter.data();
    if(!qmlFilter)                          return;

    Mlt::Filter* mltFilter = qmlFilter->getMltFilter();
    Q_ASSERT(mltFilter);
    if(mltFilter->get_filter() != filter->get_filter())     return;

    for(int nIndex = 0; nIndex < listKeyFrame.count(); nIndex++)
    {
        key_frame_item para = listKeyFrame.at(nIndex);

        QMap<QString, QString>::Iterator iter = para.paraMap.begin();
        while(iter != para.paraMap.end())
        {
            qmlFilter->cache_setKeyFrameParaValue(para.keyFrame, iter.key(), iter.value(), true);
            iter++;
        }
    }
    qmlFilter->syncCacheToProject();
}

void FilterController::removeKeyFrame(Mlt::Filter *filter, const QVector<key_frame_item> &listKeyFrame)
{
    if(m_currentFilterIndex == -1)          return;

    QmlFilter *qmlFilter = m_currentFilter.data();
    if(!qmlFilter)                          return;

    Mlt::Filter* mltFilter = qmlFilter->getMltFilter();
    Q_ASSERT(mltFilter);
    if(mltFilter->get_filter() != filter->get_filter())     return;

    for(int nIndex = 0; nIndex < listKeyFrame.count(); nIndex++)
    {
        key_frame_item para = listKeyFrame.at(nIndex);

        QMap<QString, QString>::Iterator iter = para.paraMap.begin();
        while(iter != para.paraMap.end())
        {
            qmlFilter->removeAnimationKeyFrame(para.keyFrame, iter.key(), true);
            iter++;
        }
    }
    qmlFilter->syncCacheToProject();
}

void FilterController::updateKeyFrame(Mlt::Filter *filter, int nFrame, QString name, QString value)
{
    if(m_currentFilterIndex == -1)          return;

    QmlFilter *qmlFilter = m_currentFilter.data();
    if(!qmlFilter)                          return;

    Mlt::Filter* mltFilter = qmlFilter->getMltFilter();
    Q_ASSERT(mltFilter);
    if(mltFilter->get_filter() != filter->get_filter())     return;

    qmlFilter->cache_setKeyFrameParaValue(nFrame, name, value, true);
    qmlFilter->syncCacheToProject();
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
    Q_ASSERT(first >= 0);
    Q_ASSERT(first < m_attachedModel.rowCount());

    m_currentFilterIndex = first;
    Mlt::Filter* mltFilter = m_attachedModel.getFilter(m_currentFilterIndex);
    QmlMetadata* meta = m_attachedModel.getMetadata(m_currentFilterIndex);
    Q_ASSERT(mltFilter);
    Q_ASSERT(meta);
    QmlFilter* filter = new QmlFilter(mltFilter, meta);
    Q_ASSERT(filter);
    filter->setIsNew(true);
    emit currentFilterAboutToChange(m_currentFilterIndex);
    emit currentFilterChanged(filter, meta, m_currentFilterIndex);
    m_currentFilter.reset(filter);
}

void FilterController::handleAttachDuplicateFailed(int index)
{
    Q_ASSERT(index >= 0);
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

    QmlMetadata *meta = metadataForUniqueId("audioGainVolume");
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
    Q_ASSERT(uniqueId);

    QmlMetadata* meta = nullptr;
    int rowCount = m_metadataModel.rowCount();
    QString qstrUniqueId(uniqueId);

    for (int i = 0; i < rowCount; i++) {
        QmlMetadata* tmpMeta = m_metadataModel.get(i);
        Q_ASSERT(tmpMeta);
        if (tmpMeta->uniqueId() == qstrUniqueId) {
            meta = tmpMeta;
            break;
        }
    }

    return meta;
}

void FilterController::addFilter(int nFilterIndex)
{
//    Q_ASSERT(nFilterIndex >= 0);

    int nCurrentFilter = nFilterIndex;
    if(nCurrentFilter == -1)
    {
//        nCurrentFilter = getCurrentSelectedFilterIndex();此函数得到的是一个恒值0
        nCurrentFilter = 0;
    }

    QmlMetadata *meta = m_metadataModel.get(nCurrentFilter);

    if(!meta->isAudio())
        MAIN.onShowPropertiesVideoFilterDock();
    else
        MAIN.onShowPropertiesAudioFilterDock();

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
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < m_attachedModel.rowCount());

    m_attachedModel.remove(row);
}

