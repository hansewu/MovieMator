#ifndef PACKPROJECTWINDOW_H
#define PACKPROJECTWINDOW_H

#include <QThread>
#include <QProgressDialog>

class PackProjectThread : public QThread
{
    Q_OBJECT
public:
    explicit PackProjectThread(void *pUserData, const QString &strZipFile,
                               const QStringList &fileList, QObject *pParent = nullptr);
    ~PackProjectThread();

protected:
    void run();

signals:
    void compressSucceed();  //压缩完成信号
    void compressFailed();  // 压缩失败信号

private:
    void*       m_pUserData;
    QString     m_strZipFile;
    QStringList m_strListFiles;
};

class PackProjectWindow : public QProgressDialog
{
    Q_OBJECT

public:
    explicit PackProjectWindow(QWidget *pParent = nullptr);
    ~PackProjectWindow();

    void packProject();
    qint64 getCompressed() { return m_nCompressed;}
    void setCompressed(qint64 nValue) { m_nCompressed = nValue; }

private slots:
    void onProgressDialogCanceled();
    void onCompressSucceed();
    void onCompressFailed();
    void onTimeOut();

protected:
    void closeEvent(QCloseEvent *pEvent) override;

private:
    qint64 m_nCompressed;
    qint64 m_nMaxCompressed;

    QString m_strMmpFile;
    QString m_strZipFile;

    QTimer *m_pTimer;
};

#endif // PACKPROJECTWINDOW_H
