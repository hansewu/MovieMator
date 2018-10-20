#ifndef MMSPLASHSCREEN_H
#define MMSPLASHSCREEN_H

#include <QSplashScreen>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QThread>


class MMSplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    explicit MMSplashScreen(const QPixmap &pixmap = QPixmap());
    ~MMSplashScreen();
signals:
    void progressValueChanged(int);

public slots:

private slots:
    void updateProgress();
    void onProgressValueChanged(int value);

private:
    QProgressBar *progressBar;
    QLabel *infoLabel;

    QTimer *progressTimer;
    int progressValue;

    QThread *progressThread;

};

#endif // MMSPLASHSCREEN_H
