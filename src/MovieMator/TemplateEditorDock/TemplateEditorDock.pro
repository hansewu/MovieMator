#-------------------------------------------------
#
# Project created by QtCreator 2018-11-26T14:32:35
#
#-------------------------------------------------
CONFIG   += link_prl

QT       += widgets qml quick quickwidgets
QT       -= gui

TARGET = TemplateEditorDock
TEMPLATE = lib

DEFINES += TEMPLATEEDITORDOCK_LIBRARY
win32:DEFINES += QT_STATIC

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
    templateeditordock.cpp \
    thumbnailprovider.cpp

HEADERS += \
        templateeditordock_global.h \
    templateeditordockinterface.h \
    templateeditordock.h \
    thumbnailprovider.h

INCLUDEPATH = ../CuteLogger/include ../CommonUtil  ../QmlUtilities
INCLUDEPATH += ../src

debug_and_release {
    build_pass:CONFIG(debug, debug|release) {
        LIBS += -L../CuteLogger/debug -L../CommonUtil/debug
        LIBS += -L../QmlUtilities/debug
    } else {
        LIBS += -L../CuteLogger/release -L../CommonUtil/release
        LIBS += -L../QmlUtilities/release
    }
} else {
    LIBS += -L../CuteLogger -L../CommonUtil
     LIBS += -L../QmlUtilities
}
LIBS += -lLogger -lCommonUtil
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
