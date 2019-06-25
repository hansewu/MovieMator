#ifndef VIDEOMODESETTINGSDIALOG_H
#define VIDEOMODESETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class VideoModeSettingsDialog;
}

class VideoModeSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VideoModeSettingsDialog(QWidget *parent = nullptr);
    ~VideoModeSettingsDialog();

private slots:
    void on_videoModeComboBox_currentTextChanged(const QString &arg1);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    void setUpVideoModeComboBox();
    void setupCurrentProfileUI();

private:
    Ui::VideoModeSettingsDialog *ui;
};

#endif // VIDEOMODESETTINGSDIALOG_H
