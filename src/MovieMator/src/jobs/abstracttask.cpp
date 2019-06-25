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
#include "abstracttask.h"

AbstractTask::AbstractTask(const QString &name) : QObject()
    , m_ran(false)
    , m_running(false)
    , m_finishedNormally(false)
    , m_killed(false)
    , m_stopped(false)
    , m_label(name)
{

}


void AbstractTask::start()
{
    m_ran = true;
}

void AbstractTask::stop()
{
    m_killed = true;
}


void AbstractTask::setModelIndex(const QModelIndex &index)
{
    m_index = index;
}

QModelIndex AbstractTask:: modelIndex () const
{
    return m_index;
}


void AbstractTask::setLabel(const QString& name)
{
    m_label = name;
}


void AbstractTask::setStopped(bool stopped)
{
    m_stopped = stopped;
}

void AbstractTask::setFinishedNormally(bool finishedNormally)
{
    m_finishedNormally = finishedNormally;
}
