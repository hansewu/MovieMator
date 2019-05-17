#include <QtCore/qdebug.h>
#include <QApplication>
#include <QWidget>
#include <QTranslator>
#include <QDir>

#include "CrashReporterWidget.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    QDir dir = QCoreApplication::applicationDirPath();

#ifdef Q_OS_WIN
    dir.cd("share");
    dir.cd("translations");
#else
    dir.cdUp();
    dir.cd("Resources");
    dir.cd("translations");
#endif

    QTranslator translator;
    translator.load(QLocale(), "cr", "_", dir.absolutePath());
    a.installTranslator(&translator);

    CrashReporterWidget w;

    if ( a.arguments().size() != 3 )
    {
        qDebug() << "Invalid Startup Problem" << ".dmp file path is required as argument" ;
        return 101;
    }

    w.setDmpPath(a.arguments()[1]);             // *.dmp
    w.setLogPath(a.arguments()[2].replace("*", " "));   //moviemator-log.txt

    w.show();

    return a.exec();
}




