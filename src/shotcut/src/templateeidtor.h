#ifndef TEMPLATEEIDTOR_H
#define TEMPLATEEIDTOR_H

#include <QObject>
#include <Mlt.h>
#include <maininterface.h>

typedef struct {
    int trackIndex;
    int clipIndex;
    Mlt::Producer *producer;
} TEMPLATE_CLIP_INFO;

class TemplateEidtor : public QObject
{
    Q_OBJECT
public:
    explicit TemplateEidtor(QObject *parent = nullptr);
    ~TemplateEidtor();

    int replaceFileInTemplate(int index, Mlt::Producer *producer);
    FILE_HANDLE resetFileToTemplateDefault(int index);

signals:

public slots:
    int setProducer(Mlt::Producer *producer);

private:
    Mlt::Producer *m_templateProducer;
    QList<TEMPLATE_CLIP_INFO> m_clipList;
};

#endif // TEMPLATEEIDTOR_H
