#ifndef MELTTASK_H
#define MELTTASK_H

#include "abstracttask.h"
#include <QTemporaryFile>

class MeltThread;

class MeltTask : public AbstractTask
{
public:
    explicit MeltTask(const QString& name, const QString& xml);
    ~MeltTask();

    void start();
    void stop();

    QString xmlPath() const {return m_xml.fileName();}

    static int meltCallback(int done, int success, int percent, void *callbackObj);
private:
    QTemporaryFile m_xml;
    MeltThread *m_meltThread;

};

#endif // MELTTASK_H
