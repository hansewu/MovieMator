#include "melttask.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QIODevice>
#include <QThread>

#include "melt/melt.h"

extern "C" int melt_main(int argc, char** argv, MELT_CALLBACK callback, void *callback_obj);
//int qmelt(int argc, char** argv);

volatile int stop_melt_thread = 0;


class MeltThread : public QThread
{
public:
    MeltThread(const QString& xmlPath, MELT_CALLBACK callback, void *callbackObj)
        : QThread() , m_xmlPath(xmlPath), m_callback(callback), m_callbackObj(callbackObj){
    }

protected:
    void run() {
        int     i = 0;
        int     argc = 0;
        char    **argv;
        argv = (char**)malloc(20*sizeof(char*));

        for(i =0; i<20; i++)
            argv[i] = (char*)malloc( 256*sizeof(char));

        strcpy(argv[argc], "melt");
        argc++;
        strcpy(argv[argc], "-progress2");
        argc++;
    //    strcpy(argv[argc], "-abort");
    //    argc++;
        strcpy(argv[argc], m_xmlPath.toUtf8().constData());
        argc++;

        for(i =argc; i<20; i++)
            argv[i] = NULL;

        melt_main(argc, argv, m_callback, m_callbackObj);

        for(i = 0; i<20; i++)
        {
            free(argv[i]);
        }
        free(argv);

    }

private:
    QString m_xmlPath;
    MELT_CALLBACK m_callback;
    void *m_callbackObj;
};


MeltTask::MeltTask(const QString& name, const QString& xml) : AbstractTask(name)
    , m_xml(QDir::tempPath().append("/MovieMator-XXXXXX.mmp"))
{
    m_xml.open();
    m_xml.write(xml.toUtf8());
    m_xml.close();

    m_meltThread = new MeltThread(m_xml.fileName(), meltCallback, (void *)this);
}

MeltTask::~MeltTask()
{
    delete m_meltThread;
}

void MeltTask::start()
{

    AbstractTask::start();


    m_meltThread->start();

}

void MeltTask::stop()
{
    stop_melt_thread = 1;
    AbstractTask::stop();

}

int MeltTask::meltCallback(int done, int success, int percent, void *callbackObj)
{
    if (stop_melt_thread)
    {
        stop_melt_thread = 0;
        return 1;
    }

    MeltTask *task = static_cast<MeltTask *>(callbackObj);

    emit task->progressUpdated(task->modelIndex(), percent);


    if (done)
    {
        task->setStopped(true);
        emit task->finished(task, task->finishedNormally());
    }
    if (success)
        task->setFinishedNormally(true);


    return 0;
}


