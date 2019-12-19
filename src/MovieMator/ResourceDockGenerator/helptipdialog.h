#ifndef HELPTIPDIALOG_H
#define HELPTIPDIALOG_H

#include <QDialog>

namespace Ui {
class HelpTipDialog;
}

class HelpTipDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpTipDialog(QWidget *parent = nullptr);
    ~HelpTipDialog();

    void show();

private slots:
    void on_pushButton_clicked();

    void on_checkBox_toggled(bool checked);

private:
    Ui::HelpTipDialog *ui;
};

#endif // HELPTIPDIALOG_H
