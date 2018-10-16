#ifndef REGISTRATIONTIPSDIALOG_H
#define REGISTRATIONTIPSDIALOG_H

#include <QDialog>

namespace Ui {
class RegistrationTipsDialog;
}

class RegistrationTipsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationTipsDialog(QWidget *parent = 0);
    ~RegistrationTipsDialog();

private slots:

    void on_registerButton_clicked();

    void on_buynowButton_clicked();

    void on_evaluateButton_clicked();


private:
    Ui::RegistrationTipsDialog *ui;
};

#endif // REGISTRATIONTIPSDIALOG_H
