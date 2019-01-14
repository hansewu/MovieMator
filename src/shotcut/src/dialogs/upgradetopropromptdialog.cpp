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
#include "upgradetopropromptdialog.h"
#include "ui_upgradetopropromptdialog.h"
#include "mainwindow.h"

UpgradeToProPromptDialog::UpgradeToProPromptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpgradeToProPromptDialog)
{
    ui->setupUi(this);
#if defined(Q_OS_WIN)
    ui->textEdit->setHtml(tr("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" "
                             "\"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                             "<html>"
                             "<head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head>"
                             "<body style=\"font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
                             "<p style=\" margin-top:18px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                             "<span style=\" font-family:'.Helvetica Neue DeskInterface'; font-size:13pt; font-weight:600;\">Tips</span></p>\n"
                             "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                             "<span style=\" font-family:'.Helvetica Neue DeskInterface'; font-size:13pt;\">"
                             "MovieMator Video Editor is designed to create short videos quickly. Users can export videos up to 5 minutes."
                             " </span></p>\n"
                             "</body>"
                             "</html>" ));
    ui->getButton->setHidden(true);
    ui->laterButton->setText("OK");
    ui->laterButton->setGeometry(ui->getButton->geometry());
#endif
}

UpgradeToProPromptDialog::~UpgradeToProPromptDialog()
{
    delete ui;
}

void UpgradeToProPromptDialog::on_laterButton_clicked()
{
    close();
}

void UpgradeToProPromptDialog::on_getButton_clicked()
{
    MAIN.upgradeToProVersion();
    close();
}
