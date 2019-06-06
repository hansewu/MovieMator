/****************************************************************************
**
** Copyright (C) 2007-2015 Speedovation
** Copyright (C) 2015 creativepulse.gr
** Contact: Speedovation Lab (info@speedovation.com)
**
** KineticWing IDE CrashHandler
** http:// kineticwing.com
** This file is part of the core classes of the KiWi Editor (IDE)
**
** Author: creativepulse.gr
** License : Apache License 2.0
**
** All rights are reserved.
*/



#ifndef HTTPREQUESTWORKER_H
#define HTTPREQUESTWORKER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

// 要发送的数据类型
// 根据这些类型决定使用什么请求或者发送文件还是发送变量值
enum HttpRequestVarLayout {NOT_SET, ADDRESS, URL_ENCODED, MULTIPART};

// 发送的文件
class HttpRequestInputFileElement {

    public:
        // 服务器接收的变量名
        QString variable_name;
        // 文件的路径
        QString local_filename;
        // 文件保存到服务器的名称
        QString request_filename;
        // 文件的数据类型
        QString mime_type;

};


class HttpRequestInput {

    public:
        // 请求的服务器地址
        QString url_str;
        // http请求类型：put/get/post等
        QString http_method;
        // 发送内容的类型
        HttpRequestVarLayout var_layout;
        // 保存发送到服务器的内容及标识
        QMap<QString, QString> vars;
        // 要发送的文件
        QList<HttpRequestInputFileElement> files;

        // 构造函数
        HttpRequestInput();
        // 构造函数
        HttpRequestInput(QString v_url_str, QString v_http_method);
        // 初始化成员变量
        void initialize();
        // 添加发送的内容及服务器接收的字符串变量名
        void add_var(QString key, QString value);
        // 添加要发送的文件
        void add_file(QString variable_name, QString local_filename, QString request_filename, QString mime_type);

};


class HttpRequestWorker : public QObject {
        Q_OBJECT

    public:
        // 服务器响应内容
        QByteArray response;
        // 服务器响应错误的代码
        QNetworkReply::NetworkError error_type;
        // 服务器响应错误的内容
        QString error_str;

        // 构造函数
        explicit HttpRequestWorker(QObject *parent = nullptr);

        // 将本地内容转换成 http的发送内容
        QString http_attribute_encode(QString attribute_name, QString input);
        // 发送请求到服务器
        void execute(HttpRequestInput *input);

    signals:
        // 发送请求完毕后发送的信号
        void on_execution_finished(HttpRequestWorker *worker);

    private:
        // 执行发送请求
        QNetworkAccessManager *manager;

    private slots:
        // 发送请求完毕
        void on_manager_finished(QNetworkReply *reply);

};

#endif // HTTPREQUESTWORKER_H
