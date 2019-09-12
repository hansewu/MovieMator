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

#include <QColorDialog>
#include <QFileInfo>
#include "colorproducerwidget.h"
#include "ui_colorproducerwidget.h"
#include "shotcut_mlt_properties.h"
#include "util.h"

ColorProducerWidget::ColorProducerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorProducerWidget)
{
    ui->setupUi(this);
//    Util::setColorsToHighlight(ui->label_2);
    ui->preset->saveDefaultPreset(*getPreset());
    ui->preset->loadPresets();

    ui->label_2->setStyleSheet("background-color:rgb(46,46,46);");
    ui->line->setStyleSheet("color:black;");
    ui->colorButton->setStyleSheet("QPushButton{"
                                   "border-radius:3px;border:1px solid;border-color:black;"
                                   "background-color:rgb(100,100,100);color:rgb(225,225,225);}"
                                   );
}

ColorProducerWidget::~ColorProducerWidget()
{
    delete ui;
}

void ColorProducerWidget::on_colorButton_clicked()
{
    QColorDialog dialog;
    dialog.setOption(QColorDialog::ShowAlphaChannel);
    if (dialog.exec() == QDialog::Accepted) {
        ui->colorLabel->setText(QString().sprintf("#%02X%02X%02X%02X",
                                                  qAlpha(dialog.currentColor().rgba()),
                                                  qRed(dialog.currentColor().rgba()),
                                                  qGreen(dialog.currentColor().rgba()),
                                                  qBlue(dialog.currentColor().rgba())
                                                  ));
        ui->colorLabel->setStyleSheet(QString("color: %1; background-color: %2")
                                      .arg((dialog.currentColor().value() < 150)? "white":"black")
                                      .arg(dialog.currentColor().name()));
        if (m_producer) {
            m_producer->set("resource", ui->colorLabel->text().toLatin1().constData());
            m_producer->set(kShotcutCaptionProperty, ui->colorLabel->text().toLatin1().constData());
            m_producer->set(kShotcutDetailProperty, ui->colorLabel->text().toLatin1().constData());
            emit producerChanged(m_producer);
        }
    }
}

Mlt::Producer* ColorProducerWidget::producer(Mlt::Profile& profile)
{
    Mlt::Producer* p = new Mlt::Producer(profile, "color:");
    p->set("resource", ui->colorLabel->text().toLatin1().constData());
    p->set(kShotcutCaptionProperty, ui->colorLabel->text().toLatin1().constData());
    p->set(kShotcutDetailProperty, ui->colorLabel->text().toLatin1().constData());
    return p;
}

Mlt::Properties* ColorProducerWidget::getPreset() const
{
    Mlt::Properties* p = new Mlt::Properties;
    p->set("resource", ui->colorLabel->text().toLatin1().constData());
    return p;
}

void ColorProducerWidget::loadPreset(Mlt::Properties& p)
{
    QColor color(QFileInfo(p.get("resource")).baseName());
    ui->colorLabel->setText(QString().sprintf("#%02X%02X%02X%02X",
                                              qAlpha(color.rgba()),
                                              qRed(color.rgba()),
                                              qGreen(color.rgba()),
                                              qBlue(color.rgba())
                                              ));
    ui->colorLabel->setStyleSheet(QString("color: %1; background-color: %2")
        .arg((color.value() < 150)? "white":"black")
        .arg(color.name()));
    if (m_producer) {
        m_producer->set("resource", ui->colorLabel->text().toLatin1().constData());
        m_producer->set(kShotcutCaptionProperty, ui->colorLabel->text().toLatin1().constData());
        m_producer->set(kShotcutDetailProperty, ui->colorLabel->text().toLatin1().constData());
        emit producerChanged(m_producer);
    }
}

void ColorProducerWidget::on_preset_selected(void* p)
{
//    Mlt::Properties* properties = static_cast<Mlt::Properties*>(p);
//    loadPreset(*properties);
//    delete properties;
    QMetaObject::invokeMethod(this, "onPresetLoaded", Qt::QueuedConnection, Q_ARG(void*, p));
}

void ColorProducerWidget::onPresetLoaded(void *pPreset)
{
    Mlt::Properties* pMltProperties = static_cast<Mlt::Properties*>(pPreset);
    loadPreset(*pMltProperties);
    delete pMltProperties;
}

void ColorProducerWidget::on_preset_saveClicked()
{
    ui->preset->savePreset(getPreset());
}
