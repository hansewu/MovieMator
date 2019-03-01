/*
 * Copyright (c) 2012-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
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

AvformatProducerSimpleWidget::AvformatProducerSimpleWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AvformatProducerSimpleWidget)
    , m_defaultDuration(-1)
    , m_recalcDuration(true)
    , m_tempProducer(0)
{
    ui->setupUi(this);
//    Util::setColorsToHighlight(ui->filenameLabel);
    connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onFrameDisplayed(const SharedFrame&)));

    ui->filenameLabel->setStyleSheet("background-color:rgb(82,82,82);");
    ui->line->setStyleSheet("color:black;");
    QString buttonStyle = "QPushButton{"
                          "border-radius:3px;border:1px solid;border-color:black;"
                          "background-color:rgb(100,100,100);color:rgb(225,225,225);}";
    ui->resetButton->setStyleSheet(buttonStyle);
    ui->okButton->setStyleSheet(buttonStyle);
    ui->advanced->setStyleSheet(buttonStyle);
    QString spinBoxStyle = "QSpinBox{padding-left:4px;background-color:rgb(82,82,82);color:rgb(225,225,225);border:1px solid black;border-radius:4px;}";
    spinBoxStyle.append("QSpinBox::up-button{subcontrol-origin:border;subcontrol-position:top right;width:18px;border:none;}");
    spinBoxStyle.append("QSpinBox::up-arrow{image:url(:/icons/light/8x8/up.png);width:8px;height:8px;}");
    spinBoxStyle.append("QSpinBox::down-button{subcontrol-origin:border;subcontrol-position:bottom right;width:16px;border:none;border-top-width:0;}");
    spinBoxStyle.append("QSpinBox::down-arrow{image:url(:/icons/light/8x8/down.png);width:8px;height:8px;}");
    ui->startPointSpinBox->setStyleSheet(spinBoxStyle);
    ui->endPointSpinBox->setStyleSheet(spinBoxStyle);
    QString doubleSpinBoxStyle = spinBoxStyle.replace("QSpinBox", "QDoubleSpinBox");
    ui->speedSpinBox->setStyleSheet(doubleSpinBoxStyle);
}

AvformatProducerSimpleWidget::~AvformatProducerSimpleWidget()
{
    if (m_tempProducer)
        delete m_tempProducer;
    m_tempProducer = 0;
    if (m_producer)
        delete  m_producer;
    m_producer = 0;
    delete ui;
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
    int in = p->get_in();
    int out = p->get_out();
    int position = p->position();
    double speed = p->get_speed();

    if( m_recalcDuration )
    {
        double oldSpeed = GetSpeedFromProducer(m_tempProducer);
        double newSpeed = ui->speedSpinBox->value();
        double speedRatio = oldSpeed / newSpeed;

        length = qRound(length * speedRatio);
        p->set("length", length);
        p->set_in_and_out(qMin(qRound(in * speedRatio), length - 1),
                          qMin(qRound(out * speedRatio), length - 1));
        position = qRound(position * speedRatio);
    }

    if (MLT.setProducer(p)) {
        setProducer(0);
        return;
    }
    MLT.stop();

    setProducer(p);

    connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onFrameDisplayed(const SharedFrame&)));
    emit producerReopened();
    emit producerChanged(p);
    MLT.seek(position);
    MLT.play(speed);
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


void AvformatProducerSimpleWidget::on_speedSpinBox_editingFinished()
{
    if (!m_tempProducer)
        return;
    m_recalcDuration = true;
}

void AvformatProducerSimpleWidget::setProducer(Mlt::Producer *aProducer)
{
    delete m_tempProducer;
    delete m_producer;
    m_producer = 0;//保存原producer数据
    m_tempProducer = 0;//接收改变的临时producer
    if (aProducer) {
        loadPreset(*aProducer);
        double speed = GetSpeedFromProducer(aProducer);
        m_producer = new Mlt::Producer(aProducer);//createProducer(MLT.profile(), aProducer, speed);
        m_tempProducer = createProducer(MLT.profile(), aProducer, speed);
    }
}

//创建新的producer
Mlt::Producer * AvformatProducerSimpleWidget::createProducer(Mlt::Profile& profile, Mlt::Producer *producer, double speed)
{
    Mlt::Producer* p = NULL;
    if ( speed == 1.0 )
    {
        p = new Mlt::Producer(profile, GetFilenameFromProducer(producer));
    }
    else
    {
        char* filename = GetFilenameFromProducer(producer);
        QString s = QString("%1:%L2:%3").arg("timewarp").arg(speed).arg(filename);
        p = new Mlt::Producer(profile, s.toUtf8().constData());
        p->set(kShotcutProducerProperty, "avformat");
    }

    p->pass_list(*producer, "audio_index, video_index, force_aspect_ratio,"
                 "video_delay, force_progressive, force_tff,"
                 kAspectRatioNumerator ","
                 kAspectRatioDenominator ","
                 kShotcutHashProperty);
    Mlt::Controller::copyFilters(*m_producer, *p);
    p->set_in_and_out(producer->get_in(), producer->get_out());
    p->set(kMultitrackItemProperty, producer->get(kMultitrackItemProperty));
    return p;
}

void AvformatProducerSimpleWidget::on_advanced_clicked()
{
    MAIN.onPropertiesDockTriggered(true);
}

void AvformatProducerSimpleWidget::on_okButton_clicked()
{
    double speed = ui->speedSpinBox->value();
    Mlt::Producer *producer = createProducer(MLT.profile(), m_tempProducer, speed);

    //使用用tempProducer发送producerChanged消息
    if (producer->get_int(kMultitrackItemProperty)) {
        emit producerChanged(producer);
        delete producer;
    } else {
        reopen(producer);
    }
}


void AvformatProducerSimpleWidget::on_resetButton_clicked()
{
    ui->speedSpinBox->setValue(m_producer->get_speed());//
    onFrameDisplayed(SharedFrame());
}

Mlt::Producer* AvformatProducerSimpleWidget::producer(Mlt::Profile& profile)
{
    return 0;
}
