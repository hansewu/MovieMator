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
    mltcontroller.cpp \
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
    glwidget.cpp \
    widgets/servicepresetwidget.cpp \
    abstractproducerwidget.cpp \
    widgets/avformatproducerwidget.cpp \
    widgets/imageproducerwidget.cpp \
    widgets/timespinbox.cpp \
    widgets/audiometerwidget.cpp \
    docks/recentdock.cpp \
    docks/encodedock.cpp \
    dialogs/addencodepresetdialog.cpp \
    jobqueue.cpp \
    docks/jobsdock.cpp \
    dialogs/textviewerdialog.cpp \
    models/playlistmodel.cpp \
    docks/playlistdock.cpp \
    dialogs/durationdialog.cpp \
    mvcp/qconsole.cpp \
    mvcp/mvcp_socket.cpp \
    mvcp/meltedclipsmodel.cpp \
    mvcp/meltedunitsmodel.cpp \
    mvcp/mvcpthread.cpp \
    mvcp/meltedplaylistmodel.cpp \
    mvcp/meltedplaylistdock.cpp \
    mvcp/meltedserverdock.cpp \
    widgets/colorwheel.cpp \
    models/attachedfiltersmodel.cpp \
    models/metadatamodel.cpp \
    docks/filtersdock.cpp \
    dialogs/customprofiledialog.cpp \
    qmltypes/colorpickeritem.cpp \
    qmltypes/colorwheelitem.cpp \
    qmltypes/qmlapplication.cpp \
    qmltypes/qmlfile.cpp \
    qmltypes/qmlfilter.cpp \
    qmltypes/qmlhtmleditor.cpp \
    qmltypes/qmlmetadata.cpp \
    qmltypes/timelineitems.cpp \
    qmltypes/qmlprofile.cpp \
    settings.cpp \
    widgets/lineeditclear.cpp \
    widgets/webvfxproducer.cpp \
    database.cpp \
    widgets/gltestwidget.cpp \
    models/multitrackmodel.cpp \
    docks/timelinedock.cpp \
    qmltypes/qmlutilities.cpp \
    qmltypes/qmlview.cpp \
    qmltypes/thumbnailprovider.cpp \
    commands/timelinecommands.cpp \
    util.cpp \
    widgets/lumamixtransition.cpp \
    autosavefile.cpp \
    widgets/directshowvideowidget.cpp \
    jobs/abstractjob.cpp \
    jobs/meltjob.cpp \
    jobs/encodejob.cpp \
    jobs/videoqualityjob.cpp \
    commands/playlistcommands.cpp \
    docks/scopedock.cpp \
    controllers/scopecontroller.cpp \
    widgets/scopes/scopewidget.cpp \
    widgets/scopes/audioloudnessscopewidget.cpp \
    widgets/scopes/audiopeakmeterscopewidget.cpp \
    widgets/scopes/audiospectrumscopewidget.cpp \
    widgets/scopes/audiowaveformscopewidget.cpp \
    widgets/scopes/videowaveformscopewidget.cpp \
    sharedframe.cpp \
    widgets/audioscale.cpp \
    widgets/playlisttable.cpp \
    commands/undohelper.cpp \
    models/audiolevelstask.cpp \
    mltxmlchecker.cpp \
    widgets/avfoundationproducerwidget.cpp \
    widgets/gdigrabwidget.cpp \
    widgets/trackpropertieswidget.cpp \
    widgets/timelinepropertieswidget.cpp \
    widgets/filterwidget.cpp \
    models/mediaclipmodel.cpp \
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
    jobs/melttask.cpp


HEADERS  += mainwindow.h \
    mltcontroller.h \
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
    glwidget.h \
    widgets/servicepresetwidget.h \
    widgets/avformatproducerwidget.h \
    widgets/imageproducerwidget.h \
    widgets/timespinbox.h \
    widgets/iecscale.h \
    widgets/audiometerwidget.h \
    docks/recentdock.h \
    docks/encodedock.h \
    dialogs/addencodepresetdialog.h \
    jobqueue.h \
    docks/jobsdock.h \
    dialogs/textviewerdialog.h \
    models/playlistmodel.h \
    docks/playlistdock.h \
    dialogs/durationdialog.h \
    mvcp/qconsole.h \
    mvcp/meltedclipsmodel.h \
    mvcp/meltedunitsmodel.h \
    mvcp/mvcpthread.h \
    mvcp/meltedplaylistmodel.h \
    mvcp/meltedplaylistdock.h \
    mvcp/meltedserverdock.h \
    transportcontrol.h \
    widgets/colorwheel.h \
    models/attachedfiltersmodel.h \
    models/metadatamodel.h \
    docks/filtersdock.h \
    dialogs/customprofiledialog.h \
    qmltypes/colorpickeritem.h \
    qmltypes/colorwheelitem.h \
    qmltypes/qmlapplication.h \
    qmltypes/qmlfile.h \
    qmltypes/qmlfilter.h \
    qmltypes/qmlhtmleditor.h \
    qmltypes/qmlmetadata.h \
    qmltypes/timelineitems.h \
    qmltypes/qmlprofile.h \
    settings.h \
    widgets/lineeditclear.h \
    widgets/webvfxproducer.h \
    database.h \
    widgets/gltestwidget.h \
    models/multitrackmodel.h \
    docks/timelinedock.h \
    qmltypes/qmlutilities.h \
    qmltypes/qmlview.h \
    qmltypes/thumbnailprovider.h \
    commands/timelinecommands.h \
    util.h \
    widgets/lumamixtransition.h \
    autosavefile.h \
    widgets/directshowvideowidget.h \
    jobs/abstractjob.h \
    jobs/meltjob.h \
    jobs/encodejob.h \
    jobs/videoqualityjob.h \
    commands/playlistcommands.h \
    docks/scopedock.h \
    controllers/scopecontroller.h \
    widgets/scopes/scopewidget.h \
    widgets/scopes/audioloudnessscopewidget.h \
    widgets/scopes/audiopeakmeterscopewidget.h \
    widgets/scopes/audiospectrumscopewidget.h \
    widgets/scopes/audiowaveformscopewidget.h \
    widgets/scopes/videowaveformscopewidget.h \
    dataqueue.h \
    sharedframe.h \
    widgets/audioscale.h \
    widgets/playlisttable.h \
    commands/undohelper.h \
    models/audiolevelstask.h \
    shotcut_mlt_properties.h \
    mltxmlchecker.h \
    widgets/avfoundationproducerwidget.h \
    widgets/gdigrabwidget.h \
    widgets/trackpropertieswidget.h \
    widgets/timelinepropertieswidget.h \
    widgets/filterwidget.h \
    models/mediaclipmodel.h \
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
    melt/melt.h


#OBJECTIVE_SOURCES+= \
#    ../iRate/iRate.mm \
#    securitybookmark/SecurityBookmark.mm \
#    eccregister/EccRegister.mm

#OBJECTIVE_HEADERS+= \
#    ../iRate/iRate.h \
#    securitybookmark/SecurityBookmark.h \
#    eccregister/EccRegister.h


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
    docks/recentdock.ui \
    dialogs/addencodepresetdialog.ui \
    docks/jobsdock.ui \
    dialogs/textviewerdialog.ui \
    docks/playlistdock.ui \
    dialogs/durationdialog.ui \
    mvcp/meltedserverdock.ui \
    mvcp/meltedplaylistdock.ui \
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
    docks/encodetaskdock.ui

RESOURCES += \
    ../icons/resources.qrc \
    ../other-resources.qrc

OTHER_FILES += \
    ../COPYING \
    shotcut.rc \
    ../scripts/build-shotcut.sh \
    ../icons/moviemator.icns \
    ../scripts/shotcut.nsi \
    ../Info.plist \
    ../icons/dark/index.theme \
    ../icons/light/index.theme \

TRANSLATIONS += \
     ../translations/mm_en.ts \
     ../translations/mm_zh.ts \
# #   ../translations/shotcut_ca.ts \
#    ../translations/shotcut_cs.ts \
#    ../translations/shotcut_da.ts \
#    ../translations/shotcut_de.ts \
#    ../translations/shotcut_el.ts \

#    ../translations/shotcut_es.ts \
#    ../translations/shotcut_fr.ts \
#    ../translations/shotcut_it.ts \
#    ../translations/shotcut_nl.ts \
#    ../translations/shotcut_pl.ts \
#    ../translations/shotcut_pt_BR.ts \
#    ../translations/shotcut_pt_PT.ts \
#    ../translations/shotcut_ru.ts \
#    ../translations/shotcut_sk.ts \
#    ../translations/shotcut_uk.ts \


INCLUDEPATH = ../CuteLogger/include ../mvcp #../mm

INCLUDEPATH += ../include

debug_and_release {
    build_pass:CONFIG(debug, debug|release) {
        LIBS += -L../CuteLogger/debug -L../mvcp/debug #-L../mm/debug
    } else {
        LIBS += -L../CuteLogger/release -L../mvcp/release #-L../mm/release
    }
} else {
    LIBS += -L../CuteLogger -L../mvcp #-L../mm
}
LIBS += -lLogger -lmvcp -lpthread  #-lmm


#INCLUDEPATH += ../PythonQt3.2/src

#LIBS += -L/Users/gdb/work/MacVideoEditor/trunk/shotcut/PythonQt3.2/lib

#LIBS += -lPythonQt-Qt5-Python2.7_d

mac {
    #pro share
#    DEFINES += MOVIEMATOR_PRO=1
#    DEFINES += SHARE_VERSION=1
#    QMAKE_INFO_PLIST = ../Info_share.plist
#    ICON = ../icons/moviemator-pro.icns
#    TARGET = "MovieMator Video Editor Pro"


    #free appstore
#    DEFINES += MOVIEMATOR_FREE=1
#    TARGET = "Movie Video Editor MovieMator"
#    QMAKE_INFO_PLIST = ../Info-Free.plist
#    ICON = ../icons/moviemator.icns

    #free share
#    DEFINES += SHARE_VERSION=1
#    DEFINES += MOVIEMATOR_FREE=1
#    TARGET = "MovieMator Free Mac Video Editor"
#    QMAKE_INFO_PLIST = ../Info-Free-share.plist
#    ICON = ../icons/moviemator.icns

    #pro appstore
    DEFINES += MOVIEMATOR_PRO=1
    TARGET = "Video Editor MovieMator Pro"
    QMAKE_INFO_PLIST = ../Info.plist
    ICON = ../icons/moviemator-pro.icns

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8

    # QMake from Qt 5.1.0 on OSX is messing with the environment in which it runs
    # pkg-config such that the PKG_CONFIG_PATH env var is not set.
    isEmpty(MLT_PREFIX) {
        MLT_PREFIX = /Users/gdb/shotcut_build_v1606_release/build
    }

    INCLUDEPATH += $$MLT_PREFIX/include/mlt++
    INCLUDEPATH += $$MLT_PREFIX/include/mlt
    INCLUDEPATH += ./mltmodules

    INCLUDEPATH += $$MLT_PREFIX/include/SDL

    LIBS += -L$$MLT_PREFIX/lib -lmlt++ -lmlt -lSDL
    LIBS += -lexif -lmltqt
    LIBS += "-framework Cocoa"

    INCLUDEPATH += /System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7
    LIBS += -F/System/Library/Frameworks -framework Python
}

#LIBS += -L/usr/lib -lcrypto
#QMAKE_LFLAGS += -Wl,/usr/lib/libcrypto.0.9.8.dylib

win32 {
    CONFIG += windows rtti
    isEmpty(MLT_PATH) {
        message("MLT_PATH not set; using C:\\Projects\\MovieMator. You can change this with 'qmake MLT_PATH=...'")
        MLT_PATH = C:\\Projects\\MovieMator
    }
    INCLUDEPATH += $$MLT_PATH\\include\\mlt++ $$MLT_PATH\\include\\mlt
    LIBS += -L$$MLT_PATH\\lib -lmlt++ -lmlt -lopengl32
    RC_FILE = shotcut.rc
}
unix:!mac {
    QT += x11extras
    CONFIG += link_pkgconfig
    PKGCONFIG += mlt++
    LIBS += -lX11
}

isEmpty(SHOTCUT_VERSION) {
    !win32:SHOTCUT_VERSION = $$system(date "+%y.%m.%d")
     win32:SHOTCUT_VERSION = adhoc
}
#DEFINES += SHOTCUT_VERSION=\\\"$$SHOTCUT_VERSION\\\"
DEFINES += SHOTCUT_VERSION=\\\"2.4.6\\\"


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

qmlfiles.files = $$PWD/qml
qmlfiles.path = $$PREFIX/share/MovieMator
INSTALLS += qmlfiles

DISTFILES += \
    qss/mainstylesheet.qss \
    ../Info_share.plist \
    ../translations/shotcut_zh.qm \
    ../translations/mm_en.qm \
    ../translations/mm_zh.qm


