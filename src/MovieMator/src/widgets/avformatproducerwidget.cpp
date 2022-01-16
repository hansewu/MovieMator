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

#include "avformatproducerwidget.h"
#include "ui_avformatproducerwidget.h"
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

AvformatProducerWidget::AvformatProducerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AvformatProducerWidget)
    , m_defaultDuration(-1)
    , m_recalcDuration(true)
    , m_tempProducer(nullptr)
{
    ui->setupUi(this);
    Util::setColorsToHighlight(ui->filenameLabel);
    connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onFrameDisplayed(const SharedFrame&)));
    ui->label->hide();
    ui->notesTextEdit->hide();
    ui->menuButton->hide();
    ui->speedLabel->hide();
    ui->speedSpinBox->hide();
}

AvformatProducerWidget::~AvformatProducerWidget()
{
    if (m_tempProducer)
    {
        delete m_tempProducer;
        m_tempProducer = nullptr;
    }
    if (m_producer)
    {
        delete m_producer;
        m_producer = nullptr;
    }

    delete ui;
}

Mlt::Producer* AvformatProducerWidget::producer(Mlt::Profile& profile)
{
    Mlt::Producer* p = nullptr;
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

void AvformatProducerWidget::keyPressEvent(QKeyEvent* event)
{
    if (ui->speedSpinBox->hasFocus() &&
            (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
        ui->speedSpinBox->clearFocus();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void AvformatProducerWidget::reopen(Mlt::Producer* p)
{
    int length = ui->durationSpinBox->value();
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
        setProducer(nullptr);
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

void AvformatProducerWidget::onFrameDisplayed(const SharedFrame&)
{
    // This forces avformat-novalidate or unloaded avformat to load and get
    // media information.
    delete m_tempProducer->get_frame();

    ui->tabWidget->setTabEnabled(0, false);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
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
    ui->notesTextEdit->setPlainText(QString::fromUtf8(m_tempProducer->get(kCommentProperty)));
    ui->durationSpinBox->setValue(m_tempProducer->get_length());
    m_recalcDuration = false;
    ui->speedSpinBox->setValue(warpSpeed);

    // populate the track combos
    int n = m_tempProducer->get_int("meta.media.nb_streams");
    int videoIndex = 1;
    int audioIndex = 1;
    bool populateTrackCombos = (ui->videoTrackComboBox->count() == 0 &&
                                ui->audioTrackComboBox->count() == 0);
    for (int i = 0; i < n; i++) {
        QString key = QString("meta.media.%1.stream.type").arg(i);
        QString streamType(m_tempProducer->get(key.toLatin1().constData()));
        if (streamType == "video") {
            key = QString("meta.media.%1.codec.name").arg(i);
            QString codec(m_tempProducer->get(key.toLatin1().constData()));
            key = QString("meta.media.%1.codec.width").arg(i);
            QString width(m_tempProducer->get(key.toLatin1().constData()));
            key = QString("meta.media.%1.codec.height").arg(i);
            QString height(m_tempProducer->get(key.toLatin1().constData()));
            QString name = QString("%1: %2x%3 %4")
                    .arg(videoIndex)
                    .arg(width)
                    .arg(height)
                    .arg(codec);
            if (populateTrackCombos) {
                if (ui->videoTrackComboBox->count() == 0)
                    ui->videoTrackComboBox->addItem(tr("None"), -1);
                ui->videoTrackComboBox->addItem(name, i);
            }
            if (i == m_tempProducer->get_int("video_index")) {
                key = QString("meta.media.%1.codec.long_name").arg(i);
                QString codec(m_tempProducer->get(key.toLatin1().constData()));
                ui->videoTableWidget->setItem(0, 1, new QTableWidgetItem(codec));
                key = QString("meta.media.%1.codec.pix_fmt").arg(i);
                ui->videoTableWidget->setItem(3, 1, new QTableWidgetItem(
                    m_tempProducer->get(key.toLatin1().constData())));
                ui->videoTrackComboBox->setCurrentIndex(videoIndex);
            }
            ui->tabWidget->setTabEnabled(0, true);
            videoIndex++;
        }
        else if (streamType == "audio") {
            key = QString("meta.media.%1.codec.name").arg(i);
            QString codec(m_tempProducer->get(key.toLatin1().constData()));
            key = QString("meta.media.%1.codec.channels").arg(i);
            QString channels(m_tempProducer->get(key.toLatin1().constData()));
            key = QString("meta.media.%1.codec.sample_rate").arg(i);
            QString sampleRate(m_tempProducer->get(key.toLatin1().constData()));
            QString name = QString("%1: %2 ch %3 KHz %4")
                    .arg(audioIndex)
                    .arg(channels)
                    .arg(sampleRate.toDouble()/1000)
                    .arg(codec);
            if (populateTrackCombos) {
                if (ui->audioTrackComboBox->count() == 0)
                    ui->audioTrackComboBox->addItem(tr("None"), -1);
                ui->audioTrackComboBox->addItem(name, i);
            }
            if (i == m_tempProducer->get_int("audio_index")) {
                key = QString("meta.media.%1.codec.long_name").arg(i);
                QString codec(m_tempProducer->get(key.toLatin1().constData()));
                ui->audioTableWidget->setItem(0, 1, new QTableWidgetItem(codec));
                ui->audioTableWidget->setItem(1, 1, new QTableWidgetItem(channels));
                ui->audioTableWidget->setItem(2, 1, new QTableWidgetItem(sampleRate));
                key = QString("meta.media.%1.codec.sample_fmt").arg(i);
                ui->audioTableWidget->setItem(3, 1, new QTableWidgetItem(
                    m_tempProducer->get(key.toLatin1().constData())));
                ui->audioTrackComboBox->setCurrentIndex(audioIndex);
            }
            ui->tabWidget->setTabEnabled(1, true);
            audioIndex++;
        }
    }
    if (!ui->tabWidget->isTabEnabled(0))
        ui->tabWidget->setCurrentIndex(1);

    int width = m_tempProducer->get_int("meta.media.width");
    int height = m_tempProducer->get_int("meta.media.height");
    ui->videoTableWidget->setItem(1, 1, new QTableWidgetItem(QString("%1x%2").arg(width).arg(height)));

    // We can stop listening to this signal if this is audio-only or if we have
    // received the video resolution.
    if (videoIndex == 1 || width || height)
        disconnect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, nullptr);

    double sar = m_tempProducer->get_double("meta.media.sample_aspect_num");
    if (m_tempProducer->get_double("meta.media.sample_aspect_den") > 0)
        sar /= m_tempProducer->get_double("meta.media.sample_aspect_den");
    if (m_tempProducer->get("force_aspect_ratio"))
        sar = m_tempProducer->get_double("force_aspect_ratio");
    int dar_numerator = int(width * sar);
    int dar_denominator = height;
    if (height > 0) {
        switch (int(sar * width / height * 100)) {
        case 133:
            dar_numerator = 4;
            dar_denominator = 3;
            break;
        case 177:
            dar_numerator = 16;
            dar_denominator = 9;
            break;
        case 56:
            dar_numerator = 9;
            dar_denominator = 16;
        }
    }
    if (m_tempProducer->get(kAspectRatioNumerator))
        dar_numerator = m_tempProducer->get_int(kAspectRatioNumerator);
    if (m_tempProducer->get(kAspectRatioDenominator))
        dar_denominator = m_tempProducer->get_int(kAspectRatioDenominator);
    ui->aspectNumSpinBox->blockSignals(true);
    ui->aspectNumSpinBox->setValue(dar_numerator);
    ui->aspectNumSpinBox->blockSignals(false);
    ui->aspectDenSpinBox->blockSignals(true);
    ui->aspectDenSpinBox->setValue(dar_denominator);
    ui->aspectDenSpinBox->blockSignals(false);

    double fps = m_tempProducer->get_double("meta.media.frame_rate_num");
    if (m_tempProducer->get_double("meta.media.frame_rate_den") > 0)
        fps /= m_tempProducer->get_double("meta.media.frame_rate_den");
    if (m_tempProducer->get("force_fps"))
        fps = m_tempProducer->get_double("fps");
    ui->videoTableWidget->setItem(2, 1, new QTableWidgetItem(QString("%L1").arg(fps)));

    int progressive = m_tempProducer->get_int("meta.media.progressive");
    if (m_tempProducer->get("force_progressive"))
        progressive = m_tempProducer->get_int("force_progressive");
    ui->scanComboBox->setCurrentIndex(progressive);

    int tff = m_tempProducer->get_int("meta.media.top_field_first");
    if (m_tempProducer->get("force_tff"))
        tff = m_tempProducer->get_int("force_tff");
    ui->fieldOrderComboBox->setCurrentIndex(tff);
    ui->fieldOrderComboBox->setEnabled(!progressive);

    if (populateTrackCombos) {
        for (int i = 0; i < m_tempProducer->count(); i++) {
            QString name(m_tempProducer->get_name(i));
            if (name.startsWith("meta.attr.") && name.endsWith(".markup")) {
                int row = ui->metadataTable->rowCount();
                ui->metadataTable->setRowCount(row + 1);
                ui->metadataTable->setItem(row, 0, new QTableWidgetItem(name.section('.', -2, -2)));
                ui->metadataTable->setItem(row, 1, new QTableWidgetItem(m_tempProducer->get(i)));
                ui->tabWidget->setTabEnabled(2, true);
            }
        }
    }
    ui->syncSlider->setValue(qRound(m_tempProducer->get_double("video_delay") * 1000.0));
}

void AvformatProducerWidget::on_resetButton_clicked()
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

void AvformatProducerWidget::on_videoTrackComboBox_activated(int index)
{

    if (m_tempProducer) {
        m_tempProducer->set("video_index", ui->videoTrackComboBox->itemData(index).toInt());
        recreateTempProducer();
    }
}

void AvformatProducerWidget::on_audioTrackComboBox_activated(int index)
{
    if (m_tempProducer) {
        m_tempProducer->set("audio_index", ui->audioTrackComboBox->itemData(index).toInt());
        recreateTempProducer();
    }

}

void AvformatProducerWidget::on_scanComboBox_activated(int index)
{
    if (m_tempProducer) {
        int progressive = m_tempProducer->get_int("meta.media.progressive");
        ui->fieldOrderComboBox->setEnabled(!progressive);
        if (m_tempProducer->get("force_progressive") || progressive != index)
            // We need to set these force_ properties as a string so they can be properly removed
            // by setting them NULL.
            m_tempProducer->set("force_progressive", QString::number(index).toLatin1().constData());
//        emit producerChanged(m_tempProducer);
//        connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onFrameDisplayed(const SharedFrame&)));
    }
}

void AvformatProducerWidget::on_fieldOrderComboBox_activated(int index)
{
    if (m_tempProducer) {
        int tff = m_tempProducer->get_int("meta.media.top_field_first");
        if (m_tempProducer->get("force_tff") || tff != index)
            m_tempProducer->set("force_tff", QString::number(index).toLatin1().constData());
//        emit producerChanged(m_producer);
//        connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onFrameDisplayed(const SharedFrame&)));
    }
}

//void AvformatProducerWidget::on_aspectNumSpinBox_valueChanged(int)
void AvformatProducerWidget::on_aspectNumSpinBox_editingFinished()
{
    if (m_tempProducer) {
        double new_sar = double(ui->aspectNumSpinBox->value() * m_tempProducer->get_int("meta.media.height")) /
            double(ui->aspectDenSpinBox->value() * m_tempProducer->get_int("meta.media.width"));
        double sar = m_tempProducer->get_double("meta.media.sample_aspect_num");
        if (m_tempProducer->get_double("meta.media.sample_aspect_den") > 0)
            sar /= m_tempProducer->get_double("meta.media.sample_aspect_den");
        if (m_tempProducer->get("force_aspect_ratio") || !qFuzzyCompare(new_sar,sar)) {
            m_tempProducer->set("force_aspect_ratio", QString::number(new_sar).toLatin1().constData());
            m_tempProducer->set(kAspectRatioNumerator, ui->aspectNumSpinBox->text().toLatin1().constData());
            m_tempProducer->set(kAspectRatioDenominator, ui->aspectDenSpinBox->text().toLatin1().constData());
        }
//        emit producerChanged(m_producer);
//        connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onFrameDisplayed(const SharedFrame&)));
    }
}




//void AvformatProducerWidget::on_aspectDenSpinBox_valueChanged(int i)
void AvformatProducerWidget::on_aspectDenSpinBox_editingFinished()
{
//    on_aspectNumSpinBox_valueChanged(i);
    on_aspectNumSpinBox_editingFinished();
}

void AvformatProducerWidget::on_durationSpinBox_editingFinished()
{
    if (!m_tempProducer)
        return;
    if (ui->durationSpinBox->value() == m_tempProducer->get_length())
        return;
    recreateTempProducer();
}


void AvformatProducerWidget::on_speedSpinBox_editingFinished()
{
    if (!m_tempProducer)
        return;
    m_recalcDuration = true;
    recreateTempProducer();
}

void AvformatProducerWidget::on_syncSlider_valueChanged(int value)
{
    if (m_tempProducer)
        m_tempProducer->set("video_delay", double(value) / 1000);
}

void AvformatProducerWidget::on_actionOpenFolder_triggered()
{
    QFileInfo fi(GetFilenameFromProducer(m_producer));
    QDesktopServices::openUrl(QUrl::fromLocalFile(fi.path()));
}

void AvformatProducerWidget::on_menuButton_clicked()
{
    QMenu menu;
    if (!MLT.resource().contains("://")) // not a network stream
        menu.addAction(ui->actionOpenFolder);
    menu.addAction(ui->actionCopyFullFilePath);
    menu.addAction(ui->actionFFmpegInfo);
    menu.addAction(ui->actionFFmpegIntegrityCheck);
    menu.exec(ui->menuButton->mapToGlobal(QPoint(0, 0)));
}

void AvformatProducerWidget::on_actionCopyFullFilePath_triggered()
{
    qApp->clipboard()->setText(GetFilenameFromProducer(m_producer));
}

void AvformatProducerWidget::on_notesTextEdit_textChanged()
{
    m_tempProducer->set(kCommentProperty, ui->notesTextEdit->toPlainText().toUtf8().constData());
}

void AvformatProducerWidget::on_actionFFmpegInfo_triggered()
{
    QStringList args;
    args << "-v" << "quiet";
    args << "-print_format" << "ini";
    args << "-pretty";
    args << "-show_format" << "-show_programs" << "-show_streams";
    args << GetFilenameFromProducer(m_producer);
    AbstractJob* job = new FfprobeJob(args.last(), args);
    job->start();
}

void AvformatProducerWidget::on_actionFFmpegIntegrityCheck_triggered()
{
    QString resource = GetFilenameFromProducer(m_producer);
    QStringList args;
    args << "-xerror";
    args << "-err_detect" << "+explode";
    args << "-v" << "info";
    args << "-i" << resource;
    args << "-f" << "null" << "pipe:";
    JOBS.add(new FfmpegJob(resource, args));
}

void AvformatProducerWidget::on_okButton_clicked()
{
    //使用用tempProducer发送producerChanged消息
    if (m_producer->get_int(kMultitrackItemProperty)) {
        recreateTempProducer();
        Mlt::Controller::copyFilters(*m_producer, *m_tempProducer);
        //emit producerChanged(m_tempProducer);
    } else {
        reopen( new Mlt::Producer(m_tempProducer));
    }
    //MAIN.onPropertiesDockTriggered(false);
    emit advancedExited(true, m_tempProducer);
}


void AvformatProducerWidget::setProducer(Mlt::Producer *aProducer)
{
    delete m_tempProducer;
    delete m_producer;
    m_producer = nullptr;
    m_tempProducer = nullptr;
    if (aProducer) {
        loadPreset(*aProducer);
        m_producer = new Mlt::Producer(aProducer);
        m_tempProducer = createTempProducer(MLT.profile());

        SharedFrame frame;
        onFrameDisplayed(frame);
    }
}

Mlt::Producer * AvformatProducerWidget::createTempProducer(Mlt::Profile& profile)
{
    Mlt::Producer* p = nullptr;
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
    p->set_in_and_out(m_producer->get_in(), m_producer->get_out());
    p->set(kMultitrackItemProperty, m_producer->get(kMultitrackItemProperty));

    if (m_producer->get_int(kMultitrackItemProperty)) {
        Mlt::Controller::copyFilters(*m_producer, *p);
    }
    return p;
}

Mlt::Producer *AvformatProducerWidget::recreateTempProducer()
{
    Mlt::Producer* p = producer(MLT.profile());
    p->pass_list(*m_tempProducer, "audio_index, video_index, force_aspect_ratio,"
                 "video_delay, force_progressive, force_tff,"
                 kAspectRatioNumerator ","
                 kAspectRatioDenominator ","
                 kShotcutHashProperty);
    p->set_in_and_out(m_tempProducer->get_in(), m_tempProducer->get_out());
    p->set(kMultitrackItemProperty, m_tempProducer->get(kMultitrackItemProperty));

    if (m_tempProducer->get_int(kMultitrackItemProperty)) {
        Mlt::Controller::copyFilters(*m_tempProducer, *p);
    }
    delete m_tempProducer;
    m_tempProducer = nullptr;
    m_tempProducer = p;

    return p;
}

//void AvformatProducerWidget::closeEvent(QCloseEvent *event)
//{

//}

void AvformatProducerWidget::on_cancelButton_clicked()
{
    //MAIN.onPropertiesDockTriggered(false);
    emit advancedExited(false, nullptr);
}
