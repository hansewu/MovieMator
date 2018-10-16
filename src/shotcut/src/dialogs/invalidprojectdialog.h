#ifndef INVALIDPROJECTDIALOG_H
#define INVALIDPROJECTDIALOG_H

#include <QWidget>

namespace Ui {
class InvalidProjectDialog;
}

class InvalidProjectDialog : public QWidget
{
    Q_OBJECT

public:
    explicit InvalidProjectDialog(QWidget *parent = 0);
    ~InvalidProjectDialog();

private:
    Ui::InvalidProjectDialog *ui;

private slots:
    void on_okButton_clicked();
};

#endif // INVALIDPROJECTDIALOG_H
