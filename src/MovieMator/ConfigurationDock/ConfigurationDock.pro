#-------------------------------------------------
#
# Project created by QtCreator 2018-12-04T10:41:25
#
#-------------------------------------------------

QT       += widgets qml quick quickwidgets

QT       -= gui

TARGET = ConfigurationDock
TEMPLATE = lib

DEFINES += CONFIGURATIONDOCK_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        configurationdock.cpp

HEADERS += \
        configurationdock.h \
        configurationdock_global.h 

INCLUDEPATH = ../QmlUtilities

debug_and_release {
    build_pass:CONFIG(debug, debug|release) {
        LIBS += -L../QmlUtilities/debug
    } else {
        LIBS += -L../QmlUtilities/release
    }
} else {
    LIBS += -L../QmlUtilities
}
LIBS += -lQmlUtilities


win32 {
    target.path = C:\\Projects\\MovieMator
    INSTALLS += target
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

mac {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/
    QMAKE_RPATHDIR += @executable_path/qt_lib/lib
}

include(../win32debug.pri)
