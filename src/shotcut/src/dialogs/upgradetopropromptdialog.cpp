#include "upgradetopropromptdialog.h"
#include "ui_upgradetopropromptdialog.h"
#include "mainwindow.h"

UpgradeToProPromptDialog::UpgradeToProPromptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpgradeToProPromptDialog)
{
    ui->setupUi(this);
//#if defined(Q_OS_WIN)
//    ui->textEdit->setText(tr(""));
//    ui->getButton->setHidden(true);
//    ui->laterButton->setText("OK");
//#endif
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
