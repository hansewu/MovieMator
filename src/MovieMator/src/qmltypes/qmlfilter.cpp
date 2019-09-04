/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: Dragon-S <15919917852@163.com>
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

#include "qmlfilter.h"
#include "mltcontroller.h"
#include "mainwindow.h"
#include "controllers/filtercontroller.h"
#include "jobqueue.h"
#include "jobs/meltjob.h"
#include "shotcut_mlt_properties.h"
#include <QStandardPaths>
#include <QDir>
#include <QIODevice>
#include <QTemporaryFile>
#include <QFile>
#include <QtXml>
#include <MltProducer.h>
#include "docks/timelinedock.h"
#include "commands/timelinecommands.h"

#include "encodetaskqueue.h"
#include "jobs/melttask.h"

static const char* kWidthProperty = "meta.media.width";
static const char* kHeightProperty = "meta.media.height";
static const char* kAspectNumProperty = "meta.media.sample_aspect_num";
static const char* kAspectDenProperty = "meta.media.sample_aspect_den";

QmlFilter::QmlFilter(Mlt::Filter* mltFilter, /*const*/ QmlMetadata* metadata, QObject* parent)
    : QObject(parent)
    , m_metadata(metadata)
    , m_filter(mltFilter)
    , m_sResourcePath(m_metadata->path().absolutePath().append('/'))
    , m_isNew(false)
{
}

QmlFilter::~QmlFilter()
{
    delete m_filter;
}

//#ifdef MOVIEMATOR_PRO
Mlt::Animation QmlFilter::getAnimation(const QString& name)
{
    if (m_filter)
    {
        std::string strName         = name.toStdString();
        const char  *pPropertyName  = strName.c_str();

        if (!m_filter->get_animation(pPropertyName))
        {
            // Cause a string property to be interpreted as animated value.
            m_filter->anim_get_double(pPropertyName, 0, 0);
        }

        return m_filter->get_animation(pPropertyName);
    }
    return Mlt::Animation();
}

int QmlFilter::getKeyFrameCountOnProject(QString name)
{
    if (m_filter)
    {
        int count = getAnimation(name).key_count();
        return count;
    }
    else
        return 0;
}

//获取的是在当前clip中的位置信息，不是producer中的信息，split后，获取的是在split单个clip中的位置
int QmlFilter::getKeyFrameOnProjectOnIndex(int index, QString name)
{

    if (m_filter)
    {
        int nKeyFrame = getAnimation(name).key_get_frame(index);

        int nPositionInClip = MAIN.timelineDock()->getPositionOnClip(nKeyFrame);
        int nClipLength = MAIN.timelineDock()->getCurrentClipLength();
        if (nPositionInClip > (nClipLength - 1)) return -1;

        return nPositionInClip;
//        return MAIN.timelineDock()->getPositionOnClip(nKeyFrame);
    }

    return -1;
}

void QmlFilter::removeAnimationKeyFrame(int nFrame, QString name, bool bFromUndo)
{
    int nFrameInClip = nFrame;
    nFrame = MAIN.timelineDock()->getPositionOnParentProducer(nFrame);

    bool bKeyFrame = getAnimation(name).is_key(nFrame);
    if(!bKeyFrame)      return;

    if (m_filter)
    {
        if(!bFromUndo)
        {
            QString paraType = "string";
            int paramCount = m_metadata->keyframes()->parameterCount();
            for (int i = 0; i < paramCount; i++)
            {
                 QString key = m_metadata->keyframes()->parameter(i)->property();
                 if (name == key) { paraType = m_metadata->keyframes()->parameter(i)->paraType(); break; }
            }

            QString from_value = "";
            if (paraType == "double")
                from_value = QString::number(getAnimDoubleValue(nFrameInClip, name));
            else if(paraType == "int")
                from_value = QString::number(getAnimIntValue(nFrameInClip, name));
            else if(paraType == "string")
                from_value = getAnimStringValue(nFrameInClip, name);
            else if(paraType == "rect")
            {
                QRectF rect = getAnimRectValue(nFrameInClip, name);
                from_value = QString("%1 %2 %3 %4 %5").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height()).arg(1.0);
            }
            else
                from_value = getAnimStringValue(nFrameInClip, name);


            key_frame_item para;
            para.keyFrame = nFrameInClip;
            para.paraMap.insert(name, from_value);

            QVector<key_frame_item> keyFrameRemove;
            keyFrameRemove.insert(0, para);
            MAIN.pushCommand(new Timeline::KeyFrameRemoveCommand(*(MAIN.timelineDock()->model()), m_filter, keyFrameRemove,true));
        }

        getAnimation(name).remove(nFrame);
    }

    emit keyframeNumberChanged();
}

double QmlFilter::getKeyValueOnProjectOnIndex(int index, QString name)
{
    if (m_filter)
    {
        int nKeyFrame = getAnimation(name).key_get_frame(index);
        if (nKeyFrame >= 0)
        {
            double fKeyFrameValue = m_filter->anim_get_double(name.toUtf8().constData(), nKeyFrame, 0);
            return fKeyFrameValue;
        }
        else
            return 0.0;
    }
    else
        return 0.0;
}

QString QmlFilter::getStringKeyValueOnProjectOnIndex(int index, QString name)
{
    if (m_filter)
    {
        QString str = QString::fromUtf8(m_filter->get(name.toUtf8().constData()));
        if(str.count())
        {
            QStringList subList = str.split(";", QString::SkipEmptyParts);
            QString str1 = subList.at(index);
            QStringList strKeyFrameList = str1.split("=", QString::SkipEmptyParts);
            QString  strKeyFrameValue = strKeyFrameList.at(1);
            return strKeyFrameValue;
        }
        else
            return "";
    }
    else
        return "";
}

//#endif
QString QmlFilter::get(QString name)
{
    if (m_filter)
    {
        QString str = QString::fromUtf8(m_filter->get(name.toUtf8().constData()));
        return str;
    }
    else
        return QString();
}

double QmlFilter::getDouble(QString name)
{
    if (m_filter)
        return m_filter->get_double(name.toUtf8().constData());
    else
        return 0;
}

int QmlFilter::getInt(QString name)
{
    if (m_filter)
        return m_filter->get_int(name.toUtf8().constData());
    else
        return 0;
}

QRectF QmlFilter::getRect(QString name, int position)  //
{
    if (position != -1)
        position = MAIN.timelineDock()->getPositionOnParentProducer(position);

    if (!m_filter->is_valid())
        return QRectF();

    QByteArray byteArrayName    = name.toUtf8();
    const char *pPropertyName   = byteArrayName.constData();
    const char *pPropertyValue  = m_filter->get(pPropertyName);

    if (pPropertyValue)
    {
        mlt_rect rect;
        if (position < 0)
        {
            rect = m_filter->get_rect(pPropertyName);
        }

        if (::strchr(pPropertyValue, '%'))
        {
            return QRectF(qRound(rect.x * MLT.profile().width()),
                          qRound(rect.y * MLT.profile().height()),
                          qRound(rect.w * MLT.profile().width()),
                          qRound(rect.h * MLT.profile().height()));
        }
        else
        {
            return QRectF(rect.x, rect.y, rect.w, rect.h);
        }
    }

    return QRectF();
}

//FIXME: 暫時之應用與Text Filter，之後其他濾鏡獲取rect方法統一后移除
QRectF QmlFilter::getRectOfTextFilter(QString name, int position)
{
    if (position != -1)
        position = MAIN.timelineDock()->getPositionOnParentProducer(position);
    if (!m_filter->is_valid())
        return QRectF();

    QByteArray byteArrayName    = name.toUtf8();
    const char *pPropertyName   = byteArrayName.constData();
    mlt_rect rect;

    if (position < 0)
    {
        rect = m_filter->get_rect(pPropertyName);
    }
    else
    {
        rect = m_filter->anim_get_rect(pPropertyName, position);
    }

    return QRectF(rect.x, rect.y, rect.w, rect.h);
}

void QmlFilter::set(QString name, QString value)
{
    if (!m_filter) return;
    if (qstrcmp(m_filter->get(name.toUtf8().constData()), value.toUtf8().constData()))
    { //xjp add anim
        QString fromValue = m_filter->get(name.toUtf8().constData());

        m_filter->set(name.toUtf8().constData(), value.toUtf8().constData());
         if(fromValue != "" && fromValue != value)
            MAIN.pushCommand(new Timeline::FilterCommand(*(MAIN.timelineDock()->model()), *(MAIN.filterController()->attachedModel()), MAIN.filterController()->currentFilterIndex(), name, fromValue, value,true));

        MLT.refreshConsumer();
        emit filterPropertyValueChanged();
    }
}

void QmlFilter::set(QString name, double value)
{
    if (!m_filter) return;
    if (!m_filter->get(name.toUtf8().constData())  //xjp add anim
        || !qFuzzyCompare(m_filter->get_double(name.toUtf8().constData()),value)) {

        double fromValue = m_filter->get_double(name.toUtf8().constData());
        m_filter->set(name.toUtf8().constData(), value);

        if(!qFuzzyCompare(fromValue,value))
            MAIN.pushCommand(new Timeline::FilterCommand(*(MAIN.timelineDock()->model()), *(MAIN.filterController()->attachedModel()), MAIN.filterController()->currentFilterIndex(), name,  fromValue, value,true));

        MLT.refreshConsumer();
        emit filterPropertyValueChanged();
    }
}

void QmlFilter::set(QString name, int value)
{
    if (!m_filter) return;
    if (!m_filter->get(name.toUtf8().constData())
        || m_filter->get_int(name.toUtf8().constData()) != value) {

        int fromValue = m_filter->get_int(name.toUtf8().constData());
        m_filter->set(name.toUtf8().constData(), value);
         if(fromValue != value)
            MAIN.pushCommand(new Timeline::FilterCommand(*(MAIN.timelineDock()->model()), *(MAIN.filterController()->attachedModel()), MAIN.filterController()->currentFilterIndex(), name,  fromValue, value,true));

        MLT.refreshConsumer();
        emit filterPropertyValueChanged();
    }
}

bool isValidRect(QRectF &rect)
{
    if(!rect.isValid() || rect.isEmpty() || rect.isNull()){
        return false;
    }

    if(rect.left() < -10000.0 && rect.left() > 10000.0)
        return false;
    if(rect.top() < -10000.0 && rect.top() > 10000.0)
        return false;

    if(rect.width()> 10000.0)
        return false;

    if(rect.height()> 10000.0)
        return false;

    return true;

}

void QmlFilter::set(QString name, double x, double y, double width, double height, double opacity,
                    int position, mlt_keyframe_type keyframeType)
{
    Q_UNUSED(position);
    Q_UNUSED(keyframeType);

    if (!m_filter) return;
    mlt_rect rect = m_filter->get_rect(name.toUtf8().constData());
    if (!m_filter->get(name.toUtf8().constData()) || !qFuzzyCompare(x,rect.x) || !qFuzzyCompare(y,rect.y)
        || !qFuzzyCompare(width,rect.w) || !qFuzzyCompare(height,rect.h) || !qFuzzyCompare(opacity,rect.o))
    {

        QRectF rectFrom(rect.x, rect.y, rect.w, rect.h);
        QRectF rectTo(x, y, width, height);

        m_filter->set(name.toUtf8().constData(), x, y, width, height, opacity);

        if((rectFrom != rectTo) && isValidRect(rectFrom))
        {
            MAIN.pushCommand(new Timeline::FilterCommand(*(MAIN.timelineDock()->model()), *(MAIN.filterController()->attachedModel()), MAIN.filterController()->currentFilterIndex(), name,  rectFrom, rectTo,true));
        }
        MLT.refreshConsumer();
        emit filterPropertyValueChanged();
    }
}

void QmlFilter::set(QString name, const QRectF& rect, double opacity, int position, mlt_keyframe_type keyframeType)
{
    set(name, rect.x(), rect.y(), rect.width(), rect.height(), opacity, position, keyframeType);
}


void QmlFilter::anim_set(QString name, QString value)
{
    if (!m_filter) return;

    QString anim_name = "anim-"+name;
    qDebug()<<"anim_set, key:"<<anim_name<<", value:"<<value;
    m_filter->set(anim_name.toUtf8().constData(),value.toUtf8().constData());
    MLT.refreshConsumer();
    emit filterPropertyValueChanged();
}

void QmlFilter::resetProperty(const QString& name)
{
    if (!m_filter) return;
    m_filter->clear(name.toUtf8().constData());
}

void QmlFilter::loadPresets()
{
//    Q_ASSERT(m_presets);
    m_presets.clear();
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
    if (dir.cd("presets")) {
        QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Executable);
        foreach (QString s, entries) {
            if (s == objectNameOrService() && dir.cd(s)) {
                m_presets.append(" ");
                m_presets.append(dir.entryList(QDir::Files | QDir::Readable));
                break;
            }
        }
    }
    emit presetsChanged();
}

int QmlFilter::savePreset(const QStringList &propertyNames, const QString &name)
{
    Q_ASSERT(m_filter);
    Mlt::Properties properties;
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());

    properties.pass_list(*(static_cast<Mlt::Properties*>(m_filter)), propertyNames.join('\t').toLatin1().constData());

    if (!dir.exists())
        dir.mkpath(dir.path());
    if (!dir.cd("presets")) {
        if (dir.mkdir("presets"))
            dir.cd("presets");
    }
    if (!dir.cd(objectNameOrService())) {
        if (dir.mkdir(objectNameOrService()))
            dir.cd(objectNameOrService());
    }
    const QString preset = name.isEmpty()? tr("(defaults)") : name;
    properties.save(dir.filePath(preset).toUtf8().constData());
    loadPresets();
    return m_presets.indexOf(name);
}

void QmlFilter::deletePreset(const QString &name)
{
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
    if (dir.cd("presets") && dir.cd(objectNameOrService()))
        QFile(dir.filePath(name)).remove();
    m_presets.removeOne(name);
    emit presetsChanged();
}

void QmlFilter::analyze(bool isAudio)
{
    Q_ASSERT(m_filter);

    Mlt::Service service(mlt_service(m_filter->get_data("service")));
    Q_ASSERT(service.is_valid());
    // get temp filename for input xml
    QTemporaryFile tmp;

    tmp.open();
    tmp.close();

    m_filter->set("results", nullptr, 0);
    int disable = m_filter->get_int("disable");
    m_filter->set("disable", 0);
    MLT.saveXML(tmp.fileName(), &service, false);
    m_filter->set("disable", disable);

    // get temp filename for output xml
    QTemporaryFile tmpTarget;
    tmpTarget.open();
    tmpTarget.close();


    // parse xml
    QFile f1(tmp.fileName());
    f1.open(QIODevice::ReadOnly);
    QDomDocument dom(tmp.fileName());
    dom.setContent(&f1);
    f1.close();

    // add consumer element
    QDomElement consumerNode = dom.createElement("consumer");
    QDomNodeList profiles = dom.elementsByTagName("profile");
    if (profiles.isEmpty())
        dom.documentElement().insertAfter(consumerNode, dom.documentElement());
    else
        dom.documentElement().insertAfter(consumerNode, profiles.at(profiles.length() - 1));
    consumerNode.setAttribute("mlt_service", "xml");
    consumerNode.setAttribute("all", 1);
    if (isAudio)
        consumerNode.setAttribute("video_off", 1);
    else
        consumerNode.setAttribute("audio_off", 1);
    consumerNode.setAttribute("no_meta", 1);
    consumerNode.setAttribute("resource", tmpTarget.fileName());

//    AbstractJob* job = new MeltJob(tmpTarget.fileName(), dom.toString(2));
//    if (job) {
//        AnalyzeDelegate* delegate = new AnalyzeDelegate(m_filter);
//        connect(job, &AbstractJob::finished, delegate, &AnalyzeDelegate::onAnalyzeFinished);
//        connect(job, &AbstractJob::finished, this, &QmlFilter::analyzeFinished);
//        QFileInfo info(QString::fromUtf8(service.get("resource")));
//        job->setLabel(tr("Analyze %1").arg(info.fileName()));
//        JOBS.add(job);
//    }

    AbstractTask* task = new MeltTask(tmpTarget.fileName(), dom.toString(2));
    if (task) {
        AnalyzeDelegate* delegate = new AnalyzeDelegate(m_filter);
        Q_ASSERT(delegate);
        connect(task, &AbstractTask::finished, delegate, &AnalyzeDelegate::onAnalyzeFinished);
        connect(task, &AbstractTask::finished, this, &QmlFilter::analyzeFinished);
        QFileInfo info(QString::fromUtf8(service.get("resource")));
        task->setLabel(tr("Analyze %1").arg(info.fileName()));
        ENCODETASKS.addTask(task);
    }
}

int QmlFilter::framesFromTime(const QString &time)
{
    if (MLT.producer()) {
        return MLT.producer()->time_to_frames(time.toLatin1().constData());
    }
    return 0;
}

QString QmlFilter::timeFromFrames(int frames)
{
    if (MLT.producer()) {
        return MLT.producer()->frames_to_time(frames);
    }
    return QString();
}

void QmlFilter::getHash()
{
    MLT.getHash(*m_filter);
}

int QmlFilter::producerIn() const
{
    Q_ASSERT(m_filter);
    // Every attached filter has a service property that points to the service to
    // which it is attached.
    Mlt::Producer producer(mlt_producer(m_filter->get_data("service")));
    Q_ASSERT(producer.is_valid());
    if (producer.get(kFilterInProperty))
        // Shots on the timeline will set the producer to the cut parent.
        // However, we want time-based filters such as fade in/out to use
        // the cut's in/out and not the parent's.
        return producer.get_int(kFilterInProperty);
    else
        return producer.get_in();
}

int QmlFilter::producerOut() const
{
    Q_ASSERT(m_filter);
    Mlt::Producer producer(mlt_producer(m_filter->get_data("service")));
    Q_ASSERT(producer.is_valid());
    if (producer.get(kFilterOutProperty))
        // Shots on the timeline will set the producer to the cut parent.
        // However, we want time-based filters such as fade in/out to use
        // the cut's in/out and not the parent's.
        return producer.get_int(kFilterOutProperty);
    else
        return producer.get_out();
}

void QmlFilter::setInAndOut(int in, int out)
{
    if (out < in) {
        return;
    }
    Q_ASSERT(m_filter);
    if (m_filter->get_int("in") != in || m_filter->get_int("out") != out)
        m_filter->set_in_and_out(in, out);
}

double QmlFilter::producerAspect() const
{
    Q_ASSERT(m_filter);
    // Every attached filter has a service property that points to the service to
    // which it is attached.
    Mlt::Producer producer(mlt_producer(m_filter->get_data("service")));
    Q_ASSERT(producer.is_valid());
    if (producer.get(kHeightProperty)) {
        double sar = 1.0;
        if (producer.get(kAspectDenProperty)) {
            sar = producer.get_double(kAspectNumProperty) /
                  producer.get_double(kAspectDenProperty);
        }
        return sar * producer.get_double(kWidthProperty) / producer.get_double(kHeightProperty);
    }
    return MLT.profile().dar();
}

double QmlFilter::mediaHeight() const
{
    Q_ASSERT(m_filter);
    // Every attached filter has a service property that points to the service to
    // which it is attached.
    Mlt::Producer producer(mlt_producer(m_filter->get_data("service")));
    Q_ASSERT(producer.is_valid());
    if (producer.get(kHeightProperty))
        return QString(producer.get(kHeightProperty)).toDouble();

    return -1;
}

double QmlFilter::mediaWidth() const
{
    Q_ASSERT(m_filter);
    // Every attached filter has a service property that points to the service to
    // which it is attached.
    Mlt::Producer producer(mlt_producer(m_filter->get_data("service")));
    Q_ASSERT(producer.is_valid());
    if (producer.get(kWidthProperty))
        return QString(producer.get(kWidthProperty)).toDouble();

    return -1;
}

void QmlFilter::preset(const QString &name)
{
    Q_ASSERT(m_filter);
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());

    if (!dir.cd("presets") || !dir.cd(objectNameOrService()))
        return;
    m_filter->load(dir.filePath(name).toUtf8().constData());
    MLT.refreshConsumer();
    emit filterPropertyValueChanged();
}

QString QmlFilter::objectNameOrService()
{
    Q_ASSERT(m_metadata);
    return m_metadata->objectName().isEmpty()? m_metadata->mlt_service() : m_metadata->objectName();
}

//#ifdef MOVIEMATOR_PRO

int QmlFilter::cache_getPreKeyFrameNumInParent(int currentKeyFrame, QString propertyName)
{
    Q_ASSERT(MAIN.timelineDock());
    currentKeyFrame = MAIN.timelineDock()->getPositionOnParentProducer(currentKeyFrame);

    if (propertyName == nullptr) propertyName = getAnyAnimPropertyName();
    if (propertyName == nullptr) return -1;

    if (getAnimation(propertyName).is_key(currentKeyFrame)) currentKeyFrame = currentKeyFrame - 1;

    int nKeyFrame = getAnimation(propertyName).previous_key(currentKeyFrame);

    return nKeyFrame;





//    Q_ASSERT(MAIN.timelineDock());
//    currentKeyFrame = MAIN.timelineDock()->getPositionOnParentProducer(currentKeyFrame);
//    int keyFrameCount = m_cacheKeyFrameList.count();
//    bool bExist = false;
//    if(keyFrameCount > 0)
//    {
//        key_frame_item firstPara = m_cacheKeyFrameList.at(0);
//        key_frame_item lastPara = m_cacheKeyFrameList.at(keyFrameCount-1);

//        if(firstPara.keyFrame == currentKeyFrame)//当前关键帧就是最小的关键帧，
//            return -1;
//        if(lastPara.keyFrame < currentKeyFrame)//当前关键帧就是最大的关键帧
//            return m_cacheKeyFrameList.at(keyFrameCount-1).keyFrame;
//        else if(lastPara.keyFrame == currentKeyFrame)
//        {
//            return m_cacheKeyFrameList.at(keyFrameCount-2).keyFrame;
//        }

//        for(int index=keyFrameCount-1; index >=0 ; index--)
//        {
//            key_frame_item para = m_cacheKeyFrameList.at(index);

//            if(currentKeyFrame > para.keyFrame)//(currentKeyFrame > para.keyFrame) && (currentKeyFrame < para1.keyFrame))
//            {
//                bExist = true;
//                return m_cacheKeyFrameList.at(index).keyFrame;
//            }
//        }


//    }
//    if(bExist == false)
//    {
//        return -1;
//    }

//    return -1.0;
}




int QmlFilter::cache_getPreKeyFrameNum(int currentKeyFrame, QString propertyName)
{
    Q_ASSERT(MAIN.timelineDock());

    int nFrameInParent = cache_getPreKeyFrameNumInParent(currentKeyFrame, propertyName);

    int nPositionInClip = MAIN.timelineDock()->getPositionOnClip(nFrameInParent);

    if (nPositionInClip < 0) nPositionInClip = -1;

    int nClipLength = MAIN.timelineDock()->getCurrentClipLength();
    if (nPositionInClip > (nClipLength - 1)) nPositionInClip = -1;

    return nPositionInClip;
}


int QmlFilter::cache_getNextKeyFrameNum(int currentKeyFrame, QString propertyName)
{
    Q_ASSERT(MAIN.timelineDock());
    currentKeyFrame = MAIN.timelineDock()->getPositionOnParentProducer(currentKeyFrame);

    if (propertyName == nullptr) propertyName = getAnyAnimPropertyName();
    if (propertyName == nullptr) return -1;

    if (getAnimation(propertyName).is_key(currentKeyFrame)) currentKeyFrame = currentKeyFrame + 1;

    int nKeyFrame = getAnimation(propertyName).next_key(currentKeyFrame);

    int nPositionInClip = MAIN.timelineDock()->getPositionOnClip(nKeyFrame);
    int nClipLength     = MAIN.timelineDock()->getCurrentClipLength();
    if (nPositionInClip > (nClipLength - 1)) nPositionInClip = -1;

    return nPositionInClip;


//    int keyFrameCount = m_cacheKeyFrameList.count();
//    if(keyFrameCount > 0)
//    {
//        for(int index = 0; index < keyFrameCount; index++)
//        {
//            key_frame_item para = m_cacheKeyFrameList.at(index);

//            if(para.keyFrame > currentKeyFrame)//(currentKeyFrame > para.keyFrame) && (currentKeyFrame < para1.keyFrame))
//            {
//                int nFrameInParent = para.keyFrame;

//                int nPositionInClip = MAIN.timelineDock()->getPositionOnClip(nFrameInParent);
//                int nClipLength = MAIN.timelineDock()->getCurrentClipLength();
//                if (nPositionInClip > (nClipLength - 1)) nPositionInClip = -1;

//                return nPositionInClip;
////                return para.keyFrame;
//            }
//        }
//    }

//    return -1;
}

void QmlFilter::cache_setKeyFrameParaRectValue(int frame, QString key, const QRectF& rect, double opacity)
{
    QString sValue = QString("%1 %2 %3 %4 %5").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height()).arg(opacity);
    cache_setKeyFrameParaValue(frame, key, sValue);
}

void QmlFilter::cache_setKeyFrameParaValue(int frame, QString key, QString value, bool bFromUndo)
{
    QString from_value  = "";
    int nFrameInClip    = frame;
    bool bKeyFrame      = cache_bKeyFrame(frame, key);

    if(frame < 0) return;
    frame = MAIN.timelineDock()->getPositionOnParentProducer(frame);


    Q_ASSERT(m_filter);
    if(m_metadata)
    {
        QString paraType = "string";
        int paramCount = m_metadata->keyframes()->parameterCount();
        for (int i = 0; i < paramCount; i++)
        {
             QString name = m_metadata->keyframes()->parameter(i)->property();
             if (name == key) { paraType = m_metadata->keyframes()->parameter(i)->paraType(); break; }
        }
        int duration = m_filter->get_length();

                // Only set an animation keyframe if it does not already exist with the same value.
                Mlt::Animation animation(m_filter->get_animation(key.toUtf8().constData()));
                if (paraType == "double")
                {
                    if (!animation.is_valid() || !animation.is_key(frame)
                            || !qFuzzyCompare(value.toDouble(),m_filter->anim_get_double(key.toUtf8().constData(), frame, duration)))
                    {
                        from_value = QString::number(m_filter->anim_get_double(key.toUtf8().constData(), frame, duration));
                        m_filter->anim_set(key.toUtf8().constData(), value.toDouble(), frame, duration, mlt_keyframe_linear);
                    }
                }
                else if(paraType == "int")
                {
                    if (!animation.is_valid() || !animation.is_key(frame)
                            || value.toInt() != m_filter->anim_get_int(key.toUtf8().constData(), frame, duration)) {
                        from_value = QString::number(m_filter->anim_get_int(key.toUtf8().constData(), frame, duration));
                        m_filter->anim_set(key.toUtf8().constData(), value.toInt(), frame, duration, mlt_keyframe_linear);
                    }
                }
                else if(paraType == "string")
                {
                    if (!animation.is_valid() || !animation.is_key(frame) || value != m_filter->anim_get(key.toUtf8().constData(), frame, duration))
                    {
                        from_value = QString::fromUtf8(m_filter->anim_get(key.toUtf8().constData(), frame, duration));

                        m_filter->anim_set(key.toUtf8().constData(), value.toUtf8().constData(), frame, duration);
                    }
                }
                else if(paraType == "rect")
                {
                    QStringList listValue = value.split(" ", QString::SkipEmptyParts);
                    double x = listValue[0].toDouble();
                    double y = listValue[1].toDouble();
                    double width = listValue[2].toDouble();
                    double height = listValue[3].toDouble();
                    double opacity = listValue[4].toDouble();

                    mlt_rect rect = m_filter->anim_get_rect(key.toUtf8().constData(), frame, duration);
                    if (!animation.is_valid() || !animation.is_key(frame)
                        || !qFuzzyCompare(x,rect.x) || !qFuzzyCompare(y,rect.y) || !qFuzzyCompare(width,rect.w) || !qFuzzyCompare(height,rect.h) || !qFuzzyCompare(opacity,rect.o))
                    {
                        from_value = QString("%1 %2 %3 %4 %5").arg(rect.x).arg(rect.y).arg(rect.w).arg(rect.h).arg(rect.o);
                        rect.x = x;
                        rect.y = y;
                        rect.w = width;
                        rect.h = height;
                        rect.o = opacity;

                        m_filter->anim_set(key.toUtf8().constData(), rect, frame, duration); //, mlt_keyframe_smooth); mlt_keyframe_smooth 可能有问题，在颜色显示时很明显  https://github.com/hansewu/MovieMator/issues/271
                    }
                }
                else
                {
                    if (!animation.is_valid() || !animation.is_key(frame) || value != m_filter->anim_get(key.toUtf8().constData(), frame, duration))
                    {
                        from_value = QString::fromUtf8(m_filter->anim_get(key.toUtf8().constData(), frame, duration));
                        m_filter->anim_set(key.toUtf8().constData(), value.toUtf8().constData(), frame, duration);
                    }
                }


       if(!bFromUndo && (from_value != ""))
       {
            if(bKeyFrame)   //修改关键帧数据
                MAIN.pushCommand(new Timeline::KeyFrameUpdateCommand(*(MAIN.timelineDock()->model()), m_filter, nFrameInClip, key, from_value, value,true));
            else            //增加关键帧
            {
                key_frame_item paraFrom;
                paraFrom.keyFrame = nFrameInClip;//-1;
                paraFrom.paraMap.insert(key, from_value);

                QVector<key_frame_item> keyFrameFrom;
                keyFrameFrom.insert(0, paraFrom);


                key_frame_item para;
                para.keyFrame = nFrameInClip;
                para.paraMap.insert(key, value);

                QVector<key_frame_item> keyFrameAdd;
                keyFrameAdd.insert(0, para);

                MAIN.pushCommand(new Timeline::KeyFrameInsertCommand(*(MAIN.timelineDock()->model()), m_filter, keyFrameFrom, keyFrameAdd,true));
            }
       }

    }
    emit keyframeNumberChanged();


//QVector<key_frame_item> keyFrameTo(m_cacheKeyFrameList);

//MAIN.pushCommand(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));

//           MLT.refreshConsumer();
//           emit filterPropertyValueChanged();













/*
    if(frame < 0) return;
    frame = MAIN.timelineDock()->getPositionOnParentProducer(frame);


        QVector<key_frame_item> keyFrameFrom(m_cacheKeyFrameList);
       int keyFrameCount = m_cacheKeyFrameList.count();

       bool hasSameKeyFrame = false;

       if(keyFrameCount > 0)
       {
           int index =0;

           for(index=0; index < keyFrameCount; index++)
           {
               key_frame_item para = m_cacheKeyFrameList.at(index);


          //     key_frame_item para1 = m_cacheKeyFrameList.at(index++);
               if(para.keyFrame == frame)
               {
                   para.paraMap.insert(key, value);
                   m_cacheKeyFrameList.removeAt(index);//paraMap.insert(key, value);
                   m_cacheKeyFrameList.insert(index, para);
                   hasSameKeyFrame = true;
                    QVector<key_frame_item> keyFrameTo(m_cacheKeyFrameList);

                    MAIN.pushCommand(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));
                   return;
               //  return hasSameKeyFrame;

               }
            }

           //if there is no equal key frame in m_cacheKeyFrameList
           if(hasSameKeyFrame == false)
           {
               key_frame_item newItem ;
               newItem.keyFrame = frame;
               newItem.paraMap.insert(key, value);
              // m_cacheKeyFrameList.append(newItem);

               key_frame_item firstPara = m_cacheKeyFrameList.at(0);
               key_frame_item lastPara = m_cacheKeyFrameList.at(keyFrameCount-1);

               if(frame < firstPara.keyFrame)
               {
                   m_cacheKeyFrameList.insert(0, newItem);
                   hasSameKeyFrame = true;

                   emit cache_addedKeyFrame();
                   QVector<key_frame_item> keyFrameTo(m_cacheKeyFrameList);

                   MAIN.pushCommand(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));
                   emit keyframeNumberChanged();
                   return ;
               }
               if(frame > lastPara.keyFrame)
               {
                   m_cacheKeyFrameList.insert(keyFrameCount, newItem);
                   hasSameKeyFrame = true;

                   emit cache_addedKeyFrame();
                   QVector<key_frame_item> keyFrameTo(m_cacheKeyFrameList);

                   MAIN.pushCommand(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));
                   emit keyframeNumberChanged();
                   return ;
               }

               for(index=0; index < keyFrameCount-1; index++)
               {

                   key_frame_item para = m_cacheKeyFrameList.at(index);
                   key_frame_item para1;
                   if((index+1) < keyFrameCount)
                       para1 = m_cacheKeyFrameList.at(index+1);

                   if((frame > para.keyFrame) &&(frame < para1.keyFrame))
                   {
                       m_cacheKeyFrameList.insert(index+1, newItem);
                       hasSameKeyFrame = true;
                       emit cache_addedKeyFrame();
                       QVector<key_frame_item> keyFrameTo(m_cacheKeyFrameList);

                       MAIN.pushCommand(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));
                       emit keyframeNumberChanged();
                       return ;
                   }

               }


           }
       }
       else
       {
           key_frame_item newItem ;
           newItem.keyFrame = frame;
           newItem.paraMap.insert(key, value);
           m_cacheKeyFrameList.append(newItem);
           hasSameKeyFrame = true;
       //    return true;
        //   emit cache_addedKeyFrame();

       //    emit cache_addedKeyFrame();
       }

       if(hasSameKeyFrame)
       {
           emit cache_addedKeyFrame();
           emit keyframeNumberChanged();
       }
     //  return false;
       //set all key frame value together

       QVector<key_frame_item> keyFrameTo(m_cacheKeyFrameList);

       MAIN.pushCommand(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));
*/
}

void QmlFilter::removeAllKeyFrame(bool bFromUndo)
{
    Q_ASSERT(m_metadata);
    Q_ASSERT(m_metadata->keyframes());

    int paramCount = m_metadata->keyframes()->parameterCount();
    for (int i = 0; i < paramCount; i++)
    {
         QString name = m_metadata->keyframes()->parameter(i)->property();
         removeAllKeyFrame(name, bFromUndo);
    }
}

void QmlFilter::removeAllKeyFrame(QString name, bool bFromUndo)
{
    Mlt::Animation animation = getAnimation(name);
    if (!animation.is_valid())   return;

    int nKeyFrameCount = this->getKeyFrameCountOnProject(name);
    for (int index = nKeyFrameCount-1; index >= 0; index--)
    {
        int nFrame          = animation.key_get_frame(index);
        int nFrameInClip    = MAIN.timelineDock()->getPositionOnClip(nFrame);
        removeAnimationKeyFrame(nFrameInClip, name, bFromUndo);
//        animation.remove(nFrame);
    }

    emit keyframeNumberChanged();

    MLT.refreshConsumer();
    emit filterPropertyValueChanged();
}

void QmlFilter::removeKeyFrameParaValue(int frame, bool bFromUndo)
{
    Q_ASSERT(m_metadata);
    Q_ASSERT(m_metadata->keyframes());

    int paramCount = m_metadata->keyframes()->parameterCount();
    for (int i = 0; i < paramCount; i++)
    {
         QString name = m_metadata->keyframes()->parameter(i)->property();
         removeAnimationKeyFrame(frame, name, bFromUndo);
    }

    emit keyframeNumberChanged();

    MLT.refreshConsumer();
    emit filterPropertyValueChanged();

    return;
}

void QmlFilter::syncCacheToProject()
{
    MLT.refreshConsumer();
    emit filterPropertyValueChanged();
}


bool QmlFilter::cache_bKeyFrame(int frame, QString propertyName)
{
    Q_ASSERT(MAIN.timelineDock());
    frame = MAIN.timelineDock()->getPositionOnParentProducer(frame);

    if (propertyName == nullptr) propertyName = getAnyAnimPropertyName();
    if (propertyName == nullptr) return false;

    return getAnimation(propertyName).is_key(frame);
}


bool QmlFilter::bHasPreKeyFrame(int frame)
{
    Q_ASSERT(MAIN.timelineDock());
    int frameInParent = MAIN.timelineDock()->getPositionOnParentProducer(frame);
    if (m_filter)
    {
        Q_ASSERT(m_metadata);
        int paramCount = m_metadata->keyframes()->parameterCount();
        if (paramCount <= 0) return false;
        QString name = m_metadata->keyframes()->parameter(0)->property();

        if (getAnimation(name).is_key(frameInParent)) frameInParent = frameInParent - 1;
        int nKeyFrame = getAnimation(name).previous_key(frameInParent);
        nKeyFrame = MAIN.timelineDock()->getPositionOnClip(nKeyFrame);

        if (nKeyFrame >= 0 && (nKeyFrame != frame)) return true;
    }

    return false;
}

bool QmlFilter::bHasNextKeyFrame(int frame)
{
    Q_ASSERT(MAIN.timelineDock());
    int frameInParent = MAIN.timelineDock()->getPositionOnParentProducer(frame);
    if (m_filter)
    {
        Q_ASSERT(m_metadata);
        int paramCount = m_metadata->keyframes()->parameterCount();
        if (paramCount <= 0) return false;
        QString name = m_metadata->keyframes()->parameter(0)->property();

        if (getAnimation(name).is_key(frameInParent)) frameInParent = frameInParent + 1;

        int nKeyFrame = getAnimation(name).next_key(frameInParent);
        nKeyFrame = MAIN.timelineDock()->getPositionOnClip(nKeyFrame);

        int nClipLength = MAIN.timelineDock()->getCurrentClipLength();
        if (nKeyFrame > (nClipLength - 1)) return false;

        if (nKeyFrame > frame) return true;
    }

    return false;
}

double QmlFilter::cache_getKeyFrameParaDoubleValue(int frame, QString key)
{
    Q_ASSERT(MAIN.timelineDock());
    frame = MAIN.timelineDock()->getPositionOnParentProducer(frame);
    if(m_filter)
    {
        return m_filter->anim_get_double(key.toUtf8().constData(), frame);
    }

    return -1.0;


}

QRectF QmlFilter::cache_getKeyFrameParaRectValue(int frame, QString name)
{
    if (!m_filter) return QRectF();

    const char* s = m_filter->get(name.toUtf8().constData());
    if (s) {
        Q_ASSERT(MAIN.timelineDock());
        frame = MAIN.timelineDock()->getPositionOnParentProducer(frame);
        mlt_rect rect = m_filter->anim_get_rect(name.toUtf8().constData(), frame);

        if (::strchr(s, '%')) {
            return QRectF(qRound(rect.x * MLT.profile().width()),
                          qRound(rect.y * MLT.profile().height()),
                          qRound(rect.w * MLT.profile().width()),
                          qRound(rect.h * MLT.profile().height()));
        } else {
            return QRectF(rect.x, rect.y, rect.w, rect.h);
        }
    } else {
        return QRectF(0.0, 0.0, 0.0, 0.0);
    }
}

QString QmlFilter::cache_getKeyFrameParaValue(int frame, QString key)
{
    if (!m_filter) return "";

    Q_ASSERT(MAIN.timelineDock());
    frame = MAIN.timelineDock()->getPositionOnParentProducer(frame);
    QString str = QString::fromUtf8(m_filter->anim_get(key.toUtf8().constData(), frame));
    return str;
}

QString QmlFilter::getAnimStringValue(int frame, QString key)
{
    frame = MAIN.timelineDock()->getPositionOnParentProducer(frame);
    if(m_filter)
    {
        QString str = QString::fromUtf8(m_filter->anim_get(key.toUtf8().constData(), frame));
        return str;
    }

    return QString();
}

int QmlFilter::getAnimIntValue(int frame, QString key)
{
    frame = MAIN.timelineDock()->getPositionOnParentProducer(frame);
    if(m_filter)
    {
        return m_filter->anim_get_int(key.toUtf8().constData(), frame);
    }

    return -1;
}

double QmlFilter::getAnimDoubleValue(int frame, QString key)
{
    frame = MAIN.timelineDock()->getPositionOnParentProducer(frame);
    if(m_filter)
    {
        return m_filter->anim_get_double(key.toUtf8().constData(), frame);
    }

    return -1.0;
}

QRectF QmlFilter::getAnimRectValue(int frame, QString key)
{
    frame = MAIN.timelineDock()->getPositionOnParentProducer(frame);
    if (!m_filter)
        return QRectF();

    QByteArray byteArrayName = key.toUtf8();
    const char *pPropertyName   = byteArrayName.constData();
    const char* pPropertyValue  = m_filter->get(pPropertyName);

    if (pPropertyValue)
    {
        mlt_rect rect;
        rect = m_filter->anim_get_rect(pPropertyName, frame);

        if (::strchr(pPropertyValue, '%'))
        {
            return QRectF(qRound(rect.x * MLT.profile().width()),
                          qRound(rect.y * MLT.profile().height()),
                          qRound(rect.w * MLT.profile().width()),
                          qRound(rect.h * MLT.profile().height()));
        }
        else
        {
            return QRectF(rect.x, rect.y, rect.w, rect.h);
        }
    }

    return QRectF();
}

QString QmlFilter::getAnyAnimPropertyName()
{
    Q_ASSERT(m_metadata);
    Q_ASSERT(m_metadata->keyframes());

    int nParamCount = m_metadata->keyframes()->parameterCount();
    if (nParamCount <= 0)           return nullptr;

    // 由于string没有关键帧，所以通过string类型的参数获取关键帧个数会出错，因此不能用string类型的参数去获取关键帧个数和关键帧位置
    int nParaIndex = 0;
    for(nParaIndex = 0; nParaIndex < nParamCount; nParaIndex++)
    {
        if(m_metadata->keyframes()->parameter(nParaIndex)->paraType() != "string")      break;
    }
    if (nParaIndex >= nParamCount) return nullptr;

    QString propertyName = m_metadata->keyframes()->parameter(nParaIndex)->property();

    return propertyName;
}


int QmlFilter::cache_getKeyFrameNumber(QString propertyName)
{
//    int nCount1 = m_cacheKeyFrameList.count();
//    return nCount1;

    if (propertyName == nullptr) propertyName = getAnyAnimPropertyName();
    if (propertyName == nullptr) return -2;

    int nCount = getKeyFrameCountOnProject(propertyName);
    if(nCount <= 0)
        nCount = getKeyFrameCountOnProject(propertyName);

    return nCount;
}

int QmlFilter::getKeyFrame(int index, QString propertyName)
{
    if (propertyName == nullptr) propertyName = getAnyAnimPropertyName();
    if (propertyName == nullptr) return -1;

    int nKeyFrame = getKeyFrameOnProjectOnIndex(index, propertyName);
    return nKeyFrame;
}

void QmlFilter::refreshKeyFrame(const QVector<key_frame_item> &listKeyFrame)
{
    Q_UNUSED(listKeyFrame)
}

void QmlFilter::refreshNoAnimation(const QVector<key_frame_item> &listParameter, bool bFromUndo)
{
    for(int nIndex = 0; nIndex < listParameter.count(); nIndex++)
    {
        key_frame_item para = listParameter.at(nIndex);
        QMap<QString, QString>::Iterator iter = para.paraMap.begin();
        while(iter != para.paraMap.end())
        {
            QString sPropertyName   = iter.key();
            QString sValue          = iter.value();

            QString paraType    = "string";
            int paramCount      = m_metadata->keyframes()->parameterCount();
            for (int i = 0; i < paramCount; i++)
            {
                 QString name = m_metadata->keyframes()->parameter(i)->property();
                 if (name == sPropertyName) { paraType = m_metadata->keyframes()->parameter(i)->paraType(); break; }
            }

            if (paraType == "double")
                m_filter->set(sPropertyName.toUtf8().constData(), sValue.toDouble());
            else if(paraType == "int")
                m_filter->set(sPropertyName.toUtf8().constData(), sValue.toInt());
            else if(paraType == "string")
                m_filter->set(sPropertyName.toUtf8().constData(), sValue.toUtf8().constData());
            else if(paraType == "rect")
            {
                QStringList listValue   = sValue.split(" ", QString::SkipEmptyParts);
                double x                = listValue[0].toDouble();
                double y                = listValue[1].toDouble();
                double width            = listValue[2].toDouble();
                double height           = listValue[3].toDouble();
                double opacity          = listValue[4].toDouble();

                m_filter->set(sPropertyName.toUtf8().constData(), x, y, width, height, opacity);
            }
            else
                m_filter->set(sPropertyName.toUtf8().constData(), sValue.toUtf8().constData());

            if(!bFromUndo)
            {}

            iter++;
        }
    }

    MLT.refreshConsumer();
    emit filterPropertyValueChanged();
    emit keyframeNumberChanged();
}

//#endif

AnalyzeDelegate::AnalyzeDelegate(Mlt::Filter* filter)
    : QObject(nullptr)
    , m_filter(*filter)
{}

void AnalyzeDelegate::onAnalyzeFinished(AbstractTask *job, bool isSuccess)
{
    Q_ASSERT(job);
    Q_ASSERT(m_filter.is_valid());
    QString fileName = job->objectName();

    if (isSuccess) {
        // parse the xml
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QDomDocument dom(fileName);
        dom.setContent(&file);
        file.close();

        QDomNodeList filters = dom.elementsByTagName("filter");
        for (int i = 0; i < filters.size(); i++) {
            QDomNode filterNode = filters.at(i);
            bool found = false;

            QDomNodeList properties = filterNode.toElement().elementsByTagName("property");
            for (int j = 0; j < properties.size(); j++) {
                QDomNode propertyNode = properties.at(j);
                if (propertyNode.attributes().namedItem("name").toAttr().value() == "mlt_service"
                        && propertyNode.toElement().text() == m_filter.get("mlt_service")) {
                    found = true;
                    break;
                }
            }
            if (found) {
                for (int j = 0; j < properties.size(); j++) {
                    QDomNode propertyNode = properties.at(j);
                    if (propertyNode.attributes().namedItem("name").toAttr().value() == "results") {
                        m_filter.set("results", propertyNode.toElement().text().toUtf8().constData());
                        emit MAIN.filterController()->attachedModel()->changed();
                    }
                }
                break;
            }
        }
    }
    QFile::remove(fileName);
    deleteLater();
}




