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
#include "registrationdialog.h"
#include "ui_registrationdialog.h"
#include "../eccregister/transport_ecc_register.h"
#include "registrationchecker.h"
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QDesktopServices>
#include "mainwindow.h"


RegistrationDialog::RegistrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegistrationDialog)
{
    ui->setupUi(this);

#if MOVIEMATOR_FREE
    ui->buynowButton->setText(tr("Get License Code"));
#endif
}

RegistrationDialog::~RegistrationDialog()
{
    delete ui;
}

void RegistrationDialog::on_buynowButton_clicked()
{
    MAIN.onBuynowButton_clicked();
    close();
}

void RegistrationDialog::on_activateButton_clicked()
{
    //检测邮箱是否合法
    //license 非空
    QString mail = ui->emailLineEdit->text();
    QString license  = ui->licenseCodeTextEdit->toPlainText();

    if (checkEmailAddr(mail) == false || license.isEmpty())
    {
        QMessageBox::warning(this, "Invalid License", "Registration failed!\nPlease make sure the Email and License Code are correct, any question please contact us at market@effectmatrix.com.", QMessageBox::Ok);
        return;
    }

    RegistrationType type = Registration.registerWithLicense(mail, license);
    if (type == Registration_None)
    {
        QMessageBox::warning(this, "Invalid License", "Registration failed!\nPlease make sure the Email and License Code are correct, any question please contact us at market@effectmatrix.com.", QMessageBox::Ok);
//        shakeWindow();
        return;
    }

    QMessageBox::information(this, "Registration Success", "Congratulations! You have successfully registered MovieMator.", QMessageBox::Ok);
    MAIN.updateWindowTitle();
    close();
}

void RegistrationDialog::on_emailLineEdit_editingFinished()
{
    //检测邮箱地址
//    QString emailAddr = ui->emailLineEdit->text();
//    if (!checkEmailAddr(emailAddr))
//    {
//        QMessageBox::warning(this, "Invalid Email", "Please input your own Email address to register the app.", QMessageBox::Ok);
//    }
}

bool RegistrationDialog::checkEmailAddr(QString &email)
{
    //检测邮箱地址
    QString emailFilterString("^.+@([A-Za-z0-9-]+\\.)+[A-Za-z]{2}[A-Za-z]*$");
    QRegExp emailRegExp(emailFilterString);
    bool match = emailRegExp.exactMatch(email);
    return match;
}

void RegistrationDialog::shakeWindow()
{
    QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "pos");
    pAnimation->setDuration(300);
    pAnimation->setLoopCount(1);
    pAnimation->setKeyValueAt(0, QPoint(pos().x() - 3, pos().y()));
    pAnimation->setKeyValueAt(0.1, QPoint(pos().x() + 6, pos().y()));
    pAnimation->setKeyValueAt(0.2, QPoint(pos().x() - 6, pos().y()));
    pAnimation->setKeyValueAt(0.3, QPoint(pos().x() + 6, pos().y()));
    pAnimation->setKeyValueAt(0.4, QPoint(pos().x() - 6, pos().y()));
    pAnimation->setKeyValueAt(0.5, QPoint(pos().x() + 6, pos().y()));
    pAnimation->setKeyValueAt(0.6, QPoint(pos().x() - 6, pos().y()));
    pAnimation->setKeyValueAt(0.7, QPoint(pos().x() + 6, pos().y()));
    pAnimation->setKeyValueAt(0.8, QPoint(pos().x() - 6, pos().y()));
    pAnimation->setKeyValueAt(0.9, QPoint(pos().x() + 6, pos().y()));
    pAnimation->setKeyValueAt(1, QPoint(pos().x() - 3, pos().y()));
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);

}
