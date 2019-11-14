#ifndef ASPECTRATIOSETTINGSDIALOG_H
#define ASPECTRATIOSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class AspectRatioSettingsDialog;
}

class AspectRatioSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AspectRatioSettingsDialog(QWidget *parent = nullptr);
    ~AspectRatioSettingsDialog();

protected:
    void keyPressEvent(QKeyEvent *);

private slots:
    void on_buttonBox_accepted();

private:
    void addItemsToAspectRationComboBox();
    void setDefaultItemOfAspectRationComboBox();

private:
    Ui::AspectRatioSettingsDialog *ui;
};

#endif // ASPECTRATIOSETTINGSDIALOG_H
