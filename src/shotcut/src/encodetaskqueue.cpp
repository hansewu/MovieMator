#include "encodetaskqueue.h"

EncodeTaskQueue::EncodeTaskQueue(QObject *parent)
    : QStandardItemModel(0, COLUMN_COUNT, parent)
{

}


EncodeTaskQueue::~EncodeTaskQueue()
{

}

EncodeTaskQueue& EncodeTaskQueue::singleton(QObject *parent){
    static EncodeTaskQueue *instance = 0;
    if (instance == 0)
        instance = new EncodeTaskQueue(parent);
    return *instance;
}

void EncodeTaskQueue::addTask(AbstractTask *task)
{

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
    if (item)
        item->setText(QString("%1%").arg(percent));
}

void EncodeTaskQueue::onFinished(AbstractTask *task, bool isSuccess)
{
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

