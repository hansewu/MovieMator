#-------------------------------------------------
#
# Project created by QtCreator 2018-11-28T11:02:10
#
#-------------------------------------------------

QT       += widgets qml quick opengl quickwidgets

TARGET = MltController
TEMPLATE = lib

DEFINES += MLTCONTROLLER_LIBRARY

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
        mltcontroller.cpp \
    glwidget.cpp \
    sharedframe.cpp \
    qmltypes/qmlprofile.cpp

HEADERS += \
        mltcontroller.h \
        mltcontroller_global.h \ 
    glwidget.h \
    sharedframe.h \
    transportcontrol.h \
    qmltypes/qmlprofile.h


INCLUDEPATH = ../CuteLogger/include ../CommonUtil ../QmlUtilities

debug_and_release {
    build_pass:CONFIG(debug, debug|release) {
        LIBS += -L../CuteLogger/debug -L../CommonUtil/debug -L../QmlUtilities/debug
    } else {
        LIBS += -L../CuteLogger/release -L../CommonUtil/release -L../QmlUtilities/release
    }
} else {
    LIBS += -L../CuteLogger -L../CommonUtil -L../QmlUtilities
}
LIBS += -lLogger -lCommonUtil -lQmlUtilities


mac {
    # QMake from Qt 5.1.0 on OSX is messing with the environment in which it runs
    # pkg-config such that the PKG_CONFIG_PATH env var is not set.
    isEmpty(MLT_PREFIX) {
        MLT_PREFIX = $$PWD/../../../../shotcut/mlt_build/
#        count($$USER, wzq)
#        {
#            MLT_PREFIX = /Users/wzq/Desktop/data/project/2018/moviemator/libs/mlt_build/debug
#        }
    }

    INCLUDEPATH += $$MLT_PREFIX/include/mlt++
    INCLUDEPATH += $$MLT_PREFIX/include/mlt

    LIBS += -L$$MLT_PREFIX/lib -lmlt++ -lmlt
}

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
    QMAKE_RPATHDIR += @executable_path/../Frameworks
}

include(../win32debug.pri)
