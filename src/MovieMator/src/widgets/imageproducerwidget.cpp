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

#include "imageproducerwidget.h"
#include "ui_imageproducerwidget.h"
#include "settings.h"
#include "mainwindow.h"
#include "shotcut_mlt_properties.h"
#include "util.h"
#include <QFileInfo>

ImageProducerWidget::ImageProducerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageProducerWidget),
    m_defaultDuration(-1),
    m_tempProducer(nullptr)
{
    ui->setupUi(this);
//    Util::setColorsToHighlight(ui->filenameLabel);
    ui->cancelButton->setHidden(true);

    ui->line->setStyleSheet("color:black;");
    ui->line_2->setStyleSheet("color:black");
    ui->filenameLabel->setStyleSheet("background-color:rgb(46,46,46);");
    QString editStyle= "QLineEdit{border:1px;border-radius:3px;background-color:rgb(82,82,82);padding-left:4px;}";
    ui->heightLineEdit->setStyleSheet(editStyle);
    ui->widthLineEdit->setStyleSheet(editStyle);
    QString buttonStyle = "QPushButton{"
                          "border-radius:3px;border:1px solid;border-color:black;"
                          "background-color:rgb(100,100,100);color:rgb(225,225,225);}";
    ui->resetButton->setStyleSheet(buttonStyle);
    ui->defaultDurationButton->setStyleSheet(buttonStyle);
    ui->okButton->setStyleSheet(buttonStyle);
    ui->cancelButton->setStyleSheet(buttonStyle);
    QString spinBoxStyle = "QSpinBox{padding-left:4px;background-color:rgb(82,82,82);color:rgb(225,225,225);border:1px solid black;border-radius:4px;}";
    spinBoxStyle.append("QSpinBox::up-button{subcontrol-origin:border;subcontrol-position:top right;width:22px;border:none;}");
    spinBoxStyle.append("QSpinBox::up-arrow{image:url(:/icons/light/8x8/up.png);width:8px;height:8px;}");
    spinBoxStyle.append("QSpinBox::down-button{subcontrol-origin:border;subcontrol-position:bottom right;width:20px;border:none;border-top-width:0;}");
    spinBoxStyle.append("QSpinBox::down-arrow{image:url(:/icons/light/8x8/down.png);width:8px;height:8px;}");
    ui->aspectDenSpinBox->setStyleSheet(spinBoxStyle);
    ui->aspectNumSpinBox->setStyleSheet(spinBoxStyle);
    ui->durationSpinBox->setStyleSheet(spinBoxStyle);
    ui->repeatSpinBox->setStyleSheet(spinBoxStyle);
    ui->sequenceCheckBox->setStyleSheet("QCheckBox {spacing: 8px;}"
                                        "QCheckBox::indicator:unchecked {image: url(:/icons/light/14x14/key.png);}"
                                        "QCheckBox::indicator:checked {image: url(:/icons/light/14x14/key_on.png);}"
                                        );

}

ImageProducerWidget::~ImageProducerWidget()
{
    delete ui;
}

Mlt::Producer* ImageProducerWidget::producer(Mlt::Profile& profile)
{
    Mlt::Producer* p = new Mlt::Producer(profile, m_producer->get("resource"));
    return p;
}

void ImageProducerWidget::setProducer(Mlt::Producer* p)
{
    AbstractProducerWidget::setProducer(p);
    delete m_tempProducer;
    m_tempProducer = createTempProducer(MLT.profile());
    if (m_defaultDuration == -1)
        m_defaultDuration = m_tempProducer->get_length();
    QString s;
    if (m_tempProducer->get(kShotcutResourceProperty))
        s = QString::fromUtf8(m_tempProducer->get(kShotcutResourceProperty));
    else {
        s = QString::fromUtf8(m_tempProducer->get("resource"));
        m_tempProducer->set("ttl", 1);
    }
    ui->filenameLabel->setText(ui->filenameLabel->fontMetrics().elidedText(s, Qt::ElideLeft, width() - 40));

    ui->durationSpinBox->setValue(m_tempProducer->get_out() + 1);

    ui->widthLineEdit->setText(p->get("meta.media.width"));
    ui->heightLineEdit->setText(p->get("meta.media.height"));
    ui->aspectNumSpinBox->blockSignals(true);
    if (p->get(kAspectRatioNumerator) && p->get(kAspectRatioDenominator)) {
        ui->aspectNumSpinBox->setValue(p->get_int(kAspectRatioNumerator));
        ui->aspectDenSpinBox->setValue(p->get_int(kAspectRatioDenominator));
    }
    else {
        double sar = m_tempProducer->get_double("aspect_ratio");
        if (m_tempProducer->get("force_aspect_ratio"))
            sar = m_tempProducer->get_double("force_aspect_ratio");
        if (sar == 1.0) {
            ui->aspectNumSpinBox->setValue(1);
            ui->aspectDenSpinBox->setValue(1);
        } else {
            ui->aspectNumSpinBox->setValue(int(1000 * sar));
            ui->aspectDenSpinBox->setValue(1000);
        }
    }
    ui->aspectNumSpinBox->blockSignals(false);
    if (m_tempProducer->get_int("ttl"))
        ui->repeatSpinBox->setValue(m_tempProducer->get_int("ttl"));
    ui->sequenceCheckBox->setChecked(m_tempProducer->get_int(kShotcutSequenceProperty));
    ui->repeatSpinBox->setEnabled(m_tempProducer->get_int(kShotcutSequenceProperty));
}

void ImageProducerWidget::setOutPoint(int duration)
{
    ui->durationSpinBox->setValue(duration + 1);
}

void ImageProducerWidget::reopen(Mlt::Producer* p)
{
    int out = ui->durationSpinBox->value() - 1;
    int position = m_tempProducer->position();
    double speed = m_tempProducer->get_speed();

    if (out + 1 > p->get_length()) //只有在新duration>旧duration时，才能设置length, 导致小于当前duration时，设置失效，所以屏蔽掉
        p->set("length", out + 1);
    p->set("out", out);
    if (position > p->get_out())
        position = p->get_out();
    p->set("in", m_tempProducer->get_in());
    if (MLT.setProducer(p)) {
        setProducer(nullptr);
        return;
    }
    MLT.stop();
    setProducer(p);
    emit producerReopened();
    emit producerChanged(p);
    MLT.seek(position);
    MLT.play(speed);
}

void ImageProducerWidget::on_resetButton_clicked()
{
    const char *s = m_tempProducer->get(kShotcutResourceProperty);
    if (!s)
        s = m_tempProducer->get("resource");
    Mlt::Producer* p = new Mlt::Producer(MLT.profile(), s);
    Mlt::Controller::copyFilters(*m_tempProducer, *p);

    p->set("out", Settings.imageDuration() * MLT.profile().fps() - 1);

    setProducer(p);
//    reopen(p);
}

void ImageProducerWidget::on_aspectNumSpinBox_valueChanged(int)
{
    if (m_tempProducer) {
        double new_sar = double(ui->aspectNumSpinBox->value()) /
            double(ui->aspectDenSpinBox->value());
        double sar = m_tempProducer->get_double("aspect_ratio");
        if (m_tempProducer->get("force_aspect_ratio") || !qFuzzyCompare(new_sar,sar)) {
            m_tempProducer->set("force_aspect_ratio", QString::number(new_sar).toLatin1().constData());
            m_tempProducer->set(kAspectRatioNumerator, ui->aspectNumSpinBox->text().toLatin1().constData());
            m_tempProducer->set(kAspectRatioDenominator, ui->aspectDenSpinBox->text().toLatin1().constData());
        }
//        emit producerChanged(m_producer);
    }
}

void ImageProducerWidget::on_aspectDenSpinBox_valueChanged(int i)
{
    on_aspectNumSpinBox_valueChanged(i);
}

void ImageProducerWidget::on_durationSpinBox_editingFinished()
{

    if (!m_tempProducer)
        return;
    if (ui->durationSpinBox->value() == m_tempProducer->get_out() + 1)
        return;
//    Mlt::Producer* p = producer(MLT.profile());
//    p->pass_list(*m_producer, "force_aspect_ratio, moviemator_aspect_num, moviemator_aspect_den,"
//        kShotcutResourceProperty ", resource, ttl," kShotcutSequenceProperty);
//    reopen(p);
}

void ImageProducerWidget::on_sequenceCheckBox_clicked(bool checked)
{
    QString resource = m_tempProducer->get("resource");
    ui->repeatSpinBox->setEnabled(checked);
    if (checked && !m_tempProducer->get(kShotcutResourceProperty))
        m_tempProducer->set(kShotcutResourceProperty, resource.toUtf8().constData());
    m_tempProducer->set(kShotcutSequenceProperty, checked);
    m_tempProducer->set("ttl", ui->repeatSpinBox->value());
    if (checked) {
        QFileInfo info(resource);
        QString name(info.fileName());
        QString begin = "";
        int i = name.length();
        int count = 0;

        // find the last numeric digit
        for (; i && !name[i - 1].isDigit(); i--) {};
        // count the digits and build the begin value
        for (; i && name[i - 1].isDigit(); i--, count++)
            begin.prepend(name[i - 1]);
        if (count) {
            m_tempProducer->set("begin", begin.toLatin1().constData());
            int j = begin.toInt();
            name.replace(i, count, begin.prepend('%').append('d'));
            resource = info.path() + "/" + name;
            m_tempProducer->set("resource", resource.toUtf8().constData());

            // Count the number of consecutive files.
            MAIN.showStatusMessage(tr("Getting length of image sequence..."));
            name = info.fileName();
            name.replace(i, count, "%1");
            resource = info.path().append('/').append(name);
            for (i = j; QFile::exists(resource.arg(i, count, 10, QChar('0'))); ++i);
            i -= j;
            m_tempProducer->set("length", i);
            ui->durationSpinBox->setValue(i);
            MAIN.showStatusMessage(tr("Reloading image sequence..."));
        }
    }
    else {
        m_tempProducer->set("resource", m_producer->get(kShotcutResourceProperty));
        m_tempProducer->set("length", qRound(MLT.profile().fps() * 600));
        ui->durationSpinBox->setValue(qRound(MLT.profile().fps() * Settings.imageDuration()));
    }
//    Mlt::Producer* p = producer(MLT.profile());
//    p->pass_list(*m_producer, "force_aspect_ratio,"
//        kAspectRatioNumerator "," kAspectRatioDenominator ","
//        kShotcutResourceProperty ", resource, ttl, length," kShotcutSequenceProperty);
//    reopen(p);
}

void ImageProducerWidget::on_repeatSpinBox_editingFinished()
{
    m_tempProducer->set("ttl", ui->repeatSpinBox->value());
//    emit producerChanged(m_producer);
}

void ImageProducerWidget::on_defaultDurationButton_clicked()
{
    Settings.setImageDuration(ui->durationSpinBox->value() / MLT.profile().fps());
}


Mlt::Producer * ImageProducerWidget::createTempProducer(Mlt::Profile& profile)
{
    Q_UNUSED(profile)
    Mlt::Producer* p = nullptr;

    p = producer(MLT.profile());


    p->pass_list(*m_producer, "force_aspect_ratio,"
        kAspectRatioNumerator "," kAspectRatioDenominator ","
        kShotcutResourceProperty ", resource, ttl, length," kShotcutSequenceProperty ", in, out");

//    Mlt::Controller::copyFilters(*m_producer, *p);
    return p;
}

void ImageProducerWidget::on_okButton_clicked()
{
    Mlt::Producer* p = producer(MLT.profile());
    p->pass_list(*m_tempProducer, "force_aspect_ratio,"
    kAspectRatioNumerator "," kAspectRatioDenominator ","
    kShotcutResourceProperty ", resource, ttl, length," kShotcutSequenceProperty);
    Mlt::Controller::copyFilters(*m_producer, *p);
    reopen(p);
//    MAIN.onPropertiesDockTriggered(false);
}

void ImageProducerWidget::on_cancelButton_clicked()
{
//    MAIN.onPropertiesDockTriggered(false);
}
