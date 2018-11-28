#-------------------------------------------------
#
# Project created by QtCreator 2018-11-28T11:28:57
#
#-------------------------------------------------

QT       += widgets qml quick

TARGET = QmlUtilities
TEMPLATE = lib

DEFINES += QMLUTILITIES_LIBRARY

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
        qmlutilities.cpp \
    colorpickeritem.cpp \
    colorwheelitem.cpp \
    filesavedialog.cpp \
    qmlapplication.cpp \
    qmlfile.cpp \
    qmlhtmleditor.cpp \
    qmlmetadata.cpp \
    qmlprofile.cpp \
    qmltextmetadata.cpp \
    qmlview.cpp \
    timelineitems.cpp

HEADERS += \
        qmlutilities.h \
        qmlutilities_global.h \ 
    colorpickeritem.h \
    colorwheelitem.h \
    filesavedialog.h \
    qmlapplication.h \
    qmlfile.h \
    qmlhtmleditor.h \
    qmlmetadata.h \
    qmlprofile.h \
    qmltextmetadata.h \
    qmlview.h \
    timelineitems.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
