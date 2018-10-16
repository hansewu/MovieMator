#ifndef REGISTRATIONDIALOG_H
#define REGISTRATIONDIALOG_H

#include <QDialog>

namespace Ui {
class RegistrationDialog;
}

class RegistrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationDialog(QWidget *parent = 0);
    ~RegistrationDialog();

private slots:
    void on_buynowButton_clicked();

    void on_activateButton_clicked();

    void on_emailLineEdit_editingFinished();

    bool checkEmailAddr(QString &email);

    void shakeWindow();

private:
    Ui::RegistrationDialog *ui;
};

#endif // REGISTRATIONDIALOG_H
