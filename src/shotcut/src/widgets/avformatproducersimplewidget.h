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

#ifndef AVFORMATPRODUCERSIMPLEWIDGET_H
#define AVFORMATPRODUCERSIMPLEWIDGET_H

#include <QWidget>
#include "abstractproducerwidget.h"
#include "sharedframe.h"

namespace Ui {
    class AvformatProducerSimpleWidget;
}

class AvformatProducerSimpleWidget : public QWidget, public AbstractProducerWidget
{
    Q_OBJECT

public:
    explicit AvformatProducerSimpleWidget(QWidget *parent = 0);
    ~AvformatProducerSimpleWidget();

    // AbstractProducerWidget overrides
    Mlt::Producer* producer(Mlt::Profile&);

    void setProducer(Mlt::Producer *);
signals:
    void producerChanged(Mlt::Producer*);
    void producerReopened();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onFrameDisplayed(const SharedFrame&);

    void on_resetButton_clicked();

    void on_okButton_clicked();


    void on_durationSpinBox_editingFinished();

    void on_speedSpinBox_editingFinished();

    void on_actionOpenFolder_triggered();

    void on_actionCopyFullFilePath_triggered();

    void on_advanced_clicked();

private:
    Ui::AvformatProducerSimpleWidget *ui;
    int m_defaultDuration;
    bool m_recalcDuration;

    Mlt::Producer *m_tempProducer;

    void reopen(Mlt::Producer* p);
    void recreateProducer();
    Mlt::Producer * createTempProducer(Mlt::Profile&);  // 创建一个m_producer的拷贝
    Mlt::Producer * recreateTempProducer();  //创建一个新的producer
};

#endif // AVFORMATPRODUCERSIMPLEWIDGET_H
