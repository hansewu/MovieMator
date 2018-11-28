#-------------------------------------------------
#
# Project created by QtCreator 2018-11-26T17:44:39
#
#-------------------------------------------------

QT       += widgets

TARGET = PlaylistDock
TEMPLATE = lib

DEFINES += PLAYLISTDOCK_LIBRARY

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
        playlistdock.cpp \
    models/playlistmodel.cpp \
    commands/playlistcommands.cpp \
    widgets/playlisttable.cpp

HEADERS += \
        playlistdock.h \
        playlistdock_global.h \ 
    models/playlistmodel.h \
    commands/playlistcommands.h \
    widgets/playlisttable.h

INCLUDEPATH = ../CuteLogger/include ../CommonUtil ../MltController

debug_and_release {
    build_pass:CONFIG(debug, debug|release) {
        LIBS += -L../CuteLogger/debug -L../CommonUtil/debug -L../MltController/debug
    } else {
        LIBS += -L../CuteLogger/release -L../CommonUtil/release -L../MltController/release
    }
} else {
    LIBS += -L../CuteLogger -L../CommonUtil -L../MltController
}
LIBS += -lLogger -lCommonUtil -lMltController


win32 {
#    CONFIG += windows rtti
    isEmpty(MLT_PATH) {
        message("MLT_PATH not set; using C:\\Projects\\MovieMator. You can change this with 'qmake MLT_PATH=...'")
        MLT_PATH = C:\\Projects\\MovieMator
    }
    INCLUDEPATH += $$MLT_PATH\\include\\mlt++ $$MLT_PATH\\include\\mlt
    LIBS += -L$$MLT_PATH\\lib -lmlt++ -lmlt -lopengl32
#    RC_FILE = shotcut.rc
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    playlistdock.ui
