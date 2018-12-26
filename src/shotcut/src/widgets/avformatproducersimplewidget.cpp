/*
 * Copyright (c) 2012-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
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

#include "avformatproducersimplewidget.h"
#include "ui_avformatproducersimplewidget.h"
#include "util.h"
#include "mltcontroller.h"
#include "shotcut_mlt_properties.h"
#include "jobqueue.h"
#include "jobs/ffprobejob.h"
#include "jobs/ffmpegjob.h"
#include <QtWidgets>
#include "mainwindow.h"
#include "../models/multitrackmodel.h"

//bool ProducerIsTimewarp( Mlt::Producer* producer )
//{
//    return QString::fromUtf8(producer->get("mlt_service")) == "timewarp";
//}

//char* GetFilenameFromProducer( Mlt::Producer* producer )
//{
//    char* resource = NULL;
//    if (ProducerIsTimewarp(producer))
//    {
//        resource = producer->get("warp_resource");
//    }
//    else
//    {
//        resource = producer->get("resource");
//    }
//    return resource;
//}

//double GetSpeedFromProducer( Mlt::Producer* producer )
//{
//    double speed = 1.0;
//    if (ProducerIsTimewarp(producer) )
//    {
//        speed = fabs(producer->get_double("warp_speed"));
//    }
//    return speed;
//}

AvformatProducerSimpleWidget::AvformatProducerSimpleWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AvformatProducerSimpleWidget)
    , m_defaultDuration(-1)
    , m_recalcDuration(true)
    , m_tempProducer(0)
{
    ui->setupUi(this);
    Util::setColorsToHighlight(ui->filenameLabel);
    connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onFrameDisplayed(const SharedFrame&)));
}

AvformatProducerSimpleWidget::~AvformatProducerSimpleWidget()
{
    delete ui;
}

Mlt::Producer* AvformatProducerSimpleWidget::producer(Mlt::Profile& profile)
{
    Mlt::Producer* p = NULL;
    if ( ui->speedSpinBox->value() == 1.0 )
    {
        p = new Mlt::Producer(profile, GetFilenameFromProducer(m_producer));
    }
    else
    {
        double warpspeed = ui->speedSpinBox->value();
        char* filename = GetFilenameFromProducer(m_producer);
        QString s = QString("%1:%L2:%3").arg("timewarp").arg(warpspeed).arg(filename);
        p = new Mlt::Producer(profile, s.toUtf8().constData());
        p->set(kShotcutProducerProperty, "avformat");
    }
//    if (p->is_valid())
//        p->set("video_delay", double(ui->syncSlider->value()) / 1000);
    return p;
}

void AvformatProducerSimpleWidget::keyPressEvent(QKeyEvent* event)
{
    if (ui->speedSpinBox->hasFocus() &&
            (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
        ui->speedSpinBox->clearFocus();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void AvformatProducerSimpleWidget::reopen(Mlt::Producer* p)
{
    int length = ui->endPointSpinBox->value() - ui->startPointSpinBox->value();
    int out = m_producer->get_out();
    int position = m_producer->position();
    double speed = m_producer->get_speed();

    if( m_recalcDuration )
    {
        double oldSpeed = GetSpeedFromProducer(m_producer);
        double newSpeed = ui->speedSpinBox->value();
        double speedRatio = oldSpeed / newSpeed;
        int in = m_producer->get_in();

        length = qRound(length * speedRatio);
        p->set("length", length);
        p->set_in_and_out(qMin(qRound(in * speedRatio), length - 1),
                          qMin(qRound(out * speedRatio), length - 1));
        position = qRound(position * speedRatio);
    }
    else
    {
        p->set("length", length);
        if (out + 1 >= m_producer->get_length())
            p->set("out", length - 1);
        else if (out >= length)
            p->set("out", length - 1);
        else
            p->set("out", out);
        if (position > p->get_out())
            position = p->get_out();
        p->set("in", m_producer->get_in());
    }
    if (MLT.setProducer(p)) {
        setProducer(0);
        return;
    }
    MLT.stop();
//    connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onFrameDisplayed(const SharedFrame&)));
    emit producerReopened();
    emit producerChanged(p);
    MLT.seek(position);
    MLT.play(speed);
//    setProducer(p);
}

void AvformatProducerSimpleWidget::recreateProducer()
{
    Mlt::Producer* p = producer(MLT.profile());
    p->pass_list(*m_producer, "audio_index, video_index, force_aspect_ratio,"
                 "video_delay, force_progressive, force_tff,"
                 kAspectRatioNumerator ","
                 kAspectRatioDenominator ","
                 kShotcutHashProperty);
    Mlt::Controller::copyFilters(*m_producer, *p);
    if (m_producer->get_int(kMultitrackItemProperty)) {
        emit producerChanged(p);
        delete p;
    } else {
        reopen(p);
    }
}

void AvformatProducerSimpleWidget::onFrameDisplayed(const SharedFrame&)
{
    // This forces avformat-novalidate or unloaded avformat to load and get
    // media information.
    delete m_tempProducer->get_frame();

    if (m_defaultDuration == -1)
        m_defaultDuration = m_tempProducer->get_length();

    double warpSpeed = GetSpeedFromProducer(m_tempProducer);
    QString s = QString::fromUtf8(GetFilenameFromProducer(m_tempProducer));
    QString name = Util::baseName(s);
    QString caption = name;
    if(warpSpeed != 1.0)
        caption = QString("%1 (%2x)").arg(name).arg(warpSpeed);
    m_tempProducer->set(kShotcutCaptionProperty, caption.toUtf8().constData());
    m_tempProducer->set(kShotcutDetailProperty, s.toUtf8().constData());
    ui->filenameLabel->setText(ui->filenameLabel->fontMetrics().elidedText(caption, Qt::ElideLeft, width() - 30));
    ui->filenameLabel->setToolTip(s);
    ui->startPointSpinBox->setValue(m_tempProducer->get_in());
    ui->endPointSpinBox->setValue(m_tempProducer->get_out());
    m_recalcDuration = false;
    ui->speedSpinBox->setValue(warpSpeed);
    disconnect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, 0);
}

void AvformatProducerSimpleWidget::on_resetButton_clicked()
{
//    ui->speedSpinBox->setValue(1.0);
//    Mlt::Producer* p = producer(MLT.profile());
//    ui->durationSpinBox->setValue(m_defaultDuration);
//    ui->syncSlider->setValue(0);
//    if (m_producer->get_int(kMultitrackItemProperty)) {
//        emit producerChanged(p);
//        delete p;
//    } else {
//        reopen(p);
//    }

    ui->speedSpinBox->setValue(1.0);
    Mlt::Producer *p = producer(MLT.profile());
    if (p->is_valid())
    {
        delete m_tempProducer;
        m_tempProducer = p;
        onFrameDisplayed(SharedFrame());
    }
}


void AvformatProducerSimpleWidget::on_durationSpinBox_editingFinished()
{
//    if (!m_tempProducer)
//        return;
//    if (ui->durationSpinBox->value() == m_tempProducer->get_length())
//        return;
//    recreateTempProducer();
}


void AvformatProducerSimpleWidget::on_speedSpinBox_editingFinished()
{
    if (!m_tempProducer)
        return;
    m_recalcDuration = true;
//    recreateProducer();
    recreateTempProducer();
}


void AvformatProducerSimpleWidget::on_actionOpenFolder_triggered()
{
    QFileInfo fi(GetFilenameFromProducer(m_producer));
    QDesktopServices::openUrl(QUrl::fromLocalFile(fi.path()));
}


void AvformatProducerSimpleWidget::on_actionCopyFullFilePath_triggered()
{
    qApp->clipboard()->setText(GetFilenameFromProducer(m_producer));
}


void AvformatProducerSimpleWidget::on_okButton_clicked()
{
    //使用用tempProducer发送producerChanged消息
    if (m_producer->get_int(kMultitrackItemProperty)) {
        emit producerChanged(m_tempProducer);
        delete m_tempProducer;
        m_tempProducer = 0;
    } else {
        reopen(m_tempProducer);
        m_tempProducer = 0;
    }
}


void AvformatProducerSimpleWidget::setProducer(Mlt::Producer *aProducer)
{
    delete m_tempProducer;
    delete m_producer;
    m_producer = 0;
    m_tempProducer = 0;
    if (aProducer) {
        loadPreset(*aProducer);
        m_producer = new Mlt::Producer(aProducer);
        m_tempProducer = createTempProducer(MLT.profile());
        m_tempProducer->set_in_and_out(aProducer->get_in(), aProducer->get_out());
    }
}

Mlt::Producer * AvformatProducerSimpleWidget::createTempProducer(Mlt::Profile& profile)
{
    Mlt::Producer* p = NULL;
    double speed = GetSpeedFromProducer(m_producer);
    if ( speed == 1.0 )
    {
        p = new Mlt::Producer(profile, GetFilenameFromProducer(m_producer));
    }
    else
    {
        char* filename = GetFilenameFromProducer(m_producer);
        QString s = QString("%1:%L2:%3").arg("timewarp").arg(speed).arg(filename);
        p = new Mlt::Producer(profile, s.toUtf8().constData());
        p->set(kShotcutProducerProperty, "avformat");
    }

    p->pass_list(*m_producer, "audio_index, video_index, force_aspect_ratio,"
                 "video_delay, force_progressive, force_tff,"
                 kAspectRatioNumerator ","
                 kAspectRatioDenominator ","
                 kShotcutHashProperty);
    Mlt::Controller::copyFilters(*m_producer, *p);
    return p;
}

Mlt::Producer *AvformatProducerSimpleWidget::recreateTempProducer()
{
    Mlt::Producer* p = producer(MLT.profile());
    p->pass_list(*m_tempProducer, "audio_index, video_index, force_aspect_ratio,"
                 "video_delay, force_progressive, force_tff,"
                 kAspectRatioNumerator ","
                 kAspectRatioDenominator ","
                 kShotcutHashProperty);
    Mlt::Controller::copyFilters(*m_tempProducer, *p);
    delete m_tempProducer;
    m_tempProducer = 0;
    m_tempProducer = p;

    return p;
}

void AvformatProducerSimpleWidget::on_advanced_clicked()
{
    MAIN.onPropertiesDockTriggered(true);
}
