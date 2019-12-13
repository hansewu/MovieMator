#include "recorddialog.h"
#include "ui_recorddialog.h"
#include "Logger.h"
#include "mltcontroller.h"
#include "settings.h"
#include "jobs/encodejob.h"
#include "shotcut_mlt_properties.h"

#include <QCamera>
#include <QAudioDeviceInfo>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDomComment>
#include <QThread>

static void collectProperties(QDomElement& node, int realtime)
{
    Mlt::Properties preset("consumer/avformat/Audio/WAV");
    if (preset.is_valid())
    {
        if (realtime)
        {
            preset.set("real_time", realtime);
        }

        for (int i = 0; i < preset.count(); i++)
        {
            if (preset.get_name(i) && strcmp(preset.get_name(i), ""))
            {
                node.setAttribute(preset.get_name(i), preset.get(i));
            }
            node.setAttribute(preset.get_name(i), preset.get(i));
        }
    }
}

static MeltJob* createMeltJob(Mlt::Service* service, const QString& target, int realtime, int pass = 0)
{
    Q_ASSERT(service);
    Q_ASSERT(service->is_valid());
    Q_ASSERT(!target.isEmpty());

    // get temp filename
    QTemporaryFile tmp;

    tmp.open();
    tmp.close();

    MLT.saveXML(tmp.fileName(), service);

    // parse xml
    QFile f1(tmp.fileName());
    f1.open(QIODevice::ReadOnly);
    QDomDocument dom(tmp.fileName());
    dom.setContent(&f1);
    f1.close();


    // add consumer element
    QDomElement consumerNode = dom.createElement("consumer");
    QDomNodeList profiles = dom.elementsByTagName("profile");
    if (profiles.isEmpty())
    {
        dom.documentElement().insertAfter(consumerNode, dom.documentElement());
    }
    else
    {
        dom.documentElement().insertAfter(consumerNode, profiles.at(profiles.length() - 1));
    }
    consumerNode.setAttribute("mlt_service", "avformat");
    consumerNode.setAttribute("target", target);

    collectProperties(consumerNode, realtime);

    if (pass == 1 || pass == 2) {
        consumerNode.setAttribute("pass", pass);
        consumerNode.setAttribute("passlogfile", target + "_2pass.log");
    } if (pass == 1) {
        consumerNode.setAttribute("fastfirstpass", 1);
        consumerNode.removeAttribute("acodec");
        consumerNode.setAttribute("an", 1);
    } else {
        consumerNode.removeAttribute("fastfirstpass");
    }

    QFile::remove(tmp.fileName());

    return new EncodeJob(target, dom.toString(2));
}

RecordDialog::RecordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecordDialog),
    m_outputFilePath(),
    m_pMeltJob(nullptr),
    m_pAudioProducer(nullptr)
{
    ui->setupUi(this);
    ui->startRecordButton->setStyleSheet("QPushButton{"
                                        "border-radius:3px;border:1px solid;border-color:black;"
                                        "background-color:rgb(82,82,82);color:rgb(185,185,185);}");
    ui->stopRecordButton->setStyleSheet("QPushButton{"
                                        "border-radius:3px;border:1px solid;border-color:black;"
                                        "background-color:rgb(82,82,82);color:rgb(185,185,185);}");

    ui->maskLabel->setVisible(false);

    ui->audioInputComboBox->setStyleSheet("QComboBox {\
                                  background-color:rgb(82,82,82); \
                                  color:rgb(185,185,185); \
                                  border: 1px solid black;\
                                  border-radius: 4px;\
                                  }"
                                  "QComboBox::drop-down { \
                                  subcontrol-origin: padding; \
                                  subcontrol-position: top right;\
                                  width: 15px;\
                                  border-left-width: 1px;\
                                  border-left-color: rgb(82,82,82);\
                                  border-left-style: solid;\
                                  border-top-right-radius: 4px; \
                                  border-bottom-right-radius: 4px;\
                                  }"
                                  "QComboBox::down-arrow {\
                                  image: url(:/icons/light/8x8/down.png);\
                                  }"
                                  "QComboBox::donw-arrow:on {top:1px;left:1px;}"
                                  "QComboBox QAbstractItemView {border: none;}"
                                  "QComboBox QAbstractItemView {background:rgb(82,82,82); color:rgb(185,185,185); border: none;}");


    //获取可用的音频设备
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
        ui->audioInputComboBox->addItem(deviceInfo.deviceName());
    }

    if (ui->audioInputComboBox->count() > 0)
    {
        ui->audioInputComboBox->setCurrentIndex(0);
    }                    

    m_nStartValueOfCountdown = 3;
    ui->maskLabel->setText(QString::number(m_nStartValueOfCountdown));

    m_pCountdownTimer = new QTimer(this);
    connect(m_pCountdownTimer, SIGNAL(timeout()), this, SLOT(countdown_callback()));


    m_durationTime = QTime(0, 0, 0, 0);
    ui->durationLabel->setText(m_durationTime.toString("hh:mm:ss:zzz"));

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateDuration_callback()));
}

void RecordDialog::countdown_callback()
{
    if (m_nStartValueOfCountdown <= 0)
    {
        if (m_pCountdownTimer && m_pCountdownTimer->isActive())
        {
            m_pCountdownTimer->stop();
        }

        ui->maskLabel->setVisible(false);

        startRecord();

        return;
    }

    m_nStartValueOfCountdown -= 1;
    ui->maskLabel->setText(QString(QString::number(m_nStartValueOfCountdown)));
}

void RecordDialog::updateDuration_callback()
{
    QString tim = m_durationTime.toString("hh:mm:ss:zzz");
    m_durationTime = m_durationTime.addMSecs(10);

    ui->durationLabel->setText(tim);
}

RecordDialog::~RecordDialog()
{
    if (m_pMeltJob)
    {
        m_pMeltJob->stop();
    }

    if (m_pTimer)
    {
        if (m_pTimer->isActive())
        {
            m_pTimer->stop();
        }

        delete m_pTimer;
        m_pTimer = nullptr;
    }

    if (m_pCountdownTimer && m_pCountdownTimer->isActive())
    {
        if (m_pCountdownTimer->isActive())
        {
            m_pCountdownTimer->stop();
        }

        delete  m_pCountdownTimer;
        m_pCountdownTimer = nullptr;
    }

    m_pAudioProducer.reset();
    m_pMeltJob.reset();

    delete ui;
}


void RecordDialog::createAudioProducer()
{
    //创建profile
    Mlt::Profile profile = Mlt::Profile("atsc_720p_24");
    Q_ASSERT(profile.is_valid());
    if (!profile.is_valid())
    {
        return;
    }

     //创建producer
#ifdef Q_OS_WIN
    QByteArray resource = QString("dshow:audio=%1").arg(ui->audioInputComboBox->currentText()).toUtf8();
#else //Q_OS_MAC
    QByteArray resource = QString("avfoundation:none:%1?pixel_format=yuyv422&framerate=25&video_size=1280x720")
            .arg(ui->audioInputComboBox->currentText().replace(tr("None"), "none")).toLatin1();
#endif

    LOG_DEBUG() << resource;

    m_pAudioProducer.reset(new Mlt::Producer(profile, resource.constData()));

    //FIXME：当producer创建失败后，重试创建10次，确保捕获producer可用。目前原因还没弄清楚，在有可能创建捕获producer失败，初步推测原因是设置异常或者占用
    for (int i = 0; i < 10; i++)
    {
        if (m_pAudioProducer && m_pAudioProducer->is_valid())
        {
            break;
        }
        else
        {
            m_pAudioProducer.reset(new Mlt::Producer(profile, resource.constData()));
        }
    }
}

void RecordDialog::on_startRecordButton_clicked()
{
    //设置输出目录
    QString outputDir;
    QStringList dirList = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
    Q_ASSERT(dirList.count() > 0);
    if (dirList.count() > 0)
    {
        outputDir = dirList.first();
        outputDir.append("/untitiled.wav");
    }

    QString outputFilePath = QFileDialog::getSaveFileName(this, tr("Save File"), outputDir, tr("WAV (*.wav)"));
//    Q_ASSERT(!outputFilePath.isEmpty());
    if (outputFilePath.isEmpty())
    {
        return;
    }

    //创建捕获音频producer
    createAudioProducer();

    Q_ASSERT(m_pAudioProducer);
    Q_ASSERT(m_pAudioProducer->is_valid());

    //创建录制job
    m_pMeltJob.reset(createMeltJob(m_pAudioProducer.data(), outputFilePath, -1));

    //显示录制开始倒计时
    ui->maskLabel->setVisible(true);
    m_nStartValueOfCountdown = 3;
    ui->maskLabel->setText(QString::number(m_nStartValueOfCountdown));

    m_durationTime = QTime(0, 0, 0, 0);
    ui->durationLabel->setText(m_durationTime.toString("hh:mm:ss:zzz"));

//    开启录制倒计时定时器
    m_pCountdownTimer->start(1000);
}

void RecordDialog::startRecord()
{
    Q_ASSERT(m_pMeltJob);
    if (m_pMeltJob)
    {
        connect(m_pMeltJob.data(), SIGNAL(finished(AbstractJob*, bool)), this, SLOT(onFinished(AbstractJob*, bool)));
        m_pMeltJob->start();

        m_pTimer->start(10);

        ui->startRecordButton->setEnabled(false);
        ui->audioInputComboBox->setEnabled(false);
    }
}

void RecordDialog::onFinished(AbstractJob* job, bool isSuccess)
{
    Q_UNUSED(job)
    Q_UNUSED(isSuccess)

    if(m_pTimer && m_pTimer->isActive())
    {
        m_pTimer->stop();
    }

    m_pMeltJob.reset();
//    m_pAudioProducer.reset();

    ui->audioInputComboBox->setEnabled(true);
    ui->startRecordButton->setEnabled(true);
}

void RecordDialog::on_stopRecordButton_clicked()
{
    if (m_pMeltJob)
    {
        m_pMeltJob->stop();
    }

    if(m_pTimer && m_pTimer->isActive())
    {
        m_pTimer->stop();
    }
}
