#ifndef PROFEATUREPROMPTDIALOG_H
#define PROFEATUREPROMPTDIALOG_H

#include <QDialog>

namespace Ui {
class ProFeaturePromptDialog;
}

class ProFeaturePromptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProFeaturePromptDialog(QWidget *parent = 0);
    ~ProFeaturePromptDialog();

private slots:
    void on_laterButton_clicked();

    void on_upgradeNowButton_clicked();

private:
    Ui::ProFeaturePromptDialog *ui;
};

#endif // PROFEATUREPROMPTDIALOG_H
