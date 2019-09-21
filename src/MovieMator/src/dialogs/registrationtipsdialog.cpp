/*
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
#include "registrationtipsdialog.h"
#include "ui_registrationtipsdialog.h"
#include "mainwindow.h"

RegistrationTipsDialog::RegistrationTipsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegistrationTipsDialog)
{
    ui->setupUi(this);

    // 国区版本
#ifdef MOVIEMATOR_FREE
    ui->textEdit->setHtml(QApplication::translate("RegistrationTipsDialog",
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                                "<span style=\" font-family:'.Helvetica Neue DeskInterface'; font-size:13pt;\">Moviemator Video Editor is free for personal usage.</span></p>\n"
        "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0p"
                                "x;\"><span style=\" font-family:'.Helvetica Neue DeskInterface'; font-size:13pt;\">For commercial purpose or if necessary, pls register to remove the watermark.</span></p>\n"
        "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'.Helvetica Neue DeskInterface'; font-size:13pt;\"><br /></p></body></html>", nullptr));

    // 隐藏购买按钮
    ui->buynowButton->setVisible(false);
    ui->buynowButton->setEnabled(false);

    // 保留注册按钮，并改为 移除水印
    ui->registerButton->setText(tr("Remove Watermark"));
    ui->registerButton->setIcon(QIcon());

    // 更改按钮顺序：继续 移除水印
    ui->horizontalLayout->removeItem(ui->horizontalSpacer);
    ui->horizontalLayout->removeWidget(ui->registerButton);
    ui->horizontalLayout->removeWidget(ui->evaluateButton);

    ui->horizontalLayout->addItem(ui->horizontalSpacer);
    ui->horizontalLayout->addWidget(ui->evaluateButton);
    ui->horizontalLayout->addWidget(ui->registerButton);

    // 设置移除水印为默认按钮
    ui->registerButton->setAutoDefault(true);
#endif
}

RegistrationTipsDialog::~RegistrationTipsDialog()
{
    delete ui;
}

void RegistrationTipsDialog::on_registerButton_clicked()
{
    MAIN.onActivateButton_clicked();
    this->done(100);
}

void RegistrationTipsDialog::on_buynowButton_clicked()
{
    MAIN.onBuynowButton_clicked();
    this->done(101);
}

void RegistrationTipsDialog::on_evaluateButton_clicked()
{
    this->done(102);
}

//void RegistrationTipsDialog::close()
//{
//    this->done(102);
//}
