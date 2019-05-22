#-------------------------------------------------
#
# Project created by QtCreator 2019-01-18T11:25:11
#
#-------------------------------------------------
CONFIG   += link_prl

QT       += widgets qml quick gui-private quick-private
QT       += quickwidgets
TARGET = FilterDock
TEMPLATE = lib

DEFINES += FILTERDOCK_LIBRARY
win32:DEFINES += QT_STATIC


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../src

SOURCES += \
    filterdock.cpp

HEADERS += \
    filterdock.h \
    filterdock_global.h \
    filterdockinterface.h

INCLUDEPATH = ../QmlUtilities
INCLUDEPATH += ../src

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

mac {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/
}

win32 {
    target.path = C:\\Projects\\MovieMator
    INSTALLS += target
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    filterdock.ui


include(../win32debug.pri)
