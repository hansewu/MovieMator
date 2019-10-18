#ifndef HTMLEFFECTPRODUCERWIDGET_H
#define HTMLEFFECTPRODUCERWIDGET_H

#include <QWidget>
#include "abstractproducerwidget.h"
#include "mltcontroller.h"

namespace Ui {
class HtmlEffectProducerWidget;
}

class HtmlEffectProducerWidget : public QWidget, public AbstractProducerWidget
{
    Q_OBJECT

public:
    explicit HtmlEffectProducerWidget(QWidget *parent = nullptr);
    ~HtmlEffectProducerWidget();

    // AbstractProducerWidget overrides
    Mlt::Producer* producer(Mlt::Profile&);
    virtual void setProducer(Mlt::Producer*);

signals:
    void producerChanged(Mlt::Producer*);

private:
    Ui::HtmlEffectProducerWidget *ui;
    int m_defaultDuration;

    Mlt::Producer *m_tempProducer;
    Mlt::Producer * createTempProducer(Mlt::Profile&);
};

#endif // HTMLEFFECTPRODUCERWIDGET_H
