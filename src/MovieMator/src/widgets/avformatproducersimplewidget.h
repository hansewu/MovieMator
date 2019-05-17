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

    void on_speedSpinBox_editingFinished();

    void on_advanced_clicked();

    void on_resetButton_clicked();

    void on_okButton_clicked();

private:
    Ui::AvformatProducerSimpleWidget *ui;
    int m_defaultDuration;
    bool m_recalcDuration;

    Mlt::Producer *m_tempProducer;

    void reopen(Mlt::Producer* p);
    Mlt::Producer * createProducer(Mlt::Profile&, Mlt::Producer*, double speed);  // 创建一个m_producer的拷贝
    void adjustProducer(Mlt::Producer *newProducer);
    void adjustInAndOutOfFilter(Mlt::Producer *newProducer, int newLength, double speedRatio);
};

#endif // AVFORMATPRODUCERSIMPLEWIDGET_H
