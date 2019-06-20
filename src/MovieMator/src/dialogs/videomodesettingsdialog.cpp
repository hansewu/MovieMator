#include "videomodesettingsdialog.h"
#include "ui_videomodesettingsdialog.h"
#include "customprofiledialog.h"
#include "mainwindow.h"
#include "settings.h"

#include <qmlapplication.h>
#include <qdebug.h>
#include <qstandardpaths.h>
#include <qmessagebox.h>

const QMap<QString, QString> profileNameMap = {
    {"", "Automatic"},
    {"atsc_720p_50", "HD 720p 50 fps"},
    {"atsc_720p_5994", "HD 720p 59.94 fps"},
    {"atsc_720p_60", "HD 720p 60 fps"},
    {"atsc_1080i_50", "HD 1080i 25 fps"},
    {"atsc_1080i_5994", "HD 1080i 29.97 fps"},
    {"atsc_1080p_2398", "HD 1080p 23.98 fps"},
    {"atsc_1080p_24", "HD 1080p 24 fps"},
    {"atsc_1080p_25", "HD 1080p 25 fps"},
    {"atsc_1080p_2997", "HD 1080p 29.97 fps"},
    {"atsc_1080p_30", "HD 1080p 30 fps"},
    {"dv_ntsc", "SD NTSC"},
    {"dv_pal", "SD PAL"},
    {"uhd_2160p_2398", "UHD 2160p 23.98 fps"},
    {"uhd_2160p_24", "UHD 2160p 24 fps"},
    {"uhd_2160p_25", "UHD 2160p 25 fps"},
    {"uhd_2160p_2997", "UHD 2160p 29.97 fps"},
    {"uhd_2160p_30", "UHD 2160p 30 fps"},
    {"uhd_2160p_50", "UHD 2160p 50 fps"},
    {"uhd_2160p_5994", "UHD 2160p 59.94 fps"},
    {"uhd_2160p_60", "UHD 2160p 60 fps"},

    //Non-Broadcast
    {"atsc_720p_2398", "HD 720p 23.98 fps"},
    {"atsc_720p_24", "HD 720p 24 fps"},
    {"atsc_720p_25", "HD 720p 25 fps"},
    {"atsc_720p_2997", "HD 720p 29.97 fps"},
    {"atsc_720p_30", "HD 720p 30 fps"},
    {"atsc_1080i_60", "HD 1080i 60 fps"},
    {"hdv_1080_50i", "HDV 1080i 25 fps"},
    {"hdv_1080_60i", "HDV 1080i 29.97 fps"},
    {"hdv_1080_25p", "HDV 1080p 25 fps"},
    {"dv_ntsc_wide", "DVD Widescreen NTSC"},
    {"dv_pal_wide", "DVD Widescreen PAL"},
    {"square_ntsc", "640x480 4:3 NTSC"},
    {"square_pal", "768x576 4:3 PAL"},
    {"square_ntsc_wide", "854x480 16:9 NTSC"},
    {"square_pal_wide", "1024x576 16:9 PAL"},
};


VideoModeSettingsDialog::VideoModeSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VideoModeSettingsDialog)
{
    ui->setupUi(this);

    //设置视频模式combobox
    setUpVideoModeComboBox();
}

VideoModeSettingsDialog::~VideoModeSettingsDialog()
{
    delete ui;
}

void VideoModeSettingsDialog::setupCurrentProfileUI() {
    QString currentPrefileFileName = Settings.playerProfile();
    QList<QString> keys =  profileNameMap.keys();
    if (keys.contains(currentPrefileFileName)) {
        ui->videoModeComboBox->setCurrentText(profileNameMap.value(currentPrefileFileName));
    } else {
        ui->videoModeComboBox->setCurrentText(currentPrefileFileName);
    }
}

void VideoModeSettingsDialog::setUpVideoModeComboBox() {
    int itemCount = 0;
    //添加固定菜单项
    QMap<QString, QString>::const_iterator mapIter;
    for (mapIter = profileNameMap.constBegin(); mapIter != profileNameMap.constEnd(); ++mapIter) {
        ui->videoModeComboBox->addItem(mapIter.value(), mapIter.key());
        itemCount += 1;
    }

    //添加自定义菜单项
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
    if (dir.cd("profiles")) {
        QStringList profiles = dir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
        foreach (QString name, profiles) {
            ui->videoModeComboBox->addItem(name, name);
            itemCount += 1;
        }
    }

    //添加custom项
    ui->videoModeComboBox->addItem("Custom", "mm_custom");

    //设置当前profile
    setupCurrentProfileUI();
}

void VideoModeSettingsDialog::on_videoModeComboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == "Custom") {
        CustomProfileDialog dialog(this);
        dialog.setWindowModality(QmlApplication::dialogModality());
        if (dialog.exec() == QDialog::Accepted) {
            QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
            if (dir.cd("profiles")) {
                QString name = dialog.profileName();
                int itemCount = ui->videoModeComboBox->count();
                ui->videoModeComboBox->setCurrentIndex(itemCount - 2);
                ui->videoModeComboBox->insertItem(itemCount - 1, name, name);
                ui->videoModeComboBox->setCurrentText(name);
            }
        }
    }
}

void VideoModeSettingsDialog::on_buttonBox_accepted()
{
    QString profileName = ui->videoModeComboBox->currentData().toString();
    MAIN.changeProfile(profileName);
}

void VideoModeSettingsDialog::on_buttonBox_rejected()
{
    setupCurrentProfileUI();
}
