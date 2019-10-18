#include "htmleffectproducerwidget.h"
#include "ui_htmleffectproducerwidget.h"
#include "settings.h"
#include "mainwindow.h"
#include "shotcut_mlt_properties.h"
#include "util.h"
#include <QFileInfo>

HtmlEffectProducerWidget::HtmlEffectProducerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HtmlEffectProducerWidget),
    m_defaultDuration(-1),
    m_tempProducer(nullptr)
{
    qDebug()<<"--HtmlEffectProducerWidget构造---start";

    ui->setupUi(this);

    qDebug()<<"---HtmlEffectProducerWidget构造---end";

}

HtmlEffectProducerWidget::~HtmlEffectProducerWidget()
{
    qDebug()<<"--HtmlEffectProducerWidget析构---start";

    delete ui;

    qDebug()<<"---HtmlEffectProducerWidget析构---end";
}

Mlt::Producer* HtmlEffectProducerWidget::producer(Mlt::Profile& profile)
{
    Mlt::Producer* p = new Mlt::Producer(profile, m_producer->get("resource"));
    return p;
}

void HtmlEffectProducerWidget::setProducer(Mlt::Producer* p)
{
    AbstractProducerWidget::setProducer(p);
    delete m_tempProducer;
    m_tempProducer = createTempProducer(MLT.profile());
    if (m_defaultDuration == -1)
        m_defaultDuration = m_tempProducer->get_length();

    QString strProducerName = p->get("moviemator:producer_name");

    ui->filenameLabel->setText(ui->filenameLabel->fontMetrics().elidedText(strProducerName, Qt::ElideLeft, width() - 40));

    ui->durationSpinBox->setValue(m_tempProducer->get_out() + 1);
}

Mlt::Producer * HtmlEffectProducerWidget::createTempProducer(Mlt::Profile& profile)
{
    Q_UNUSED(profile)
    Mlt::Producer* p = nullptr;

    p = producer(MLT.profile());

    p->pass_list(*m_producer, "force_aspect_ratio,"
        kAspectRatioNumerator "," kAspectRatioDenominator ","
        kShotcutResourceProperty ", resource, ttl, length," kShotcutSequenceProperty ", in, out");

    return p;
}
