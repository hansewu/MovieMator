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
#include "avformatproducerwidget.h"
/*
// Try fast create m_tempProducer with thread
#include <QObject>
#include <QDebug>
#include <QThread>
#include <pthread.h>

typedef struct
{
    Mlt::Producer *aProducer;
    double speed;
}THREAD_PARA;

class CCreateProducerThread;
static void* thread_func( void *arg );

class CCreateProducerThread
{
public:
        CCreateProducerThread()
        {
            m_created_producer = nullptr;
            //pthread_t m_th_createProducer;
            m_threadPara.aProducer = nullptr;
            m_threadPara.speed = 1.0;
            m_iThreadState = 0; //0  1 runnning 2 Exited
        };
        virtual ~CCreateProducerThread()
        {

        }

        void start_createProducer(Mlt::Producer *aProducer, double speed)
        {
            if(m_iThreadState == 1)
                return;

            //get_created_producer();// pthread_join exit
            //pthread_t th;
            //THREAD_PARA *arg = new THREAD_PARA;
            m_threadPara.aProducer = aProducer;
            m_threadPara.speed = speed;
            m_iThreadState = 1;

            int ret = pthread_create( &m_th_createProducer, NULL, thread_func, this );
            if( ret != 0 )
            {
                printf( "Create thread error!\n");
                return ;
            }
        }

        void createProducerInThread()
        {
            m_iThreadState = 1;
            createProducer(MLT.profile(), m_threadPara.aProducer, m_threadPara.speed);
            m_iThreadState = 2;
        }

        void createProducer(Mlt::Profile& profile, Mlt::Producer *producer, double speed)
        {
            m_created_producer = nullptr;
            qDebug()<<"receive the execute signal---------------------------------";
            qDebug()<<"     current thread ID:"<<QThread::currentThreadId();

            Mlt::Producer* p = nullptr;
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
            p->set_in_and_out(producer->get_in(), producer->get_out());
            p->set(kMultitrackItemProperty, producer->get(kMultitrackItemProperty));

            m_created_producer = p;
            qDebug()<<"      finish the work and sent the resultReady signal\n";
            //emit resultReady(created_producer);           //emit啥事也不干，是给程序员看的，表示发出信号发出信号
            //return p;
        }

        Mlt::Producer *get_created_producer()
        {
          //  if(s_iThreadState == 0) return nullptr;
            int *thread_ret = NULL;
            while(m_iThreadState != 2)
            {
                //qApp->processEvents();
                QThread::msleep(10);
            }
            //pthread_join(m_th_createProducer, (void**)&thread_ret);

            return m_created_producer;
        }
private:
        Mlt::Producer * m_created_producer;
        pthread_t m_th_createProducer;
        THREAD_PARA m_threadPara;
        volatile int m_iThreadState; //0  1 runnning 2 Exited

};

static void* thread_func( void *arg )
{
    CCreateProducerThread *pThread = (CCreateProducerThread *)arg;

    //s_iThreadState = 1; //running
    pThread->createProducerInThread();

    int rv = 44;
    pthread_exit((void*)rv);
    return arg;
}

std::map<AvformatProducerSimpleWidget *, CCreateProducerThread *> s_mapCreateProducerThread;
//static CCreateProducerThread *s_pCreateProducerThread;
*/
AvformatProducerSimpleWidget::AvformatProducerSimpleWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AvformatProducerSimpleWidget)
    , m_defaultDuration(-1)
    , m_recalcDuration(true)
    , m_tempProducer(nullptr)
    , m_propertiesDock(nullptr)
{
    //s_mapCreateProducerThread[this] = new CCreateProducerThread();
    ui->setupUi(this);
    setStyleSheet("background-color:rgb(26, 30, 34);");
//    Util::setColorsToHighlight(ui->filenameLabel);
    connect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, SLOT(onFrameDisplayed(const SharedFrame&)));

    ui->speedIcon->setPixmap(QPixmap(":/icons/light/32x32/speed_icon.png"));

    ui->filenameLabel->setStyleSheet("color:rgb(185,185,185);");
    QFont font = ui->filenameLabel->font();
    font.setPointSize(14);
    ui->filenameLabel->setFont(font);

    ui->line->setStyleSheet("color:black;");
    ui->line_2->setStyleSheet("color:black;");
    QString buttonStyle = "QPushButton{"
                          "border-radius:3px;border:1px solid;border-color:black;"
                          "background-color:rgb(82,82,82);color:rgb(185,185,185);}";
    ui->resetButton->setStyleSheet(buttonStyle);
    ui->okButton->setStyleSheet(buttonStyle);
    ui->advanced->setStyleSheet(buttonStyle);
    QString spinBoxStyle = "QSpinBox{padding-left:4px;background-color:rgb(82,82,82);color:rgb(185,185,185);border:1px solid black;border-radius:4px;}";
    spinBoxStyle.append("QSpinBox::up-button{subcontrol-origin:border;subcontrol-position:top right;width:18px;border:none;}");
    spinBoxStyle.append("QSpinBox::up-arrow{image:url(:/icons/light/8x8/up.png);width:8px;height:8px;}");
    spinBoxStyle.append("QSpinBox::down-button{subcontrol-origin:border;subcontrol-position:bottom right;width:16px;border:none;border-top-width:0;}");
    spinBoxStyle.append("QSpinBox::down-arrow{image:url(:/icons/light/8x8/down.png);width:8px;height:8px;}");
    ui->startPointSpinBox->setStyleSheet(spinBoxStyle);
    ui->endPointSpinBox->setStyleSheet(spinBoxStyle);
    QString doubleSpinBoxStyle = spinBoxStyle.replace("QSpinBox", "QDoubleSpinBox");
    ui->speedSpinBox->setStyleSheet(doubleSpinBoxStyle);

    // 去除 spinBox的上下箭头按钮，使之像一个普通的不可编辑文本框
    ui->startPointSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->endPointSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    // 去除 spinBox的边框
    ui->startPointSpinBox->setStyleSheet("QSpinBox{background-color:transparent; border:none;}");
    ui->endPointSpinBox->setStyleSheet("QSpinBox{background-color:transparent; border:none;}");

    ui->startPointLabel->setStyleSheet("color:rgb(185,185,185);");
//    QFont startPointLabelFont = ui->startPointLabel->font();
//    startPointLabelFont.setPointSize(13);
//    ui->startPointLabel->setFont(startPointLabelFont);

    ui->endPointLabel->setStyleSheet("color:rgb(185,185,185);");
//    QFont endPointLabelFont = ui->startPointLabel->font();
//    endPointLabelFont.setPointSize(13);
//    ui->endPointLabel->setFont(endPointLabelFont);

    ui->speedLabel->setStyleSheet("color:rgb(185,185,185);");
//    QFont speedLabelFont = ui->startPointLabel->font();
//    speedLabelFont.setPointSize(13);
//    ui->speedLabel->setFont(speedLabelFont);

    m_bStatus       = false;
    m_dOpacityValue = 1.0;
    m_timer         = new QTimer(this);
    m_opacityEffect = new QGraphicsOpacityEffect(ui->okButton);

    m_timer->setInterval(150);
    m_opacityEffect->setOpacity(m_dOpacityValue);
    ui->okButton->setGraphicsEffect(m_opacityEffect);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(okButtonFlash()));
}

AvformatProducerSimpleWidget::~AvformatProducerSimpleWidget()
{

    //m_tempProducer = s_mapCreateProducerThread[this]->get_created_producer();//wait for thread exit
    if (m_tempProducer)
        delete m_tempProducer;
    m_tempProducer = nullptr;

    if (m_producer)
        delete  m_producer;
    m_producer = nullptr;

    if (m_timer)
    {
        m_timer->stop();
        delete m_timer;
    }

    if (m_opacityEffect)
        delete m_opacityEffect;

    delete ui;

    //delete s_mapCreateProducerThread[this];
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
        //if(!m_tempProducer)  m_tempProducer = s_mapCreateProducerThread[this]->get_created_producer();
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
        setProducer(nullptr);
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
    //if(!m_tempProducer)  m_tempProducer = s_mapCreateProducerThread[this]->get_created_producer();
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
    disconnect(MLT.videoWidget(), SIGNAL(frameDisplayed(const SharedFrame&)), this, nullptr);
}


void AvformatProducerSimpleWidget::on_speedSpinBox_editingFinished()
{
    if (!m_tempProducer)
        return;
    m_recalcDuration = true;

    m_timer->start();
}


void AvformatProducerSimpleWidget::setProducer(Mlt::Producer *aProducer)
{
    if(m_tempProducer)  //m_tempProducer = get_created_producer();
        delete m_tempProducer;
    delete m_producer;
    m_producer = nullptr;//保存原producer数据
    m_tempProducer = nullptr;//接收改变的临时producer
    if (aProducer) {
        loadPreset(*aProducer);
        double speed = GetSpeedFromProducer(aProducer);
        m_producer = new Mlt::Producer(aProducer);//createProducer(MLT.profile(), aProducer, speed);

        m_tempProducer = createProducer(MLT.profile(), aProducer, speed);
        //s_mapCreateProducerThread[this]->start_createProducer(m_producer, speed);

    }
}

//创建新的producer
Mlt::Producer * AvformatProducerSimpleWidget::createProducer(Mlt::Profile& profile, Mlt::Producer *producer, double speed)
{
    Mlt::Producer* p = nullptr;
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
    p->set_in_and_out(producer->get_in(), producer->get_out());
    p->set(kMultitrackItemProperty, producer->get(kMultitrackItemProperty));
    return p;
}

void AvformatProducerSimpleWidget::adjustProducer(Mlt::Producer* newProducer) {
    Q_ASSERT(newProducer);
    Q_ASSERT(newProducer->is_valid());
    //if(!m_tempProducer)  m_tempProducer = s_mapCreateProducerThread[this]->get_created_producer();
    Q_ASSERT(m_tempProducer);
    Q_ASSERT(m_tempProducer->is_valid());
    if (!newProducer || !newProducer->is_valid() || !m_tempProducer || !m_tempProducer->is_valid()) {
        return;
    }

    double oldSpeed = GetSpeedFromProducer(m_tempProducer);
    double newSpeed = GetSpeedFromProducer(newProducer);
    double speedRatio = oldSpeed / newSpeed;

    Mlt::Producer& oldProducerWithParent = m_tempProducer->parent();
    Q_ASSERT(oldProducerWithParent.is_valid());
    if (!oldProducerWithParent.is_valid()) {
        return;
    }

    int oldLength = oldProducerWithParent.get_length();
    int newLength = qRound(oldLength * speedRatio);

    newProducer->set("length", newLength);
    newProducer->set_in_and_out(qMin(qRound(oldProducerWithParent.get_in() * speedRatio), newLength - 1),
                                qMin(qRound(oldProducerWithParent.get_out() * speedRatio), newLength - 1));

    adjustInAndOutOfFilter(newProducer, newLength, speedRatio);//更新新的Producer上滤镜的入点、出点

    //TODO：更新关键帧信息

    //TODO：更新转场信息
}

void AvformatProducerSimpleWidget::adjustInAndOutOfFilter(Mlt::Producer* newProducer, int newLength, double speedRatio) {
    Q_ASSERT(newProducer);
    Q_ASSERT(newProducer->is_valid());

    if (!newProducer || !newProducer->is_valid()) {
        return;
    }

    int n = newProducer->filter_count();
    for (int j = 0; j < n; j++) {
        QScopedPointer<Mlt::Filter> filter(newProducer->filter(j));
        Q_ASSERT(filter);
        Q_ASSERT(filter->is_valid());
        if (filter && filter->is_valid() && !filter->get_int("_loader")) {
            //if(!m_tempProducer)  m_tempProducer = s_mapCreateProducerThread[this]->get_created_producer();
            int in = qMin(qRound(m_tempProducer->get_in() * speedRatio), newLength - 1);
            int out = qMin(qRound(m_tempProducer->get_out() * speedRatio), newLength - 1);
            filter->set_in_and_out(in, out);
        }
    }
}

void AvformatProducerSimpleWidget::on_advanced_clicked()
{
    QScrollArea* advancedScrollArea = nullptr;
    if(m_propertiesDock)
    {
        advancedScrollArea = qobject_cast<QScrollArea*>(m_propertiesDock->widget()); //视频高级属性
        if(advancedScrollArea)
        {
            if (advancedScrollArea->widget())
                advancedScrollArea->widget()->deleteLater();
        }
        delete m_propertiesDock;
    }

    AvformatProducerWidget *advancedW = new AvformatProducerWidget(this);

    m_propertiesDock = new QDockWidget(tr("Properties"));
    m_propertiesDock->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    m_propertiesDock->setWindowModality(Qt::WindowModal);
    m_propertiesDock->setObjectName("propertiesDock");
    m_propertiesDock->setMinimumWidth(410);
    m_propertiesDock->setStyleSheet(".QWidget {background-color: rgb(53,53,53)}");
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    m_propertiesDock->setWidget(scroll);
    //QWidget* advancedW = nullptr; //视频高级属性
    advancedScrollArea = qobject_cast<QScrollArea*>(m_propertiesDock->widget()); //视频高级属性
    connect(advancedW, SIGNAL(advancedExited(bool, Mlt::Producer *)), this, SLOT(on_advanced_exited(bool, Mlt::Producer *)));


    if (advancedW)
    {
        dynamic_cast<AbstractProducerWidget*>(advancedW)->setProducer(m_producer);
        /*if (-1 != advancedW->metaObject()->indexOfSignal("producerChanged(Mlt::Producer*)"))
        {
            connect(advancedW, SIGNAL(producerChanged(Mlt::Producer*)), SLOT(onProducerChanged()));
            connect(advancedW, SIGNAL(producerChanged(Mlt::Producer*)), m_filterController, SLOT(setProducer(Mlt::Producer*)));
            if (producer->get_int(kMultitrackItemProperty))
                connect(advancedW, SIGNAL(producerChanged(Mlt::Producer*)), m_timelineDock, SLOT(onProducerChanged(Mlt::Producer*)));
        }*/
        advancedScrollArea->setWidget(advancedW);
        //onProducerChanged();
    }

    m_propertiesDock->show();
    m_propertiesDock->raise();
    //MAIN.onPropertiesDockTriggered(true);
}

void AvformatProducerSimpleWidget::on_advanced_exited(bool bCheck, Mlt::Producer* changed)
{
    if(bCheck)
    {
        m_tempProducer->pass_list(*changed, "audio_index, video_index, force_aspect_ratio,"
                     "video_delay, force_progressive, force_tff,"
                     kAspectRatioNumerator ","
                     kAspectRatioDenominator ","
                     kShotcutHashProperty);
        m_tempProducer->set_in_and_out(changed->get_in(), changed->get_out());
        m_tempProducer->set(kMultitrackItemProperty, changed->get(kMultitrackItemProperty));

        if (changed->get_int(kMultitrackItemProperty)) {
            Mlt::Controller::copyFilters(*changed, *m_tempProducer);
        }
        m_propertiesDock->hide();
        on_okButton_clicked();
    }
    else
        m_propertiesDock->hide();
}

void AvformatProducerSimpleWidget::on_okButton_clicked()
{
    double speed = ui->speedSpinBox->value();
    //if(!m_tempProducer)  m_tempProducer = s_mapCreateProducerThread[this]->get_created_producer();
    Mlt::Producer *producer = createProducer(MLT.profile(), m_tempProducer, speed);
//    adjustProducer(producer);//调节Producer属性（包括Producer入点出点，滤镜入点出点，转场，关键帧）

    //发送producerChanged消息
    if (producer->get_int(kMultitrackItemProperty)) {
        Mlt::Controller::copyFilters(*m_producer, *producer);
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

    stopTimer();
}

Mlt::Producer* AvformatProducerSimpleWidget::producer(Mlt::Profile& profile)
{
    Q_UNUSED(profile)
    return nullptr;
}

void AvformatProducerSimpleWidget::okButtonFlash()
{
    ui->okButton->setStyleSheet("QPushButton{"
                                "border-radius:3px;border:1px solid;border-color:black;"
                                "background-color:rgb(192,72,44);color:rgb(225,225,225);}");
    if(m_bStatus)
    {
        m_dOpacityValue -= 0.2;
    }
    else
    {
        m_dOpacityValue += 0.2;
    }

    if(m_dOpacityValue > 0.9)
    {
        m_bStatus = true;
    }
    else if(m_dOpacityValue < 0.5)
    {
        m_bStatus = false;
    }

    m_opacityEffect->setOpacity(m_dOpacityValue);
}

void AvformatProducerSimpleWidget::stopTimer()
{
    ui->okButton->setStyleSheet("QPushButton{"
                                "border-radius:3px;border:1px solid;border-color:black;"
                                "background-color:rgb(100,100,100);color:rgb(225,225,225);}");
    m_timer->stop();

    m_dOpacityValue = 1.0;
    m_opacityEffect->setOpacity(m_dOpacityValue);
}
