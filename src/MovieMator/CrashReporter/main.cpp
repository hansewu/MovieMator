#include <QtCore/qdebug.h>
#include <QApplication>
#include <QWidget>

#include "CrashReporterWidget.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

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




