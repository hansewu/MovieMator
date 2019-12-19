#include "aspectratiosettingsdialog.h"
#include "ui_aspectratiosettingsdialog.h"

#include <qevent.h>
#include "mainwindow.h"
#include "settings.h"

AspectRatioSettingsDialog::AspectRatioSettingsDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::CustomizeWindowHint),
    ui(new Ui::AspectRatioSettingsDialog)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/moviemator-pro-logo-64.png"));
#if defined(MOVIEMATOR_FREE) && !defined(SHARE_VERSION)
    setWindowIcon(QIcon(":/icons/moviemator-logo-64.png"));
#endif

    addItemsToAspectRationComboBox();

    setDefaultItemOfAspectRationComboBox();
}

AspectRatioSettingsDialog::~AspectRatioSettingsDialog()
{
    delete ui;
}

void AspectRatioSettingsDialog::setDefaultItemOfAspectRationComboBox()
{
    QString strCurrentPrefileFileName   = Settings.playerProfile();
    int i = 0;
    for (; i < ui->aspectRationComboBox->count(); i++)
    {
        if (strCurrentPrefileFileName == ui->aspectRationComboBox->itemData(i).toString())
        {
            ui->aspectRationComboBox->setCurrentIndex(i);
            break;
        }
    }

    if (i == ui->aspectRationComboBox->count())
    {
        ui->aspectRationComboBox->setCurrentIndex(0);
    }
}

void AspectRatioSettingsDialog::addItemsToAspectRationComboBox()
{
    ui->aspectRationComboBox->addItem(tr("16:9(Landscape)"), "atsc_720p_24");
    ui->aspectRationComboBox->addItem(tr("9:16(Portrait)"), "atsc_1280X720_24_9_16");
    ui->aspectRationComboBox->addItem(tr("4:3(Classic)"), "dv_pal");
    ui->aspectRationComboBox->addItem(tr("3:4(E-store)"), "atsc_750X1000_24_3_4");
    ui->aspectRationComboBox->addItem(tr("1:1(Square)"), "atsc_800X800_24_1_1");
}

void AspectRatioSettingsDialog::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        break;
    default:
        QDialog::keyPressEvent(event);
    }
}

void AspectRatioSettingsDialog::on_buttonBox_accepted()
{
    int currentItemIndex = ui->aspectRationComboBox->currentIndex();
    QString strProfileName = ui->aspectRationComboBox->itemData(currentItemIndex).toString();
    Settings.setPlayerProfile(strProfileName);
    MAIN.setProfile(strProfileName);
}
