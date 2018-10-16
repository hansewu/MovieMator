#include "invalidprojectdialog.h"
#include "ui_invalidprojectdialog.h"

InvalidProjectDialog::InvalidProjectDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InvalidProjectDialog)
{
    ui->setupUi(this);
}

InvalidProjectDialog::~InvalidProjectDialog()
{
    delete ui;
}

void InvalidProjectDialog::on_okButton_clicked()
{
    close();
}
