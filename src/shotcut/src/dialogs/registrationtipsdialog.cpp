#include "registrationtipsdialog.h"
#include "ui_registrationtipsdialog.h"
#include "mainwindow.h"

RegistrationTipsDialog::RegistrationTipsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegistrationTipsDialog)
{
    ui->setupUi(this);
}

RegistrationTipsDialog::~RegistrationTipsDialog()
{
    delete ui;
}

void RegistrationTipsDialog::on_registerButton_clicked()
{
    MAIN.on_activateButton_clicked();
    this->done(100);
}

void RegistrationTipsDialog::on_buynowButton_clicked()
{
    MAIN.on_buynowButton_clicked();
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
