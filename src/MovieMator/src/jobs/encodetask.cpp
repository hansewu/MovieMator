/*
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
#include "encodetask.h"
#include <QDebug>
#include "registrationchecker.h"
#include "mltcontroller.h"
#include "mainwindow.h"


EncodeTask::EncodeTask(Mlt::Producer *producer, Mlt::Profile *profile, Mlt::Properties *presets, QString target) : AbstractTask(target)
    , m_consumer(nullptr)
    , m_producer(nullptr)
    , m_cut(nullptr)
    , m_duration(-1)
    , m_outputName(target)
    , m_textFilter(nullptr)
{
    Q_UNUSED(producer)
    //m_producer = new Mlt::Producer(producer);
    m_producer = new Mlt::Producer(*(MAIN.multitrack()));
    m_consumer = createConsumer(profile, presets, target);
    m_consumer->connect(*m_producer);
    m_duration = m_producer->get_length();

    connect(this, SIGNAL(endOfStream()), this, SLOT(onEndOfStream()));
}

EncodeTask::~EncodeTask()
{
    if (m_consumer)
        delete m_consumer;
    if (m_producer)
        delete m_producer;
    if (m_textFilter)
        delete m_textFilter;
}

void EncodeTask::start()
{
    AbstractTask::start();
    m_producer->seek(0);
    m_producer->set_speed(1.0);

#if SHARE_VERSION
#if MOVIEMATOR_PRO
    //加水印
    if (Registration.registrationType() == Registration_None)
    {
        m_textFilter = new Mlt::Filter(MLT.profile(), "dynamictext");
        m_textFilter->set("argument", "MovieMator Video Editor Pro\nwww.macvideostudio.com");
        m_textFilter->set("family", "Arial");
        m_textFilter->set("fgcolour", "#ffffffff");
        m_textFilter->set("bgcolour", "#00000000");
        m_textFilter->set("olcolour", "#ff000000");
        m_textFilter->set("outline", "1");

        m_textFilter->set("weight", 600);

        m_textFilter->set("geometry",   "25%/0%:50%x100%");
        m_textFilter->set("valign", "center");
        m_textFilter->set("halign", "center");
        m_producer->attach(*m_textFilter);
    }
#endif
#endif


#ifndef MOVIEMATOR_PRO
    int length = 5*60*MLT.profile().fps();
    if ( m_duration > length)
    {
    //m_producer->set_in_and_out(0, length);
        m_cut = m_producer->cut(0, length - 1);
        m_duration = length;
        m_consumer->connect(*m_cut);
    }
    //m_producer->set("length", length);
#endif


    int ret = m_consumer->start();
    if (ret != 0)
    {
        AbstractTask::setStopped(true);
        emit finished(this, false);

        resetProducer();

    }
}


void EncodeTask::stop()
{
    m_consumer->stop();
    AbstractTask::stop();
}


Mlt::FilteredConsumer *EncodeTask::createConsumer(Mlt::Profile *profile, Mlt::Properties *presets, QString target)
{
    Q_ASSERT(profile != nullptr);
    Q_ASSERT(presets != nullptr);
    Q_ASSERT(target != nullptr);

    m_consumer = new Mlt::FilteredConsumer(*profile, "avformat", target.toUtf8().constData());

    //
    //设置参数
    //
    if (presets && presets->is_valid()) {
        for (int i = 0; i < presets->count(); i++)
            m_consumer->set(QString("%1").arg(presets->get_name(i)).toLatin1().constData(), presets->get(i));
    }
    if (target.endsWith(".mp4") || target.endsWith(".mov"))
        m_consumer->set("strict", "experimental");

    m_consumer->listen("consumer-frame-show", this, mlt_listener(on_frame_show));
    m_consumer->listen("consumer-stopped", this, mlt_listener(on_stopped));

    return m_consumer;
}

void EncodeTask::on_frame_show(mlt_consumer, void *self, mlt_frame frame)
{
    int position = mlt_frame_get_position(frame);

    EncodeTask *task = static_cast<EncodeTask *>(self);
    if (position >= task->duration() -1 )
        emit task->endOfStream();

    int percent = int(position * 100.0 / task->duration());
    emit task->progressUpdated(task->modelIndex(), uint(percent));

    qDebug() << "CURRENT POS:" << position << "   TOTAL: " <<  task->duration();

}

void EncodeTask::on_stopped(mlt_consumer, void *self)
{
    //可能是完成了，正常结束，也可能出错结束
    qDebug() << "XXXXXX XXXXXX FINISHED";

     EncodeTask *task = static_cast<EncodeTask *>(self);
     task->setStopped(true);
     task->resetProducer();

    emit task->finished(task, task->finishedNormally());
}

void EncodeTask::onEndOfStream()
{
    m_producer->set_speed(0);

//    m_finishedNormally = true;
    AbstractTask::setFinishedNormally(true);
}

void EncodeTask::resetProducer()
{
#if SHARE_VERSION
#if MOVIEMATOR_PRO
    if (Registration.registrationType() == Registration_None)
    {
        if (m_textFilter)
        {
            m_producer->detach(*m_textFilter);
            delete m_textFilter;
            m_textFilter = nullptr;
        }
    }
#endif
#endif
    m_producer->set_speed(0);

#ifndef MOVIEMATOR_PRO
    if (m_cut)
    {
        m_cut->clear();
        m_cut = NULL;
    }
#endif
}
