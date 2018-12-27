/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
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
    , m_path(m_metadata->path().absolutePath().append('/'))
    , m_isNew(false)
{
}

QmlFilter::~QmlFilter()
{
    delete m_filter;
}

#ifdef MOVIEMATOR_PRO
Mlt::Animation QmlFilter::getAnimation(const QString& name)
{
    if (m_filter) {
        const char* propertyName = name.toUtf8().constData();
        if (!m_filter->get_animation(propertyName)) {
            // Cause a string property to be interpreted as animated value.
            m_filter->anim_get_double(propertyName, 0, 0);
        }
        return m_filter->get_animation(propertyName);
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

int QmlFilter::getKeyFrameOnProjectOnIndex(int index, QString name)
{
    if (m_filter)
    {
        int nKeyFrame = getAnimation(name).key_get_frame(index);

        return MAIN.timelineDock()->getPositionOnClip(nKeyFrame);
    }

    return -1;
}

void QmlFilter::removeAnimationKeyFrame(int nFrame, QString name)
{
    if (m_filter)
    {
        getAnimation(name).remove(nFrame);
    }
}

float QmlFilter::getKeyValueOnProjectOnIndex(int index, QString name)
{
    if (m_filter)
    {
        int nKeyFrame = getAnimation(name).key_get_frame(index);
        if (nKeyFrame >= 0)
        {
            float fKeyFrameValue = m_filter->anim_get_double(name.toUtf8().constData(), nKeyFrame, 0);
            return fKeyFrameValue;
        }
        else
            return 0;
    }
    else
        return 0;
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

#endif
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

QRectF QmlFilter::getRect(QString name, int position)
{
    if (!m_filter->is_valid()) return QRectF();
    const char* s = m_filter->get(name.toUtf8().constData());
    if (s) {
        const char* propertyName = name.toUtf8().constData();
        mlt_rect rect;
        if (position < 0) {
            rect = m_filter->get_rect(propertyName);
        } else {
//            rect = m_filter->anim_get_rect(propertyName, position, duration());
        }
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

//FIXME: 暫時之應用與Text Filter，之後其他濾鏡獲取rect方法統一后移除
QRectF QmlFilter::getRectOfTextFilter(QString name, int position)
{
    if (!m_filter->is_valid()) return QRectF();

    const char* propertyName = name.toUtf8().constData();
    mlt_rect rect;
    if (position < 0) {
        rect = m_filter->get_rect(propertyName);
    } else {
        rect = m_filter->anim_get_rect(propertyName, position);
    }

    return QRectF(rect.x, rect.y, rect.w, rect.h);
}

void QmlFilter::set(QString name, QString value)
{
    if (!m_filter) return;
    if (qstrcmp(m_filter->get(name.toUtf8().constData()), value.toUtf8().constData()))
    { //xjp add anim
        QString from_value = m_filter->get(name.toUtf8().constData());

        m_filter->set(name.toUtf8().constData(), value.toUtf8().constData());
         if(from_value != "" && from_value != value)
            MAIN.undoStack()->push(new Timeline::FilterCommand(m_filter, name,  from_value, value));

        MLT.refreshConsumer();
        emit changed();
    }
}

void QmlFilter::set(QString name, double value)
{
    if (!m_filter) return;
    if (!m_filter->get(name.toUtf8().constData())  //xjp add anim
        || m_filter->get_double(name.toUtf8().constData()) != value) {

        double from_value = m_filter->get_double(name.toUtf8().constData());
        m_filter->set(name.toUtf8().constData(), value);

        if(from_value != value)
            MAIN.undoStack()->push(new Timeline::FilterCommand(m_filter, name,  from_value, value));

        MLT.refreshConsumer();
        emit changed();
    }
}

void QmlFilter::set(QString name, int value)
{
    if (!m_filter) return;
    if (!m_filter->get(name.toUtf8().constData())
        || m_filter->get_int(name.toUtf8().constData()) != value) {

        int from_value = m_filter->get_int(name.toUtf8().constData());
        m_filter->set(name.toUtf8().constData(), value);
         if(from_value != value)
            MAIN.undoStack()->push(new Timeline::FilterCommand(m_filter, name,  from_value, value));

        MLT.refreshConsumer();
        emit changed();
    }
}

bool isValidRect(QRectF &rect)
{
    if(rect.isValid() || rect.isEmpty() || rect.isNull())
        return false;

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
    if (!m_filter) return;
    mlt_rect rect = m_filter->get_rect(name.toUtf8().constData());
    if (!m_filter->get(name.toUtf8().constData()) || x != rect.x || y != rect.y
        || width != rect.w || height != rect.h || opacity != rect.o) {

        QRectF rect_from(rect.x, rect.y, rect.w, rect.h);
        QRectF rect_to(x, y, width, height);

        m_filter->set(name.toUtf8().constData(), x, y, width, height, opacity);
        if(rect_from != rect_to && isValidRect(rect_from))
            MAIN.undoStack()->push(new Timeline::FilterCommand(m_filter, name,  rect_from, rect_to));

        MLT.refreshConsumer();
        emit changed();
    }
}

void QmlFilter::set(QString name, const QRectF& rect, double opacity, int position, mlt_keyframe_type keyframeType)
{
    set(name, rect.x(), rect.y(), rect.width(), rect.height(), opacity, position, keyframeType);
}


void QmlFilter::anim_set(QString name, QString value)
{
    if (!m_filter) return;
//    if (!m_filter->get_data("parameters_animations"))
//    {
//        mlt_properties paraAnimations = mlt_properties_new();
//        m_filter->set("parameters_animations", paraAnimations, 0, (mlt_destructor)mlt_properties_close);

//    }

//    mlt_properties para_animations = (mlt_properties)m_filter->get_data("parameters_animations");
//    if (para_animations)
//    {
//        if (qstrcmp(mlt_properties_get(para_animations, name.toUtf8().constData()), value.toUtf8().constData()))
//        {
//            mlt_properties_set(para_animations, name.toUtf8().constData(), value.toUtf8().constData());
//            MLT.refreshConsumer();
//            emit changed();
//        }
//    }
//   m_filter->set(name, value);
    QString anim_name = "anim-"+name;
    qDebug()<<"anim_set, key:"<<anim_name<<", value:"<<value;
    m_filter->set(anim_name.toUtf8().constData(),value.toUtf8().constData());
    MLT.refreshConsumer();
    emit changed();
}

void QmlFilter::resetProperty(const QString& name)
{
    m_filter->clear(name.toUtf8().constData());
}

void QmlFilter::loadPresets()
{
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
    Mlt::Properties properties;
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());

    properties.pass_list(*((Mlt::Properties*)m_filter), propertyNames.join('\t').toLatin1().constData());

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
    Mlt::Service service(mlt_service(m_filter->get_data("service")));

    // get temp filename for input xml
    QTemporaryFile tmp;

    tmp.open();
    tmp.close();

    m_filter->set("results", NULL, 0);
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
    // Every attached filter has a service property that points to the service to
    // which it is attached.
    Mlt::Producer producer(mlt_producer(m_filter->get_data("service")));
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
    // Every attached filter has a service property that points to the service to
    // which it is attached.
    Mlt::Producer producer(mlt_producer(m_filter->get_data("service")));
    if (producer.get(kFilterOutProperty))
        // Shots on the timeline will set the producer to the cut parent.
        // However, we want time-based filters such as fade in/out to use
        // the cut's in/out and not the parent's.
        return producer.get_int(kFilterOutProperty);
    else
        return producer.get_out();
}

double QmlFilter::producerAspect() const
{
    // Every attached filter has a service property that points to the service to
    // which it is attached.
    Mlt::Producer producer(mlt_producer(m_filter->get_data("service")));
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

void QmlFilter::preset(const QString &name)
{
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());

    if (!dir.cd("presets") || !dir.cd(objectNameOrService()))
        return;
    m_filter->load(dir.filePath(name).toUtf8().constData());
    MLT.refreshConsumer();
    emit changed();
}

QString QmlFilter::objectNameOrService()
{
    return m_metadata->objectName().isEmpty()? m_metadata->mlt_service() : m_metadata->objectName();
}

#ifdef MOVIEMATOR_PRO

double QmlFilter::getPreKeyFrameNum(double currentKeyFrame)
{
//    if(m_metadata)
//    {
//        return m_metadata->getPreKeyFrameNum(currentKeyFrame);
//    }

    int keyFrameCount = m_keyFrameList.count();
    bool bExist = false;
    if(keyFrameCount > 0)
    {
        key_frame_item firstPara = m_keyFrameList.at(0);
        key_frame_item lastPara = m_keyFrameList.at(keyFrameCount-1);

        if(firstPara.keyFrame == currentKeyFrame)//当前关键帧就是最小的关键帧，
            return -1;
        if(lastPara.keyFrame < currentKeyFrame)//当前关键帧就是最大的关键帧
            return m_keyFrameList.at(keyFrameCount-1).keyFrame;
        else if(lastPara.keyFrame == currentKeyFrame)
        {
            return m_keyFrameList.at(keyFrameCount-2).keyFrame;
        }

        for(int index=keyFrameCount-1; index >=0 ; index--)
        {
            key_frame_item para = m_keyFrameList.at(index);

            if(currentKeyFrame > para.keyFrame)//(currentKeyFrame > para.keyFrame) && (currentKeyFrame < para1.keyFrame))
            {
                bExist = true;
                return m_keyFrameList.at(index).keyFrame;
            }
        }


    }
    if(bExist == false)
    {
        return -1;
    }

}


double QmlFilter::getNextKeyFrameNum(double currentKeyFrame)
{

    int keyFrameCount = m_keyFrameList.count();
    if(keyFrameCount > 0)
    {
        for(int index = 0; index < keyFrameCount; index++)
        {
            key_frame_item para = m_keyFrameList.at(index);

            if(para.keyFrame > currentKeyFrame)//(currentKeyFrame > para.keyFrame) && (currentKeyFrame < para1.keyFrame))
            {
                return para.keyFrame;
            }
        }
    }

    return -1;
}

void QmlFilter::setKeyFrameParaRectValue(double frame, QString key, const QRectF& rect, double opacity)
{
    QString sValue = QString("%1 %2 %3 %4 %5").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height()).arg(opacity);
    setKeyFrameParaValue(frame, key, sValue);
}

void QmlFilter::setKeyFrameParaValue(double frame, QString key, QString value)
{
        QVector<key_frame_item> keyFrameFrom(m_keyFrameList);
       int keyFrameCount = m_keyFrameList.count();

       bool hasSameKeyFrame = false;

       if(keyFrameCount > 0)
       {
           int index =0;

           for(index=0; index < keyFrameCount; index++)
           {
               key_frame_item para = m_keyFrameList.at(index);


          //     key_frame_item para1 = m_keyFrameList.at(index++);
               if(para.keyFrame == frame)
               {
                   para.paraMap.insert(key, value);
                   m_keyFrameList.removeAt(index);//paraMap.insert(key, value);
                   m_keyFrameList.insert(index, para);
                   hasSameKeyFrame = true;
                    QVector<key_frame_item> keyFrameTo(m_keyFrameList);

                    MAIN.undoStack()->push(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));
                   return;
               //  return hasSameKeyFrame;

               }
            }

           //if there is no equal key frame in m_keyFrameList
           if(hasSameKeyFrame == false)
           {
               key_frame_item newItem ;
               newItem.keyFrame = frame;
               newItem.paraMap.insert(key, value);
              // m_keyFrameList.append(newItem);

               key_frame_item firstPara = m_keyFrameList.at(0);
               key_frame_item lastPara = m_keyFrameList.at(keyFrameCount-1);

               if(frame < firstPara.keyFrame)
               {
                   m_keyFrameList.insert(0, newItem);
                   hasSameKeyFrame = true;

                   emit addKeyFrame();
                   QVector<key_frame_item> keyFrameTo(m_keyFrameList);

                   MAIN.undoStack()->push(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));
                   emit keyframeNumberChanged();
                   return ;
               }
               if(frame > lastPara.keyFrame)
               {
                   m_keyFrameList.insert(keyFrameCount, newItem);
                   hasSameKeyFrame = true;

                   emit addKeyFrame();
                   QVector<key_frame_item> keyFrameTo(m_keyFrameList);

                   MAIN.undoStack()->push(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));
                   emit keyframeNumberChanged();
                   return ;
               }

               for(index=0; index < keyFrameCount-1; index++)
               {

                   key_frame_item para = m_keyFrameList.at(index);
                   key_frame_item para1;
                   if((index+1) < keyFrameCount)
                       para1 = m_keyFrameList.at(index+1);

                   if((frame > para.keyFrame) &&(frame < para1.keyFrame))
                   {
                       m_keyFrameList.insert(index+1, newItem);
                       hasSameKeyFrame = true;
                       emit addKeyFrame();
                       QVector<key_frame_item> keyFrameTo(m_keyFrameList);

                       MAIN.undoStack()->push(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));
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
           m_keyFrameList.append(newItem);
           hasSameKeyFrame = true;
       //    return true;
        //   emit addKeyFrame();

       //    emit addKeyFrame();
       }

       if(hasSameKeyFrame)
       {
           emit addKeyFrame();
           emit keyframeNumberChanged();
       }
     //  return false;
       //set all key frame value together

       QVector<key_frame_item> keyFrameTo(m_keyFrameList);

       MAIN.undoStack()->push(new Timeline::KeyFrameCommand(m_filter, keyFrameFrom, keyFrameTo));

}

void QmlFilter::removeAllKeyFrame()
{
    int paramCount = m_metadata->keyframes()->parameterCount();
    for (int i = 0; i < paramCount; i++)
    {
         QString name = m_metadata->keyframes()->parameter(i)->property();
         removeAllKeyFrame(name);
    }
}

void QmlFilter::removeAllKeyFrame(QString name)
{
    double keyFrameCount = this->getKeyFrameCountOnProject(name);
    for (int index = 0; index < keyFrameCount; index++) {
        double nFrame = this->getKeyFrameOnProjectOnIndex(index, name);
        this->removeKeyFrameParaValue(nFrame);
    }
}

void QmlFilter::removeKeyFrameParaValue(double frame)
{
//    if(m_metadata)
//    {
//        bool result = m_metadata->removeKeyFrameParaValue(frame);
//        if(result)
//            emit removeKeyFrame();
//    }
    int paramCount = m_metadata->keyframes()->parameterCount();
    for (int i = 0; i < paramCount; i++)
    {
         QString name = m_metadata->keyframes()->parameter(i)->property();

         Mlt::Animation animation = getAnimation(name);
         if (animation.is_valid())
             animation.remove(frame);
    }


    int keyFrameCount = m_keyFrameList.count();
    if(keyFrameCount==0)
        return ;


    for(int index=0; index < keyFrameCount; index++)
    {
        key_frame_item para = m_keyFrameList.at(index);
        if(para.keyFrame == frame)
        {
            m_keyFrameList.remove(index);
            emit removeKeyFrame();
            emit keyframeNumberChanged();
            if(keyFrameCount > 1)
                combineAllKeyFramePara();

            return;
        }
    }

 //   return false;

//    emit keyFrameChanged();


}

void QmlFilter::combineAllKeyFramePara()
{

    if(m_metadata)
    {
        int keyFrameCount = m_keyFrameList.count();
        if(keyFrameCount>0)
        {
            key_frame_item para1 = m_keyFrameList.at(0);
            int paraCount = para1.paraMap.keys().count();

//           QVector<QString> str(paraCount);
           for(int index=0; index<keyFrameCount; index++)
          {
               key_frame_item para = m_keyFrameList.at(index);
              // paraCount = para.paraMap.keys().count();
              for(int keyIndex=0; keyIndex<paraCount; keyIndex++)
              {

               int out = producerOut();
               int in = producerIn();

                QString key = para1.paraMap.keys().at(keyIndex);
                QString value = para.paraMap.values().at(keyIndex);


                QString paraType = "string";
                int paramCount = m_metadata->keyframes()->parameterCount();
                for (int i = 0; i < paramCount; i++)
                {
                     QString name = m_metadata->keyframes()->parameter(i)->property();
                     if (name == key) { paraType = m_metadata->keyframes()->parameter(i)->paraType(); break; }
                }

                // Only set an animation keyframe if it does not already exist with the same value.
                Mlt::Animation animation(m_filter->get_animation(key.toUtf8().constData()));
                if (paraType == "double")
                {
                    if (!animation.is_valid() || !animation.is_key(para.keyFrame)
                            || value.toDouble() != m_filter->anim_get_double(key.toUtf8().constData(), para.keyFrame, out - in + 1)) {
                        m_filter->anim_set(key.toUtf8().constData(), value.toDouble(), para.keyFrame, out - in + 1, mlt_keyframe_linear);
                        MLT.refreshConsumer();
                        emit changed();
                    }
                }
                else if(paraType == "int")
                {
                    if (!animation.is_valid() || !animation.is_key(para.keyFrame)
                            || value.toInt() != m_filter->anim_get_int(key.toUtf8().constData(), para.keyFrame, out - in + 1)) {
                        m_filter->anim_set(key.toUtf8().constData(), value.toInt(), para.keyFrame, out - in + 1, mlt_keyframe_linear);
                        MLT.refreshConsumer();
                        emit changed();
                    }
                }
                else if(paraType == "string")
                {
                    if (!animation.is_valid() || !animation.is_key(para.keyFrame) || value != m_filter->anim_get(key.toUtf8().constData(), para.keyFrame, out - in + 1))
                    {
                        m_filter->anim_set(key.toUtf8().constData(), value.toUtf8().constData(), para.keyFrame, out - in + 1);
                        MLT.refreshConsumer();
                        emit changed();
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

                    mlt_rect rect = m_filter->anim_get_rect(key.toUtf8().constData(), para.keyFrame, out - in + 1);
                    if (!animation.is_valid() || !animation.is_key(para.keyFrame)
                        || x != rect.x || y != rect.y || width != rect.w || height != rect.h || opacity != rect.o)
                    {
                        rect.x = x;
                        rect.y = y;
                        rect.w = width;
                        rect.h = height;
                        rect.o = opacity;
                        m_filter->anim_set(key.toUtf8().constData(), rect, para.keyFrame, out - in + 1, mlt_keyframe_smooth);
                        MLT.refreshConsumer();
                        emit changed();
                    }
                }
                else
                {
                    if (!animation.is_valid() || !animation.is_key(para.keyFrame) || value != m_filter->anim_get(key.toUtf8().constData(), para.keyFrame, out - in + 1))
                    {
                        m_filter->anim_set(key.toUtf8().constData(), value.toUtf8().constData(), para.keyFrame, out - in + 1);
                        MLT.refreshConsumer();
                        emit changed();
                    }
                }
             }
          }
        }

    }

}


bool QmlFilter::bKeyFrame(double frame)
{
    int keyFrameCount = m_keyFrameList.count();
    if(keyFrameCount == 0)
        return false;

    for(int index=0; index < keyFrameCount; index++)
    {
        key_frame_item para = m_keyFrameList.at(index);
        if(frame == para.keyFrame)
            return true;

    }

    return false;

}


bool QmlFilter::bHasPreKeyFrame(double frame)
{
//    if(m_metadata)
//         return m_metadata->bHasPreKeyFrame(frame);
//    else
//        return false;
    int keyFrameCount = m_keyFrameList.count();
    if(keyFrameCount == 0)
        return false;

    key_frame_item firstKeyFrame = m_keyFrameList.at(0);
    if(firstKeyFrame.keyFrame < frame)
        return true;
    else
        return false;

}

bool QmlFilter::bHasNextKeyFrame(double frame)
{
//    if(m_metadata)
//        return m_metadata->bHasNextKeyFrame(frame);
//    else
//        return false;
    int keyFrameCount = m_keyFrameList.count();
    if(keyFrameCount == 0)
        return false;

    key_frame_item lastKeyFrame = m_keyFrameList.at(keyFrameCount-1);
    if(lastKeyFrame.keyFrame > frame)
        return true;
    else
        return false;

}

double QmlFilter::getKeyFrameParaDoubleValue(double frame, QString key)
{
//    if(m_filter)
//    {
//        return m_filter->anim_get_double(key.toUtf8().constData(), frame);
//    }

//    return -1.0;

//    if(m_metadata)
//        return m_metadata->getKeyFrameParaDoubleValue(frame, key);
//    return -1.0;

    int keyFrameCount = m_keyFrameList.count();
    if(keyFrameCount == 0)
        return -1.0;
    else
    {
        for(int index=0; index < keyFrameCount; index++)
        {
            key_frame_item para = m_keyFrameList.at(index);
            if(frame == para.keyFrame)
                return para.paraMap.value(key).toDouble();

        }
    }
    return -1.0;

}

QRectF QmlFilter::getKeyFrameParaRectValue(double frame, QString key)
{
    int keyFrameCount = m_keyFrameList.count();
    if(keyFrameCount == 0) {
        return QRectF();
    } else {
        for(int index=0; index < keyFrameCount; index++) {
            key_frame_item para = m_keyFrameList.at(index);
            if(frame == para.keyFrame) {
                QString value = para.paraMap.value(key);
                QStringList listValue = value.split(" ", QString::SkipEmptyParts);
                double x = listValue[0].toDouble();
                double y = listValue[1].toDouble();
                double width = listValue[2].toDouble();
                double height = listValue[3].toDouble();

                return QRectF(x, y, width, height);
            }
        }
    }
    return QRectF();
}

QString QmlFilter::getKeyFrameParaValue(double frame, QString key)
{
//    if(m_metadata)
//        return m_metadata->getKeyFrameParaValue(frame, key);
//    else
//        return "";
    int keyFrameCount = m_keyFrameList.count();
    if(keyFrameCount == 0)
        return "";
    else
    {
        int keyLength = key.size();
        int keySize = key.length();
        QString subKey = key.right(keyLength-5);
        for(int index=0; index < keyFrameCount; index++)
        {
            key_frame_item para = m_keyFrameList.at(index);
            if(frame == para.keyFrame)
            {
                qDebug()<<"para.paraMap.value(key):"<<para.paraMap.value(subKey);
                return para.paraMap.value(subKey);
            }
        }
    }
    return "";

}

QString QmlFilter::getAnimStringValue(double frame, QString key)
{
    if(m_filter)
    {
        QString str = QString::fromUtf8(m_filter->anim_get(key.toUtf8().constData(), frame));
        return str;
    }

    return QString();
}

int QmlFilter::getAnimIntValue(double frame, QString key)
{
    if(m_filter)
    {
        return m_filter->anim_get_int(key.toUtf8().constData(), frame);
    }

    return -1;
}

double QmlFilter::getAnimDoubleValue(double frame, QString key)
{
    if(m_filter)
    {
        return m_filter->anim_get_double(key.toUtf8().constData(), frame);
    }

    return -1.0;
}

QRectF QmlFilter::getAnimRectValue(double frame, QString key)
{
    if (!m_filter) return QRectF();

    const char* s = m_filter->get(key.toUtf8().constData());
    if (s) {
        const char* propertyName = key.toUtf8().constData();
        mlt_rect rect;
//        if (frame < 0) {
//            rect = m_filter->get_rect(propertyName);
//        } else {
            rect = m_filter->anim_get_rect(propertyName, frame);
//        }
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


int QmlFilter::getKeyFrameNumber()
{
//    int paramCount = m_metadata->keyframes()->parameterCount();
//    if (paramCount > 0)
//    {
//         QString name = m_metadata->keyframes()->parameter(0)->property();

//         int nCount = getAnimation(name).key_count();
//         return nCount;
//    }

//    return 0;

    return m_keyFrameList.count();
}

int QmlFilter::getKeyFrame(int index)
{
    int keyFrameCount = m_keyFrameList.count();
    if(index < keyFrameCount)
        return m_keyFrameList.at(index).keyFrame;
    else
        return -1;

}

void QmlFilter::refreshKeyFrame(const QVector<key_frame_item> &listKeyFrame)
{
    removeAllKeyFrame();
    m_keyFrameList = listKeyFrame;
    combineAllKeyFramePara();
}

#endif

AnalyzeDelegate::AnalyzeDelegate(Mlt::Filter* filter)
    : QObject(0)
    , m_filter(*filter)
{}

void AnalyzeDelegate::onAnalyzeFinished(AbstractTask *job, bool isSuccess)
{
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




