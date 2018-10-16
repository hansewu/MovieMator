#include "upgradetopropromptdialog.h"
#include "ui_upgradetopropromptdialog.h"
#include "mainwindow.h"

UpgradeToProPromptDialog::UpgradeToProPromptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpgradeToProPromptDialog)
{
    ui->setupUi(this);
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
