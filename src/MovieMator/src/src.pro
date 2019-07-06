CONFIG   += link_prl

QT       += widgets opengl xml qml quick sql svg
QT       += multimedia quickwidgets
QT       += qml-private core-private quick-private gui-private
#QMAKE_LFLAGS +=MovieMator_Pro=1
TARGET = "MovieMator"
TEMPLATE = app
#CONFIG   += static

win32:DEFINES += QT_STATIC

SOURCES += main.cpp\
    mainwindow.cpp \
    scrubbar.cpp \
    openotherdialog.cpp \
    controllers/filtercontroller.cpp \
    widgets/plasmawidget.cpp \
    widgets/lissajouswidget.cpp \
    widgets/isingwidget.cpp \
    widgets/video4linuxwidget.cpp \
    widgets/colorproducerwidget.cpp \
    widgets/decklinkproducerwidget.cpp \
    widgets/networkproducerwidget.cpp \
    widgets/colorbarswidget.cpp \
    widgets/noisewidget.cpp \
    widgets/pulseaudiowidget.cpp \
    widgets/jackproducerwidget.cpp \
    widgets/toneproducerwidget.cpp \
    widgets/alsawidget.cpp \
    widgets/x11grabwidget.cpp \
    player.cpp \
    widgets/servicepresetwidget.cpp \
    abstractproducerwidget.cpp \
    widgets/avformatproducerwidget.cpp \
    widgets/imageproducerwidget.cpp \
    widgets/timespinbox.cpp \
    widgets/audiometerwidget.cpp \
    docks/encodedock.cpp \
    dialogs/addencodepresetdialog.cpp \
    jobqueue.cpp \
    docks/jobsdock.cpp \
    dialogs/textviewerdialog.cpp \
    dialogs/durationdialog.cpp \
    widgets/colorwheel.cpp \
    models/attachedfiltersmodel.cpp \
    models/metadatamodel.cpp \
    docks/filtersdock.cpp \
    dialogs/customprofiledialog.cpp \
    qmltypes/colorpickeritem.cpp \
    qmltypes/colorwheelitem.cpp \
    qmltypes/qmlfile.cpp \
    qmltypes/qmlfilter.cpp \
    qmltypes/qmlhtmleditor.cpp \
    qmltypes/timelineitems.cpp \
    widgets/lineeditclear.cpp \
    widgets/webvfxproducer.cpp \
    widgets/gltestwidget.cpp \
    models/multitrackmodel.cpp \
    docks/timelinedock.cpp \
    qmltypes/thumbnailprovider.cpp \
    commands/timelinecommands.cpp \
    widgets/lumamixtransition.cpp \
    autosavefile.cpp \
    widgets/directshowvideowidget.cpp \
    jobs/abstractjob.cpp \
    jobs/meltjob.cpp \
    jobs/encodejob.cpp \
    jobs/videoqualityjob.cpp \
    docks/scopedock.cpp \
    controllers/scopecontroller.cpp \
    widgets/scopes/scopewidget.cpp \
    widgets/scopes/audioloudnessscopewidget.cpp \
    widgets/scopes/audiopeakmeterscopewidget.cpp \
    widgets/scopes/audiospectrumscopewidget.cpp \
    widgets/scopes/audiowaveformscopewidget.cpp \
    widgets/scopes/videowaveformscopewidget.cpp \
    widgets/audioscale.cpp \
    commands/undohelper.cpp \
    models/audiolevelstask.cpp \
    mltxmlchecker.cpp \
    widgets/avfoundationproducerwidget.cpp \
    widgets/gdigrabwidget.cpp \
    widgets/trackpropertieswidget.cpp \
    widgets/timelinepropertieswidget.cpp \
    widgets/filterwidget.cpp \
    jobs/ffprobejob.cpp \
    jobs/ffmpegjob.cpp \
    dialogs/unlinkedfilesdialog.cpp \
    widgets/textmanagerwidget.cpp \
    qmltypes/qmltextmetadata.cpp \
    models/textmetadatamodel.cpp \
    docks/textlistdock.cpp \
    docks/resourcebuttondockwidget.cpp \
    qmltypes/filesavedialog.cpp \
    dialogs/registrationtipsdialog.cpp \
    dialogs/registrationdialog.cpp \
    registrationchecker.cpp \
    dialogs/profeaturepromptdialog.cpp \
    dialogs/upgradetopropromptdialog.cpp \
    maincontroller.cpp \
    jobs/encodetask.cpp \
    dialogs/invalidprojectdialog.cpp \
    encodetaskqueue.cpp \
    docks/encodetaskdock.cpp \
    melt/melt_main.c \
    melt/io.c \
    jobs/abstracttask.cpp \
    jobs/melttask.cpp \
    dialogs/mmsplashscreen.cpp \
    qmltypes/mmqmlutilities.cpp \
    maininterface.cpp \
    containerdock.cpp \
    widgets/avformatproducersimplewidget.cpp \
    CrashHandler/CrashHandler.cpp \
#    templateeidtor.cpp \
    commands/abstractcommand.cpp \
    dialogs/videomodesettingsdialog.cpp

HEADERS  += mainwindow.h \
    scrubbar.h \
    openotherdialog.h \
    controllers/filtercontroller.h \
    widgets/plasmawidget.h \
    abstractproducerwidget.h \
    widgets/lissajouswidget.h \
    widgets/isingwidget.h \
    widgets/video4linuxwidget.h \
    widgets/colorproducerwidget.h \
    widgets/decklinkproducerwidget.h \
    widgets/networkproducerwidget.h \
    widgets/colorbarswidget.h \
    widgets/noisewidget.h \
    widgets/pulseaudiowidget.h \
    widgets/jackproducerwidget.h \
    widgets/toneproducerwidget.h \
    widgets/alsawidget.h \
    widgets/x11grabwidget.h \
    player.h \
    widgets/servicepresetwidget.h \
    widgets/avformatproducerwidget.h \
    widgets/imageproducerwidget.h \
    widgets/timespinbox.h \
    widgets/iecscale.h \
    widgets/audiometerwidget.h \
    docks/encodedock.h \
    dialogs/addencodepresetdialog.h \
    jobqueue.h \
    docks/jobsdock.h \
    dialogs/textviewerdialog.h \
    dialogs/durationdialog.h \
    widgets/colorwheel.h \
    models/attachedfiltersmodel.h \
    models/metadatamodel.h \
    docks/filtersdock.h \
    dialogs/customprofiledialog.h \
    qmltypes/colorpickeritem.h \
    qmltypes/colorwheelitem.h \
    qmltypes/qmlfile.h \
    qmltypes/qmlfilter.h \
    qmltypes/qmlhtmleditor.h \
    qmltypes/timelineitems.h \
    widgets/lineeditclear.h \
    widgets/webvfxproducer.h \
    widgets/gltestwidget.h \
    models/multitrackmodel.h \
    docks/timelinedock.h \
    qmltypes/thumbnailprovider.h \
    commands/timelinecommands.h \
    widgets/lumamixtransition.h \
    autosavefile.h \
    widgets/directshowvideowidget.h \
    jobs/abstractjob.h \
    jobs/meltjob.h \
    jobs/encodejob.h \
    jobs/videoqualityjob.h \
    docks/scopedock.h \
    controllers/scopecontroller.h \
    widgets/scopes/scopewidget.h \
    widgets/scopes/audioloudnessscopewidget.h \
    widgets/scopes/audiopeakmeterscopewidget.h \
    widgets/scopes/audiospectrumscopewidget.h \
    widgets/scopes/audiowaveformscopewidget.h \
    widgets/scopes/videowaveformscopewidget.h \
    dataqueue.h \
    widgets/audioscale.h \
    commands/undohelper.h \
    models/audiolevelstask.h \
    mltxmlchecker.h \
    widgets/avfoundationproducerwidget.h \
    widgets/gdigrabwidget.h \
    widgets/trackpropertieswidget.h \
    widgets/timelinepropertieswidget.h \
    widgets/filterwidget.h \
    jobs/ffprobejob.h \
    jobs/ffmpegjob.h \
    dialogs/unlinkedfilesdialog.h \
    widgets/textmanagerwidget.h \
    qmltypes/qmltextmetadata.h \
    models/textmetadatamodel.h \
    docks/textlistdock.h \
    docks/resourcebuttondockwidget.h \
    iratetransport.h \
    securitybookmark/transport_security_bookmark.h \
    qmltypes/filesavedialog.h \
    eccregister/transport_ecc_register.h \
    dialogs/registrationtipsdialog.h \
    dialogs/registrationdialog.h \
    registrationchecker.h \
    dialogs/profeaturepromptdialog.h \
    dialogs/upgradetopropromptdialog.h \
    maincontroller.h \
    jobs/encodetask.h \
    dialogs/invalidprojectdialog.h \
    encodetaskqueue.h \
    docks/encodetaskdock.h \
    melt/io.h \
    jobs/abstracttask.h \
    jobs/melttask.h \
    melt/melt.h \
    dialogs/mmsplashscreen.h \
    qmltypes/mmqmlutilities.h \
    maininterface.h \
    containerdock.h \
    widgets/avformatproducersimplewidget.h \
#    templateeidtor.h \
    eccregister/CEccRegister.h \
    commands/abstractcommand.h \
    CrashHandler/CrashHandler.h \
    dialogs/videomodesettingsdialog.h

mac {
    SOURCES += securitybookmark/SecurityBookmark.mm \
                ../iRate/iRate.mm
    HEADERS += securitybookmark/SecurityBookmark.h \
                ../iRate/iRate.h
}


FORMS    += mainwindow.ui \
    openotherdialog.ui \
    widgets/plasmawidget.ui \
    widgets/lissajouswidget.ui \
    widgets/isingwidget.ui \
    widgets/video4linuxwidget.ui \
    widgets/colorproducerwidget.ui \
    widgets/decklinkproducerwidget.ui \
    widgets/networkproducerwidget.ui \
    widgets/colorbarswidget.ui \
    widgets/noisewidget.ui \
    widgets/pulseaudiowidget.ui \
    widgets/jackproducerwidget.ui \
    widgets/toneproducerwidget.ui \
    widgets/alsawidget.ui \
    widgets/x11grabwidget.ui \
    widgets/servicepresetwidget.ui \
    widgets/avformatproducerwidget.ui \
    widgets/imageproducerwidget.ui \
    dialogs/addencodepresetdialog.ui \
    docks/jobsdock.ui \
    dialogs/textviewerdialog.ui \
    dialogs/durationdialog.ui \
    dialogs/customprofiledialog.ui \
    widgets/webvfxproducer.ui \
    docks/timelinedock.ui \
    widgets/lumamixtransition.ui \
    widgets/directshowvideowidget.ui \
    widgets/avfoundationproducerwidget.ui \
    widgets/gdigrabwidget.ui \
    widgets/trackpropertieswidget.ui \
    widgets/timelinepropertieswidget.ui \
    docks/encodedock.ui \
    dialogs/unlinkedfilesdialog.ui \
    docks/textlistdock.ui \
    docks/resourcebuttondockwidget.ui \
    dialogs/registrationtipsdialog.ui \
    dialogs/registrationdialog.ui \
    dialogs/profeaturepromptdialog.ui \
    dialogs/upgradetopropromptdialog.ui \
    dialogs/invalidprojectdialog.ui \
    docks/encodetaskdock.ui \
    widgets/avformatproducersimplewidget.ui \
    dialogs/videomodesettingsdialog.ui

RESOURCES += \
    ../icons/resources.qrc \
    ../other-resources.qrc

OTHER_FILES += \
    ../COPYING \
    moviemator.rc \
    ../icons/moviemator.icns \
    ../Info.plist \
    ../icons/dark/index.theme \
    ../icons/light/index.theme \

TRANSLATIONS += \
     ../translations/mm_en.ts \
     ../translations/mm_zh.ts \


INCLUDEPATH = ../CuteLogger/include ../RecentDock ../FilterDock ../CommonUtil ../MltController ../QmlUtilities
INCLUDEPATH += ../AudioFilterDock
INCLUDEPATH += ../EffectDock ../EffectDock
INCLUDEPATH += ../include
INCLUDEPATH += ../Breakpad/breakpad/src
INCLUDEPATH += ../VideoFilterDock
INCLUDEPATH += ../TextDock

debug_and_release {
    build_pass:CONFIG(debug, debug|release) {
        LIBS += -L../CuteLogger/debug -L../RecentDock/debug -L../FilterDock/debug -L../CommonUtil/debug -L../MltController/debug
        LIBS += -L../QmlUtilities/debug
        LIBS += -L../EffectDock/debug
        LIBS += -L../AudioFilterDock/debug
        LIBS += -L../Breakpad/debug
        LIBS += -L../VideoFilterDock/debug
        LIBS += -L../TextDock/debug
    } else {
        LIBS += -L../CuteLogger/release -L../RecentDock/release -L../FilterDock/release -L../CommonUtil/release -L../MltController/release
        LIBS += -L../QmlUtilities/release
        LIBS += -L../EffectDock/release
        LIBS += -L../AudioFilterDock/release
        LIBS += -L../Breakpad/release
        LIBS += -L../VideoFilterDock/release
        LIBS += -L../TextDock/release
    }
} else {
    LIBS += -L../CuteLogger -L../RecentDock -L../FilterDock -L../CommonUtil -L../MltController -L../QmlUtilities #-L../mm
    LIBS += -L../EffectDock
    LIBS += -L../AudioFilterDock
    LIBS += -L../Breakpad
    LIBS += -L../VideoFilterDock
    LIBS += -L../TextDock
}

LIBS += -lLogger -lpthread  -lRecentDock -lFilterDock -lAudioFilterDock -lCommonUtil -lMltController -lQmlUtilities
LIBS += -lEffectDock
LIBS += -lBreakpad
LIBS += -lVideoFilterDock
LIBS += -lTextDock



#INCLUDEPATH += ../PythonQt3.2/src

#LIBS += -L/Users/gdb/work/MacVideoEditor/trunk/MovieMator/PythonQt3.2/lib

#LIBS += -lPythonQt-Qt5-Python2.7_d

mac {
#    DEFINES += STEAM=1
    #pro share
    DEFINES += MOVIEMATOR_PRO=1
    DEFINES += SHARE_VERSION=1
    QMAKE_INFO_PLIST = ../Info_share.plist
    ICON = ../icons/moviemator-pro.icns
    TARGET = "MovieMator Video Editor Pro"


    #free appstore
#    DEFINES += MOVIEMATOR_FREE=1
#    TARGET = "MovieMator Video Editor Lite"
#    QMAKE_INFO_PLIST = ../Info-Free.plist
#    ICON = ../icons/moviemator.icns

    #free share
#    DEFINES += SHARE_VERSION=1
#    DEFINES += MOVIEMATOR_FREE=1
#    TARGET = "MovieMator Video Editor Lite"
#    QMAKE_INFO_PLIST = ../Info-Free-share.plist
#    ICON = ../icons/moviemator.icns

    #pro appstore
#    DEFINES += MOVIEMATOR_PRO=1
#    TARGET = "MovieMator Video Editor Pro"
#    QMAKE_INFO_PLIST = ../Info.plist
#    ICON = ../icons/moviemator-pro.icns

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8

    # QMake from Qt 5.1.0 on OSX is messing with the environment in which it runs
    # pkg-config such that the PKG_CONFIG_PATH env var is not set.
    isEmpty(MLT_PREFIX) {
#        MLT_PREFIX = $$PWD/../../../../mlt_lib
#        MLT_PREFIX = $$PWD/../../../../MovieMator_gdb/MacVideoEditor/trunk/shotcut/mlt_build
        MLT_PREFIX = $$PWD/../../../../shotcut/mlt_build/
#        count($$USER, wzq)
#        {
#            MLT_PREFIX = /Users/wzq/Desktop/data/project/2018/moviemator/libs/mlt_build/debug
#        }
    }

    INCLUDEPATH += $$MLT_PREFIX/include/mlt++
    INCLUDEPATH += $$MLT_PREFIX/include/mlt
#    INCLUDEPATH += ./mltmodules

#    INCLUDEPATH += $$MLT_PREFIX/include/SDL

    LIBS += -L$$MLT_PREFIX/lib -lmlt++ -lmlt
#    LIBS += -L$$MLT_PREFIX/lib -lmlt++ -lmlt -lSDL
#    LIBS += -lexif -lmltqt
    LIBS += "-framework Cocoa"
    LIBS += -L$$PWD/../../../../build/ecc -lregister

    QMAKE_LFLAGS += -Wl,/usr/lib/libcrypto.0.9.8.dylib
#    INCLUDEPATH += /System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7
#    LIBS += -F/System/Library/Frameworks -framework Python
    QMAKE_RPATHDIR += @executable_path/qt_lib/lib
#    QT_PLUGIN_PATH += @executable_path/qt_lib/plugins/
#    QT_QPA_PLATFORM_PLUGIN_PATH = @executable_path/qt_lib/plugins/platforms/
}


win32 {
    #free share
#    DEFINES += SHARE_VERSION=1
#    DEFINES += MOVIEMATOR_FREE=1

    #pro share
    DEFINES += STEAM=1
    DEFINES += MOVIEMATOR_PRO=1
    DEFINES += SHARE_VERSION=1
}

win32 {
    CONFIG += windows rtti
    isEmpty(MLT_PATH) {
        message("MLT_PATH not set; using C:\\Projects\\MovieMator. You can change this with 'qmake MLT_PATH=...'")
        MLT_PATH = C:\\Projects\\MovieMator
    }
    INCLUDEPATH += $$MLT_PATH\\include\\mlt++ $$MLT_PATH\\include\\mlt
    LIBS += -L$$MLT_PATH\\lib -lmlt++ -lmlt -lopengl32
    RC_FILE = moviemator.rc

    LIBS += -L$$MLT_PATH -leay32
    LIBS += -L$$MLT_PATH -lregister
}

unix:!mac {
    QT += x11extras
    CONFIG += link_pkgconfig
    PKGCONFIG += mlt++
    LIBS += -lX11
}

isEmpty(MOVIEMATOR_VERSION) {
    !win32:MOVIEMATOR_VERSION = $$system(date "+%y.%m.%d")
     win32:MOVIEMATOR_VERSION = adhoc
}
#DEFINES += MOVIEMATOR_VERSION=\\\"$$MOVIEMATOR_VERSION\\\"
DEFINES += MOVIEMATOR_VERSION=\\\"2.6.2\\\"


unix:!mac:isEmpty(PREFIX) {
    message("Install PREFIX not set; using /usr/local. You can change this with 'qmake PREFIX=...'")
    PREFIX = /usr/local
}
win32:isEmpty(PREFIX) {
    message("Install PREFIX not set; using C:\\Projects\\MovieMator. You can change this with 'qmake PREFIX=...'")
    PREFIX = C:\\Projects\\MovieMator
}
unix:target.path = $$PREFIX/bin
win32:target.path = $$PREFIX
INSTALLS += target

win32 {
    qmlfiles.files = $$PWD/moviemator_qml/moviemator/qml
    qmlfiles.path = $$PREFIX/share/moviemator
    INSTALLS += qmlfiles
}

mac {
    qmlfiles.files = $$PWD/../../../../shotcut/moviemator_qml/moviemator/qml
    qmlfiles.path = $$PREFIX/share/MovieMator
    INSTALLS += qmlfiles
}

DISTFILES += \
    qss/mainstylesheet.qss \
    ../Info_share.plist \
    ../translations/mm_en.qm \
    ../translations/mm_zh.qm


include(../win32debug.pri)
