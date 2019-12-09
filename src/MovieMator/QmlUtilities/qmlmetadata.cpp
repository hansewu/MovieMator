/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
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

#include "qmlmetadata.h"
#include "settings.h"

QmlMetadata::QmlMetadata(QObject *parent)
    : QObject(parent)
    , m_type(Filter)
    , m_needsGPU(false)
    , m_qmlFileName("")
    , m_vuiFileName("")
    , m_isAudio(false)
    , m_isHidden(false)
    , m_isFavorite(false)
    , m_gpuAlt("")
    , m_allowMultiple(true)
    , m_isClipOnly(false)
    , m_needsProVersion(false)
    , m_freeVersion(false)
    , m_isGpuCompatible(true)
    , m_filterType("")
{
}

void QmlMetadata::loadSettings()
{
    //Override the default favorite setting if it has been set by the user.
    QString favorite = Settings.filterFavorite(uniqueId());
    if (favorite == "yes") {
        m_isFavorite = true;
    } else if (favorite == "no") {
        m_isFavorite = false;
    }
}

void QmlMetadata::setType(QmlMetadata::PluginType type)
{
    m_type = type;
}

void QmlMetadata::setName(const QString &name)
{
    m_name = name;
}

void QmlMetadata::set_mlt_service(const QString &service)
{
    m_mlt_service = service;
}

QString QmlMetadata::uniqueId() const
{
    if (!objectName().isEmpty()) {
        return objectName();
    } else {
        return mlt_service();
    }
}

void QmlMetadata::setNeedsGPU(bool needs)
{
    m_needsGPU = needs;
    emit changed();
}

void QmlMetadata::setQmlFileName(const QString &fileName)
{
    m_qmlFileName = fileName;
}

void QmlMetadata::setVuiFileName(const QString &fileName)
{
    m_vuiFileName = fileName;
}

void QmlMetadata::setPath(const QDir &path)
{
    m_path = path;
}

QUrl QmlMetadata::qmlFilePath() const
{
    QUrl retVal = QUrl();
    if (!m_qmlFileName.isEmpty()) {
        retVal = QUrl::fromLocalFile(m_path.absoluteFilePath(m_qmlFileName));
    }
    return retVal;
}

QUrl QmlMetadata::vuiFilePath() const
{
    QUrl retVal = QUrl();
    if (!m_vuiFileName.isEmpty()) {
        retVal = QUrl::fromLocalFile(m_path.absoluteFilePath(m_vuiFileName));
    }
    return retVal;
}

void QmlMetadata::setIsAudio(bool isAudio)
{
    m_isAudio = isAudio;
    emit changed();
}

void QmlMetadata::setIsHidden(bool isHidden)
{
    m_isHidden = isHidden;
    emit changed();
}

void QmlMetadata::setIsFavorite(bool isFavorite)
{
    m_isFavorite = isFavorite;

    if (!uniqueId().isEmpty()) {
        if (isFavorite) {
            Settings.setFilterFavorite(uniqueId(), "yes");
        } else {
            Settings.setFilterFavorite(uniqueId(), "no");
        }
    }
    emit changed();
}

void QmlMetadata::setGpuAlt(const QString& gpuAlt)
{
    m_gpuAlt = gpuAlt;
    emit changed();
}

void QmlMetadata::setAllowMultiple(bool allowMultiple)
{
    m_allowMultiple = allowMultiple;
}

void QmlMetadata::setIsClipOnly(bool isClipOnly)
{
    m_isClipOnly = isClipOnly;
}

void QmlMetadata::setThumbnail(const QString &thumbnail)
{
    m_thumbnail = thumbnail;
}

void QmlMetadata::setNeedsProVersion(bool needsProVersion)
{
    m_needsProVersion = needsProVersion;
}

void QmlMetadata::setFreeVersion(bool freeVersion)
{
    m_freeVersion = freeVersion;
}

void QmlMetadata::setFilterType(const QString &filterType)
{
    m_filterType = filterType;
}

QmlKeyframesMetadata::QmlKeyframesMetadata(QObject* parent)
    : QObject(parent)
    , m_allowTrim(true)
    , m_allowAnimateIn(false)
    , m_allowAnimateOut(false)
    , m_enabled(true)

{
}

QQmlListProperty<QmlKeyframesParameter> QmlKeyframesMetadata::parameters()
{
    return QQmlListProperty<QmlKeyframesParameter>(this, this, &QmlKeyframesMetadata::appendParameter,
                                                               &QmlKeyframesMetadata::paramCount,
                                                               &QmlKeyframesMetadata::paramAt,
                                                               &QmlKeyframesMetadata::clearParameter);
}

void QmlKeyframesMetadata::appendParameter(QmlKeyframesParameter *param)
{
    Q_ASSERT(param);

    m_parameters.append(param);
}

int QmlKeyframesMetadata::paramCount() const
{
    return m_parameters.count();
}

QmlKeyframesParameter *QmlKeyframesMetadata::paramAt(int idx) const
{
    Q_ASSERT(idx >= 0);
    Q_ASSERT(idx < m_parameters.count());

    return m_parameters.at(idx);
}

void QmlKeyframesMetadata::clearParameter()
{
    m_parameters.clear();
}

void QmlKeyframesMetadata::appendParameter(QQmlListProperty<QmlKeyframesParameter> *paramsList, QmlKeyframesParameter * param)
{
    Q_ASSERT(paramsList);
    Q_ASSERT(param);

    reinterpret_cast< QmlKeyframesMetadata* >(paramsList->data)->appendParameter(param);
}

int QmlKeyframesMetadata::paramCount(QQmlListProperty<QmlKeyframesParameter> *list)
{
    Q_ASSERT(list);

    return reinterpret_cast<QmlKeyframesMetadata *>(list->data)->paramCount();
}

QmlKeyframesParameter *QmlKeyframesMetadata::paramAt(QQmlListProperty<QmlKeyframesParameter> *list,int idx)
{
    Q_ASSERT(list);

    return reinterpret_cast<QmlKeyframesMetadata *>(list->data)->paramAt(idx);
}

void QmlKeyframesMetadata::clearParameter(QQmlListProperty<QmlKeyframesParameter> *list)
{
    Q_ASSERT(list);

    reinterpret_cast<QmlKeyframesMetadata *>(list->data)->clearParameter();
}

void QmlKeyframesMetadata::checkVersion(const QString& version)
{
    if (!m_minimumVersion.isEmpty()) {
        // m_enabled = version >= m_minimumVersion, but we need to compare each field of the version separately.
//        LOG_DEBUG() << "MLT version:" << version << "Shotcut minimumVersion:" << m_minimumVersion;
        QStringList versionParts = version.split('.');
        QStringList minimumVersionParts = m_minimumVersion.split('.');
        int i = 0;
        foreach (QString field, versionParts) {
            if (field.toUInt() < minimumVersionParts[i++].toUInt()) {
                setDisabled();
                break;
            }
        }
    }
}

void QmlKeyframesMetadata::setDisabled()
{
    m_enabled = m_allowAnimateIn = m_allowAnimateOut = false;
}

bool QmlKeyframesMetadata::supportAnimation()
{
    for (int i = 0; i < m_parameters.count(); i++)
    {
        QmlKeyframesParameter *pParameter = m_parameters.at(i);
        if (pParameter->supportAnimationFlag() == "1")
            return true;
    }
    return false;
}



QmlKeyframesParameter::QmlKeyframesParameter(QObject* parent)
    : QObject(parent)
    , m_isSimple(false)
    , m_isCurve(false)
    , m_minimum(0.0)
    , m_maximum(0.0)
    , m_paraType("string")
    , m_strSupportAnimationFlag("0")
{
}


