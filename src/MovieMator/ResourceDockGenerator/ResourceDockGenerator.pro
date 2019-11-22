#-------------------------------------------------
#
# Project created by QtCreator 2019-07-11T22:46:18
#
#-------------------------------------------------

CONFIG   += link_prl

QT       += widgets xml script

TARGET = ResourceDockGenerator
TEMPLATE = lib

DEFINES += RESOURCEDOCKGENERATOR_LIBRARY
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
    basedockwidget.cpp \
    textdockwidget.cpp \
    baselistview.cpp \
    baseitemdelegate.cpp \
    baseitemmodel.cpp \
    textitemmodel.cpp \
    translationhelper.cpp \
    stickerdockwidget.cpp \
    stickeritemmodel.cpp \
    filterdockwidget.cpp \
    filteritemmodel.cpp \
    recentdock/recentdockwidget.cpp \
    recentdock/recentitemmodel.cpp \
    recentdock/lineeditclear.cpp \
    audiodockwidget.cpp \
    audioitemmodel.cpp \
    helptipdialog.cpp

HEADERS += \
        resourcedockgenerator_global.h \ 
    basedockwidget.h \
    textdockwidget.h \
    baselistview.h \
    baseitemdelegate.h \
    baseitemmodel.h \
    textitemmodel.h \
    uiuserdef.h \
    translationhelper.h \
    stickerdockwidget.h \
    stickeritemmodel.h \
    filterdockwidget.h \
    filteritemmodel.h \
    recentdock/recentdockwidget.h \
    recentdock/recentitemmodel.h \
    recentdock/lineeditclear.h \
    unsortmap.h \
    audiodockwidget.h \
    audioitemmodel.h \
    helptipdialog.h

INCLUDEPATH = ../CuteLogger/include ../CommonUtil
INCLUDEPATH += ../src

debug_and_release {
    build_pass:CONFIG(debug, debug|release) {
        LIBS += -L../CuteLogger/debug -L../CommonUtil/debug
    } else {
        LIBS += -L../CuteLogger/release -L../CommonUtil/release
    }
} else {
    LIBS += -L../CuteLogger -L../CommonUtil
}
LIBS += -lLogger -lCommonUtil

mac {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/
    QMAKE_RPATHDIR += @executable_path/../Frameworks
}

win32 {
    target.path = C:\\Projects\\MovieMator
    INSTALLS += target
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include(../win32debug.pri)

FORMS += \
    basedockwidget.ui \
    helptipdialog.ui

