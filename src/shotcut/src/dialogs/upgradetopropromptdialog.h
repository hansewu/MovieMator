#ifndef UPGRADETOPROPROMPTDIALOG_H
#define UPGRADETOPROPROMPTDIALOG_H

#include <QDialog>

namespace Ui {
class UpgradeToProPromptDialog;
}

class UpgradeToProPromptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpgradeToProPromptDialog(QWidget *parent = 0);
    ~UpgradeToProPromptDialog();

private slots:
    void on_laterButton_clicked();

    void on_getButton_clicked();

private:
    Ui::UpgradeToProPromptDialog *ui;
};

#endif // UPGRADETOPROPROMPTDIALOG_H
