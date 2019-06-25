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
#ifndef ENCODETASK_H
#define ENCODETASK_H

#include <QObject>
#include <Mlt.h>
#include <QModelIndex>
#include "abstracttask.h"

class EncodeTask : public AbstractTask
{
    Q_OBJECT
public:
    explicit EncodeTask(Mlt::Producer *producer, Mlt::Profile *profile, Mlt::Properties *presets, QString target);

    ~EncodeTask();
    void start();
    void stop();

    Mlt::FilteredConsumer *createConsumer(Mlt::Profile *profile, Mlt::Properties *presets, QString target);

//    void setModelIndex(const QModelIndex& index);
//    QModelIndex modelIndex() const;

    QString outputName() const { return m_outputName; }


signals:
    void endOfStream();

public slots:
    void onEndOfStream();

private:
    Mlt::FilteredConsumer   *m_consumer;
    Mlt::Producer           *m_producer;
    Mlt::Producer           *m_cut;
    int                     m_duration;
    QString                 m_outputName;
    Mlt::Filter             *m_textFilter;


    static void on_frame_show(mlt_consumer, void* self, mlt_frame frame);
    static void on_stopped(mlt_consumer, void* self);
    int duration(){return m_duration;}
    void resetProducer();

};

#endif // ENCODETASK_H
