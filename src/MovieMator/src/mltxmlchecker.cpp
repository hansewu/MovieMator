/*
 * Copyright (c) 2014-2016 Meltytech, LLC
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

#include "mltxmlchecker.h"
#include "mltcontroller.h"
#include "shotcut_mlt_properties.h"
#include "util.h"
#include <QLocale>
#include <QDir>
#include <QCoreApplication>
#include <QUrl>
#include <QRegExp>
#include <Logger.h>

#if defined (Q_OS_MAC)
    #include "securitybookmark/transport_security_bookmark.h"
#endif


// 没有使用到，消除警告
//static int alignWidth(int width)
//{
//    return (width + 7) / 8 * 8;
//}
static bool isMltClass(const QStringRef& name)
{
    return name == "profile" || name == "producer" ||
           name == "filter" || name == "playlist" ||
           name == "tractor" || name == "track" ||
           name == "transition" || name == "consumer";
}

static bool isNetworkResource(const QString& string)
{
    // Check if it looks like a qualified URL. Try parsing it and see.
    QRegExp schemaTest(QLatin1String("^[a-zA-Z]{2,}\\:.*"));
    // Not actually checking network URL due to latency and transience.
    return (schemaTest.exactMatch(string) && QUrl(string).isValid() && !string.startsWith("plain:"));
}


static bool isNumericProperty(const QString& name)
{
    return name == "length" || name == "geometry" || name == "rect";
}

MltXmlChecker::MltXmlChecker()
    : m_needsGPU(false)
    , m_hasEffects(false)
    , m_isCorrected(false)
    , m_decimalPoint(QLocale::system().decimalPoint())
    , m_tempFile(QDir::tempPath().append("/moviemator-XXXXXX.xml"))//xjp
    , m_hasComma(false)
    , m_hasPeriod(false)
    , m_numericValueChanged(false)
{
    LOG_DEBUG() << "decimal point" << m_decimalPoint;
    m_unlinkedFilesModel.setColumnCount(ColumnCount);
    m_isCorrected = false;

}

bool MltXmlChecker::check(const QString& fileName)
{
    LOG_DEBUG() << "begin";
#ifdef MOVIEMATOR_PRO
#ifndef SHARE_VERSION
    m_nUnbookmarkedFileCount = 0;

#endif
#endif
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text) &&
            m_tempFile.open()) {
        m_tempFile.resize(0);
        m_basePath = QFileInfo(fileName).canonicalPath();
        m_xml.setDevice(&file);
        m_newXml.setDevice(&m_tempFile);
        m_newXml.setAutoFormatting(true);
        m_newXml.setAutoFormattingIndent(2);
        if (m_xml.readNextStartElement()) {
            if (m_xml.name() == "mlt") {
                m_newXml.writeStartDocument();
                m_newXml.writeCharacters("\n");
                m_newXml.writeStartElement("mlt");
                foreach (QXmlStreamAttribute a, m_xml.attributes()) {
                    if (a.name().toString().toUpper() != "LC_NUMERIC")
                        m_newXml.writeAttribute(a);
                }
                readMlt();
                m_newXml.writeEndElement();
                m_newXml.writeEndDocument();
                m_isCorrected = m_isCorrected || (m_hasPeriod && m_hasComma && m_numericValueChanged);
            } else {
                m_xml.raiseError(QObject::tr("The file is not a MLT XML file."));
            }
        }
    }
    if (m_tempFile.isOpen()) {
        m_tempFile.close();
//        m_tempFile.open();
//        LOG_DEBUG() << m_tempFile.readAll().constData();
//        m_tempFile.close();
    }
    LOG_DEBUG() << "end";



    return m_xml.error() == QXmlStreamReader::NoError;
}

QString MltXmlChecker::errorString() const
{
    return m_xml.errorString();
}

void MltXmlChecker::readMlt()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "mlt");
    bool isPropertyElement = false;

    while (!m_xml.atEnd()) {
        switch (m_xml.readNext()) {
        case QXmlStreamReader::Characters:
            if (!isPropertyElement)
                m_newXml.writeCharacters(m_xml.text().toString());
            LOG_DEBUG()<<m_xml.text().toString();
            break;
        case QXmlStreamReader::Comment:
            m_newXml.writeComment(m_xml.text().toString());
            LOG_DEBUG()<<m_xml.text().toString();
            break;
        case QXmlStreamReader::DTD:
            m_newXml.writeDTD(m_xml.text().toString());
            LOG_DEBUG()<<m_xml.text().toString();
            break;
        case QXmlStreamReader::EntityReference:
            m_newXml.writeEntityReference(m_xml.name().toString());
            LOG_DEBUG()<<m_xml.text().toString();
            break;
        case QXmlStreamReader::ProcessingInstruction:
            m_newXml.writeProcessingInstruction(m_xml.processingInstructionTarget().toString(), m_xml.processingInstructionData().toString());
            LOG_DEBUG()<<m_xml.text().toString();
            break;
        case QXmlStreamReader::StartDocument:
            m_newXml.writeStartDocument(m_xml.documentVersion().toString(), m_xml.isStandaloneDocument());
            LOG_DEBUG()<<m_xml.text().toString();
            break;
        case QXmlStreamReader::EndDocument:
            m_newXml.writeEndDocument();
            LOG_DEBUG()<<m_xml.text().toString();
            break;
        case QXmlStreamReader::StartElement: {
            const QString element = m_xml.name().toString();
            LOG_DEBUG()<<m_xml.text().toString();
            if (element == "property") {
                const QString name = m_xml.attributes().value("name").toString();

                m_properties << MltProperty(name, m_xml.readElementText());
                isPropertyElement = true;
            } else {
                isPropertyElement = false;
                processProperties();
                m_newXml.writeStartElement(m_xml.namespaceUri().toString(), element);
                if (isMltClass(m_xml.name()))
                    mlt_class = element;
                checkInAndOutPoints(); // This also copies the attributes.
            }

            break;
        }
        case QXmlStreamReader::EndElement:
            LOG_DEBUG()<<m_xml.text().toString();
            if (m_xml.name() != "property") {
                processProperties();
                m_newXml.writeEndElement();
                if (isMltClass(m_xml.name())) {
                    mlt_class.clear();
                }
            }
            break;
        default:
            break;
        }
    }

}

void MltXmlChecker::processProperties()
{
    QString mlt_service;
    QVector<MltProperty> newProperties;
    m_resource.clear();

    // First pass: collect information about mlt_service and resource.
    foreach (MltProperty p, m_properties) {
        // Get the name of the MLT service.
        if (p.first == "mlt_service") {
            mlt_service = p.second;
        } else if (p.first == kShotcutHashProperty) {
            m_resource.hash = p.second;
        } else if (isNumericProperty(p.first)) {
            checkNumericString(p.second);
        } else if (readResourceProperty(p.first, p.second)) {
            fixUnlinkedFile(p.second);
        }
        newProperties << MltProperty(p.first, p.second);
    }

    if (mlt_class == "filter" || mlt_class == "transition" || mlt_class == "producer") {
        checkGpuEffects(mlt_service);
        checkUnlinkedFile(mlt_service);

        // Second pass: amend property values.
        m_properties = newProperties;
        newProperties.clear();
        foreach (MltProperty p, m_properties) {
            if (p.first == "resource" && mlt_service.contains("webvfx")) {
                fixWebVfxPath(p.second);

            // Fix some properties if re-linked file.
            } else if (p.first == kShotcutHashProperty) {
                if (!m_resource.newHash.isEmpty())
                    p.second = m_resource.newHash;
            } else if (p.first == kShotcutCaptionProperty) {
                if (!m_resource.newDetail.isEmpty())
                    p.second = Util::baseName(m_resource.newDetail);
            } else if (p.first == kShotcutDetailProperty) {
                if (!m_resource.newDetail.isEmpty())
                    p.second = m_resource.newDetail;
            } else if (p.first == "audio_index" || p.first == "video_index") {
                fixStreamIndex(p.second);
            }

            if (!p.second.isEmpty())
                newProperties << MltProperty(p.first, p.second);
        }
    }

    // Write all of the properties.
    foreach (MltProperty p, newProperties) {
        m_newXml.writeStartElement("property");
        m_newXml.writeAttribute("name", p.first);
        m_newXml.writeCharacters(p.second);
        m_newXml.writeEndElement();
    }
    m_properties.clear();
}

void MltXmlChecker::checkInAndOutPoints()
{
    Q_ASSERT(m_xml.isStartElement());

    // Fix numeric values of in and out point attributes.
    foreach (QXmlStreamAttribute a, m_xml.attributes()) {
        if (a.name() == "in" || a.name() == "out") {
            QString value = a.value().toString();
            if (checkNumericString(value)) {
                m_newXml.writeAttribute(a.name().toString(), value);
                continue;
            }
        }
        m_newXml.writeAttribute(a);
    }
}

bool MltXmlChecker::checkNumericString(QString& value)
{
    // See which delimiters are being used.
    if (!m_hasComma)
        m_hasComma = value.contains(',');
    if (!m_hasPeriod)
        m_hasPeriod = value.contains('.');

    // Determine if there is a decimal point inconsistent with locale.
    if (!value.contains(m_decimalPoint) &&
            (value.contains('.') || value.contains(','))) {
        value.replace(',', m_decimalPoint);
        value.replace('.', m_decimalPoint);
        m_numericValueChanged = true;
        return true;
    }
    return false;
}

bool MltXmlChecker::fixWebVfxPath(QString& resource)
{
    // The path, if absolute, should start with the Shotcut executable path.
    QFileInfo fi(resource);
    if (fi.isAbsolute()) {
        QDir appPath(QCoreApplication::applicationDirPath());

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
        // Leave the bin directory on Linux.
        appPath.cdUp();
#endif
        if (!resource.startsWith(appPath.path())) {
            // Locate "share/shotcut" and replace the front of it with appPath.
            int i = resource.indexOf("/share/MovieMator/");
            if (i >= 0) {
                resource.replace(0, i, appPath.path());
                m_isCorrected = true;
                return true;
            }
        }
    }
    return false;
}

static QString getPrefix(const QString& name, const QString& value)
{
    int length = 0;

    // Webvfx and timewarp are only using "resource".
    if (name == "resource") {
        const QString plain("plain:");
        // webvfx "plain:"
        if (value.startsWith(plain)) {
            return plain;
        } else {
            // timewarp speed parameter
            length = value.indexOf(':');
            if (length > 0 && (value[length - 1].isDigit() ||
                               value[length - 1] == '.' ||
                               value[length - 1] == ','))
                return value.left(length + 1); // include the colon
        }
    }
    return QString();
}

#ifdef MOVIEMATOR_PRO
  #ifndef SHARE_VERSION
//void MltXmlChecker::checkUnbookmarkedFiles(QString& filename)
//{
//    int strLen = filename.length();
//    if(strLen>4)
//    {
//        if(filename.at(strLen-4) == '.' || filename.at(strLen-5) == '.')
//        {
//            QString profile_name = "";
//            Mlt::Profile *tempProfile = new Mlt::Profile("atsc_720p_24");
//            Mlt::Profile tmp(profile_name.toLatin1().constData());
//            tempProfile->set_colorspace(tmp.colorspace());
//            tempProfile->set_frame_rate(tmp.frame_rate_num(), tmp.frame_rate_den());
//            tempProfile->set_height(tmp.height());
//            tempProfile->set_progressive(tmp.progressive());
//            tempProfile->set_sample_aspect(tmp.sample_aspect_num(), tmp.sample_aspect_den());
//            tempProfile->set_display_aspect(tmp.display_aspect_num(), tmp.display_aspect_den());
//            tempProfile->set_width(alignWidth(tmp.width()));
//            tempProfile->set_explicit(false);
//            Mlt::Producer *producerTemp = new Mlt::Producer(*tempProfile,  filename.toUtf8().constData());
//            if(!producerTemp->is_valid())
//            {
//                LOG_DEBUG()<<"Failed to bookmark file:"<<filename;
//               m_nUnbookmarkedFileCount++;
//            }


//            delete producerTemp;
//            producerTemp = NULL;

//          }
//     }
//}

bool MltXmlChecker::hasAccessPermission(const QString &filename)
{
    return has_access_permission(filename.toUtf8().constData());
}


#endif
#endif

bool MltXmlChecker::readResourceProperty(const QString& name, QString& value)
{
    if (mlt_class == "filter" || mlt_class == "transition" || mlt_class == "producer")
    if (name == "resource" || name == "src" || name == "filename"
            || name == "luma" || name == "luma.resource" || name == "composite.luma"
            || name == "producer.resource") {

        // Handle special prefix such as "plain:" or speed.
//#ifdef MOVIEMATOR_PRO
//#ifndef SHARE_VERSION
//        checkUnbookmarkedFiles(value);
//#endif
//#endif
        m_resource.prefix = getPrefix(name, value);
        // Save the resource name (minus prefix) for later check for unlinked files.
        m_resource.info.setFile(value.mid(m_resource.prefix.size()));
        if (!isNetworkResource(value) && m_resource.info.isRelative())
            m_resource.info.setFile(m_basePath, m_resource.info.filePath());
        return true;
    }
    return false;
}

void MltXmlChecker::checkGpuEffects(const QString& mlt_service)
{
    // Check for GPU effects.
    if (!MLT.isAudioFilter(mlt_service))
        m_hasEffects = true;
    if (mlt_service.startsWith("movit.") || mlt_service.startsWith("glsl."))
        m_needsGPU = true;
}

void MltXmlChecker::checkUnlinkedFile(const QString& mlt_service)
{
    // Check for an unlinked file.
    // not the color producer

    if (!mlt_service.isEmpty() && mlt_service != "color" && mlt_service != "colour")

    // not a builtin luma wipe file
    if (mlt_service != "luma" || !m_resource.info.baseName().startsWith('%'))

    // not a URL
    if (!m_resource.info.filePath().isEmpty() && !isNetworkResource(m_resource.info.filePath()))

    // file does not exist

    if (!m_resource.info.exists())
    // not already in the model
    {
    if (m_unlinkedFilesModel.findItems(m_resource.info.filePath(),
            Qt::MatchFixedString | Qt::MatchCaseSensitive).isEmpty()) {
        LOG_ERROR()<< "file not found: " << m_resource.info.filePath();

        QIcon icon(":/icons/oxygen/32x32/status/task-reject.png");
        QStandardItem* item = new QStandardItem(icon, m_resource.info.filePath());
        item->setToolTip(item->text());
        item->setData(m_resource.hash, ShotcutHashRole);
        m_unlinkedFilesModel.appendRow(item);
    }
    }
#ifdef MOVIEMATOR_PRO
#ifndef SHARE_VERSION
    else if (!hasAccessPermission(m_resource.info.filePath())) {
        m_nUnbookmarkedFileCount++;
    }
#endif
#endif
    LOG_DEBUG()<<"file name:"<<m_resource.info.filePath();

}

bool MltXmlChecker::fixUnlinkedFile(QString& value)
{
    // Replace unlinked files if model is populated with replacements.
    for (int row = 0; row < m_unlinkedFilesModel.rowCount(); ++row) {
        const QStandardItem* replacement = m_unlinkedFilesModel.item(row, ReplacementColumn);
        if (replacement && !replacement->text().isEmpty() &&
                m_unlinkedFilesModel.item(row, MissingColumn)->text() == m_resource.info.filePath()) {
            m_resource.info.setFile(replacement->text());
            m_resource.newDetail = replacement->text();
            m_resource.newHash = replacement->data(ShotcutHashRole).toString();
            // Restore special prefix such as "plain:" or speed value.
            value = replacement->text().prepend(m_resource.prefix);
            m_isCorrected = true;
            return true;
        }
    }
    return false;
}

void MltXmlChecker::fixStreamIndex(QString& value)
{
    // Remove a stream index property if re-linked file is different.
    if (!m_resource.hash.isEmpty() && !m_resource.newHash.isEmpty()
        && m_resource.hash != m_resource.newHash) {
        value.clear();
    }
}
