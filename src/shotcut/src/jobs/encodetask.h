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
    QString&                m_outputName;
    Mlt::Filter             *m_textFilter;


    static void on_frame_show(mlt_consumer, void* self, mlt_frame frame);
    static void on_stopped(mlt_consumer, void* self);
    int duration(){return m_duration;}
    void resetProducer();

};

#endif // ENCODETASK_H
