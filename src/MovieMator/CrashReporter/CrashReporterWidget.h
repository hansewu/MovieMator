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


#ifndef CRASHREPORTERWIDGET_H
#define CRASHREPORTERWIDGET_H

#include <QtWidgets/QWidget>

#include "HttpRequestWorker.h"

namespace Ui {
    class CrashReporterWidget;
}

class CrashReporterWidget : public QWidget
{
        Q_OBJECT

    public:
        // 构造函数
        explicit CrashReporterWidget(QWidget *parent = nullptr);
        // 析构函数
        ~CrashReporterWidget();

        // 设定 path为 .dmp文件的路径
        void setDmpPath(const QString& path);
        // 设定 path为 .log文件的路径
        void setLogPath(const QString& path);
        // 设定要发送到服务器的内容
        void setInputData();

    protected:
        void changeEvent(QEvent *e);

    private slots:
        // 响应 btnSendRport的槽函数
        void on_btnSendReport_clicked();
        // 响应 btnClose的槽函数
        void on_btnClose_clicked();
        // worker执行完毕的处理函数
        void handle_result(HttpRequestWorker *worker);
        // 自动发送报告的槽函数
        void on_autoSendReport_triggered();

    private:
        // 界面 ui
        Ui::CrashReporterWidget *ui;

        // .dmp文件路径
        QString _dmpPath;
        // .log文件路径
        QString _logPath;

        // 发送内容到服务器的请求
        HttpRequestInput *m_input;
};

#endif // CRASHREPORTERWIDGET_H
