#include "helptipdialog.h"
#include "ui_helptipdialog.h"
#include "settings.h"
#include <qdesktopservices.h>
#include <qurl.h>

HelpTipDialog::HelpTipDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpTipDialog)
{
    ui->setupUi(this);
}

HelpTipDialog::~HelpTipDialog()
{
    delete ui;
}

void HelpTipDialog::show()
{
#ifdef Q_OS_WIN
     this->setWindowModality(QmlApplication::dialogModality());
 #endif

    this->exec();
}

void HelpTipDialog::on_pushButton_clicked()
{
    //显示提示网页
    QDesktopServices::openUrl(QUrl("http://jiandashi.com/jiandashi_tietu.html"));

    this->accept();
}

void HelpTipDialog::on_checkBox_toggled(bool checked)
{
    Settings.setStickerDockShowPrompt(!checked);
}
