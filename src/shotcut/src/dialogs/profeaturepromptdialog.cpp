#include "profeaturepromptdialog.h"
#include "ui_profeaturepromptdialog.h"
#include "mainwindow.h"

ProFeaturePromptDialog::ProFeaturePromptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProFeaturePromptDialog)
{
    ui->setupUi(this);
}

ProFeaturePromptDialog::~ProFeaturePromptDialog()
{
    delete ui;
}

void ProFeaturePromptDialog::on_laterButton_clicked()
{
    close();
}

void ProFeaturePromptDialog::on_upgradeNowButton_clicked()
{
    MAIN.upgradeToProVersion();
    close();
}
