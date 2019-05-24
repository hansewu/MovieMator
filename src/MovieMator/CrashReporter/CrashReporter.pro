#****************************************************************************
#*
#* Copyright (C) 2010 Speedovation
#* Contact: Speedovation Lab (info@speedovation.com)
#* Author : Yash
#* License : Apache License 2.0
#*
#* KineticWing IDE CrashHandler
#* This file is part of the core classes of the KiWi Editor (IDE)
#*
#* Author: Yash Bhardwaj
#*
#* All rights are reserved.
#****************************************************************************/


QT += core widgets gui sql network uitools concurrent

TARGET = CrashReporter
TEMPLATE = app

win32:DEFINES += QT_STATIC
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += CRASHREPORTER_SUBMIT_URL=\\\"http://192.168.1.181/upload/upload.php\\\"

mac {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/

    LIBS += "-framework Cocoa"

    QMAKE_LFLAGS += -Wl,/usr/lib/libcrypto.0.9.8.dylib
#    INCLUDEPATH += /System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7
#    LIBS += -F/System/Library/Frameworks -framework Python
    QMAKE_RPATHDIR += @executable_path/qt_lib/lib
}

win32 {
    target.path = C:\\Projects\\MovieMator
    INSTALLS += target
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

macx{
    cache()
    CONFIG -= app_bundle
}

#CONFIG += c++11
QMAKE_CXXFLAGS+= -std=c++11
QMAKE_LFLAGS +=  -std=c++11

SOURCES += \
    CrashReporterWidget.cpp \
    HttpRequestWorker.cpp \
    main.cpp

HEADERS  += \
    CrashReporterWidget.h \
    HttpRequestWorker.h \

FORMS += \
    CrashReporterWidget.ui


RESOURCES += \
    ../icons/resources.qrc
