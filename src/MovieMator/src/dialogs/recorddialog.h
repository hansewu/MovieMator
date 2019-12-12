#ifndef RECORDDIALOG_H
#define RECORDDIALOG_H

#include <QDialog>

#include "jobs/meltjob.h"

#include <MltProducer.h>
#include <QTimer>
#include <QTime>

namespace Ui {
class RecordDialog;
}

class RecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordDialog(QWidget *parent = nullptr);
    ~RecordDialog();

private slots:
    void on_startRecordButton_clicked();

    void on_stopRecordButton_clicked();

    void onFinished(AbstractJob*, bool isSuccess);

    void startRecord();

    void countdown_callback();

    void fun_clicked();

private:
    void createAudioProducer();

private:
    Ui::RecordDialog *ui;

    QString m_outputFilePath;
    QScopedPointer<MeltJob> m_pMeltJob;
    QScopedPointer<Mlt::Producer> m_pAudioProducer;

    QTimer* m_pCountdownTimer;
    int m_nStartValueOfCountdown;

    QTime m_durationTime;
    QTimer* m_pTimer;
};

#endif // RECORDDIALOG_H
