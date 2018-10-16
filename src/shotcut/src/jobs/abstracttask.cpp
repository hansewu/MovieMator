#include "abstracttask.h"

AbstractTask::AbstractTask(const QString &name) : QObject()
    , m_label(name)
    , m_ran(false)
    , m_finishedNormally(false)
    , m_running(false)
    , m_stopped(false)
    , m_killed(false)
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
