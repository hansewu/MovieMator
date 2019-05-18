/****************************************************************************
**
** Copyright (C) 2007-2015 Speedovation
** Contact: Speedovation Lab (info@speedovation.com)
**
** KineticWing IDE CrashHandler
** http:// kineticwing.com
** This file is part of the core classes of the KiWi Editor (IDE)
**
** Author: Yash Bhardwaj
** License : Apache License 2.0
**
** All rights are reserved.
*/

#include "CrashReporterWidget.h"
#include "ui_CrashReporterWidget.h"

#include <QNetworkReply>
#include <QProcess>
#include <QDesktopWidget>

#include <QTimer>

//#define CRASHREPORTER_SUBMIT_URL "http://192.168.1.181/upload/upload.php"

CrashReporterWidget::CrashReporterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrashReporterWidget)
{
    ui->setupUi(this);

    ui->info->clear();
    ui->btnClose->hide();

    ui->restartCheckBox->setChecked(false);
    ui->restartCheckBox->setVisible(false);

    setWindowTitle(tr("CrashReporter"));

    //Move the dialog away from the center
    setGeometry(0,0,this->width(),this->height());
    int i = 0;
    if ( qApp->desktop()->screenCount() > 1 )
    {
        i = qApp->desktop()->screenNumber(this) ;
    }

    //Put the dialog in the screen center
    const QRect screen = qApp->desktop()->screenGeometry(i);
    move( screen.center() - rect().center() );

    setWindowFlags(Qt::WindowStaysOnTopHint);   // 总在最前
    setWindowFlags(windowFlags() &~ Qt::WindowMinMaxButtonsHint);   //禁止最大和最小化

    m_input = new HttpRequestInput(CRASHREPORTER_SUBMIT_URL, "POST");

    // 2s后自动发送
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_autoSendReport_triggered()));
    timer->setSingleShot(true);     // 只触发一次
    timer->start(2000);
}

CrashReporterWidget::~CrashReporterWidget()
{
    delete m_input;
    m_input = nullptr;
    delete ui;
}

void CrashReporterWidget::setDmpPath(const QString& path)
{
    _dmpPath = path;
}

void CrashReporterWidget::setLogPath(const QString& path)
{
    _logPath = path;
}

void CrashReporterWidget::setInputData()
{
    Q_ASSERT(m_input);
    if(!m_input)
    {
        m_input = new HttpRequestInput(CRASHREPORTER_SUBMIT_URL, "POST");
    }
#ifdef Q_OS_WIN
    m_input->add_var("os-name", "windows");
#else
    m_input->add_var("os-name", "mac");
#endif
    QStringList list = _dmpPath.split("-");
    QString verStr = list[list.size()-1];
    verStr = verStr.left(verStr.indexOf(".dmp"));
    m_input->add_var("version", verStr);
//    m_input->add_var("version", MOVIEMATOR_VERSION);
    m_input->add_var("email", ui->email->text());
    m_input->add_var("desc", ui->desc->toPlainText());
    // "dmp-file"和 "log-file"供 php中 $_FILES使用的
    // 发送 dump文件
    m_input->add_file("dmp-file", _dmpPath, NULL, "binary/octet-stream");
    // 发送 log文件
    m_input->add_file("log-file", _logPath, NULL, "binary/octet-stream");
}

void CrashReporterWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void CrashReporterWidget::on_btnSendReport_clicked()
{
    delete m_input;
    m_input = new HttpRequestInput(CRASHREPORTER_SUBMIT_URL, "POST");
    setInputData();

    HttpRequestWorker *worker = new HttpRequestWorker(this);
    connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(handle_result(HttpRequestWorker*)));
    worker->execute(m_input);

    ui->info->setText(tr("Sending..."));
    ui->btnSendReport->setEnabled(false);
}

void CrashReporterWidget::on_btnClose_clicked()
{
    qApp->quit();
}

void CrashReporterWidget::handle_result(HttpRequestWorker *worker) {
    if (worker->error_type == QNetworkReply::NoError) {
        // communication was successful
        ui->info->setText(tr("Send successfully."));
        ui->btnSendReport->hide();
        ui->btnClose->show();
        ui->desc->appendPlainText(worker->response);
        // 重启 Moviemator
        if(ui->restartCheckBox->isChecked())
        {
            QProcess p;

        #if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
            QString path = QCoreApplication::applicationDirPath()+"/MovieMator Video Editor Pro";
            if(!p.startDetached(path, QStringList()))
            {
                path = QCoreApplication::applicationDirPath()+"/MovieMator Video Editor";
                p.startDetached(path, QStringList());
            }
        #elif defined(Q_OS_WIN)
            p.startDetached("MovieMator.exe");
        #endif

            qApp->quit();
        }
    }
    else {
        // an error occurred
        ui->info->setText(tr("Sending failed."));
        ui->btnSendReport->setEnabled(true);
        ui->desc->appendPlainText(worker->response);
    }
}

void CrashReporterWidget::on_autoSendReport_triggered()
{
    setInputData();

    HttpRequestWorker *worker = new HttpRequestWorker(this);
    connect(worker, &HttpRequestWorker::on_execution_finished, this, [=](HttpRequestWorker *w){
        if (w->error_type != QNetworkReply::NoError) {
            w->execute(m_input);    // 发送不成功就再发送
        }
    });
    worker->execute(m_input);
}

