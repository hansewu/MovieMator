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

#include "encodetaskqueue.h"

EncodeTaskQueue::EncodeTaskQueue(QObject *parent)
    : QStandardItemModel(0, COLUMN_COUNT, parent)
{

}


EncodeTaskQueue::~EncodeTaskQueue()
{

}

EncodeTaskQueue& EncodeTaskQueue::singleton(QObject *parent){
    static EncodeTaskQueue *instance = nullptr;
    if (instance == nullptr)
        instance = new EncodeTaskQueue(parent);
    return *instance;
}

void EncodeTaskQueue::addTask(AbstractTask *task)
{
    Q_ASSERT(task);
    int row = rowCount();
    QList<QStandardItem*> items;
    items << new QStandardItem(task->label());
    items << new QStandardItem(tr("pending"));
    appendRow(items);
    task->setParent(this);
    task->setModelIndex(index(row, COLUMN_STATUS));
    connect(task, SIGNAL(progressUpdated(QModelIndex, uint)), this, SLOT(onProgressUpdated(QModelIndex,uint)));
    connect(task, SIGNAL(finished(AbstractTask *, bool)), this, SLOT(onFinished(AbstractTask *, bool)));
    m_mutex.lock();
    m_tasks.append(task);
    m_mutex.unlock();

    emit taskAdded();

    startNextTask();
}

void EncodeTaskQueue::startNextTask()
{
    QMutexLocker locker(&m_mutex);
    if (!m_tasks.isEmpty()) {
        foreach(AbstractTask* task, m_tasks) {
            // if there is already a job started or running, then exit
            if (task->ran() && !task->stopped())
                break;
            // otherwise, start first non-started job and exit
            if (!task->ran()) {
                task->start();
                break;
            }
        }
    }
}

bool EncodeTaskQueue::hasIncomplete() const
{
    foreach (AbstractTask* task, m_tasks) {
        //if (!task->ran() || task->running())
        if (!task->stopped())
            return true;
    }
    return false;
}


void EncodeTaskQueue::cleanup()
{
    QMutexLocker locker(&m_mutex);
    qDeleteAll(m_tasks);
}

AbstractTask *EncodeTaskQueue::taskFromIndex(const QModelIndex &index) const
{
    Q_ASSERT(index.row() < m_tasks.size());
    return m_tasks.at(index.row());
}

void EncodeTaskQueue::remove(const QModelIndex &index)
{
    int row = index.row();
    removeRow(index.row());
    m_mutex.lock();

    AbstractTask* task = m_tasks.at(row);
    m_tasks.removeOne(task);
    delete task;

    // Reindex the subsequent jobs.
    for (int i = row; i < m_tasks.size(); ++i) {
        QModelIndex modelIndex = this->index(i, COLUMN_STATUS);
        m_tasks.at(i)->setModelIndex(modelIndex);
    }
    m_mutex.unlock();
}

void EncodeTaskQueue::stopTasks()
{
    foreach (AbstractTask* task, m_tasks) {
//        if (!task->ran() || task->running())
        if (!task->stopped())
            task->stop();
    }
}

void EncodeTaskQueue::onProgressUpdated(QModelIndex index, uint percent)
{
    QStandardItem* item = itemFromIndex(index);
    Q_ASSERT(item);
    if (item)
        item->setText(QString("%1%").arg(percent));
}

void EncodeTaskQueue::onFinished(AbstractTask *task, bool isSuccess)
{
    Q_ASSERT(task);
    QStandardItem* item = itemFromIndex(task->modelIndex());
    if (item) {
        if (isSuccess)
            item->setText(tr("done"));
        else if (task->killed())
            item->setText(tr("stopped"));
        else
            item->setText(tr("failed"));
    }
    startNextTask();
}

