#ifndef ENCODETASKQUEUE_H
#define ENCODETASKQUEUE_H
#include <QStandardItemModel>
#include <QMutex>
#include "jobs/encodetask.h"
#include "jobs/abstracttask.h"

class EncodeTaskQueue : public QStandardItemModel
{
    Q_OBJECT


public:

    enum ColumnRole {
        COLUMN_OUTPUT = 0,
        COLUMN_STATUS,
        COLUMN_COUNT
    };

    explicit EncodeTaskQueue(QObject *parent = 0);
    ~EncodeTaskQueue();

    static EncodeTaskQueue& singleton(QObject* parent = 0);

    void addTask(AbstractTask *task);
    void cleanup();
    AbstractTask *taskFromIndex(const QModelIndex &index) const;
    void remove(const QModelIndex &index);
    void stopTasks();

    void startNextTask();

    bool hasIncomplete() const;


signals:
    void taskAdded(); //通知主窗口显示TaskDock

public slots:
    void onProgressUpdated(QModelIndex index, uint percent);//处理task的进度更新消息，更新进度值
    void onFinished(AbstractTask *task, bool isSuccess);//接收task完成消息，更新状态并启动下一个task，在线程里面调用，因此需要一个互斥锁保护m_tasks

private:
    QList<AbstractTask *>m_tasks;
    QMutex m_mutex;
};

#define ENCODETASKS EncodeTaskQueue::singleton()
#endif // ENCODETASKQUEUE_H
