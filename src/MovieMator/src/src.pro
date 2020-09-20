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
    dialogs/videomodesettingsdialog.cpp \
    docks/advanceddock.cpp \
    presettabstyle.cpp \
    dialogs/aspectratiosettingsdialog.cpp \
    widgets/twolinebutton.cpp \
    dialogs/recorddialog.cpp


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
    dialogs/videomodesettingsdialog.h \
    docks/advanceddock.h \
    presettabstyle.h \
    dialogs/aspectratiosettingsdialog.h \
    widgets/twolinebutton.h \
    dialogs/recorddialog.h

mac {
    SOURCES += \
                dialogs/inappdialog.cpp

    HEADERS += securitybookmark/SecurityBookmark.h \
                ../iRate/iRate.h \
                inapp/IAP.h \
                inapp/iap_c_interface.h \
                inapp/XYAppReceipt.h \
                dialogs/inappdialog.h

    OBJECTIVE_SOURCES += \
                inapp/IAP.mm \
                inapp/iap_c_interface.mm \
                inapp/XYAppReceipt.mm \
                securitybookmark/SecurityBookmark.mm \
                ../iRate/iRate.mm

    FORMS    += \
                dialogs/inappdialog.ui
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
    dialogs/videomodesettingsdialog.ui \
    docks/advanceddock.ui \
    dialogs/aspectratiosettingsdialog.ui \
    dialogs/recorddialog.ui


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


INCLUDEPATH = ../CuteLogger/include ../CommonUtil ../MltController ../QmlUtilities
INCLUDEPATH += ../include
INCLUDEPATH += ../Breakpad/breakpad/src
INCLUDEPATH += ../ResourceDockGenerator

debug_and_release {
    build_pass:CONFIG(debug, debug|release) {
        LIBS += -L../CuteLogger/debug -L../CommonUtil/debug -L../MltController/debug
        LIBS += -L../QmlUtilities/debug
        LIBS += -L../Breakpad/debug
        LIBS += -L../ResourceDockGenerator/debug
    } else {
        LIBS += -L../CuteLogger/release -L../CommonUtil/release -L../MltController/release
        LIBS += -L../QmlUtilities/release
        LIBS += -L../Breakpad/release
        LIBS += -L../ResourceDockGenerator/release
    }
} else {
    LIBS += -L../CuteLogger -L../CommonUtil -L../MltController -L../QmlUtilities #-L../mm
    LIBS += -L../Breakpad
    LIBS += -L../ResourceDockGenerator
}

LIBS += -lLogger -lpthread -lCommonUtil -lMltController -lQmlUtilities
LIBS += -lBreakpad
LIBS += -lResourceDockGenerator



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
#    TARGET = "MovieMator Video Editor"
#    QMAKE_INFO_PLIST = ../Info-Free.plist
#    ICON = ../icons/moviemator.icns

    #share 国区免费版本
#    DEFINES += SHARE_VERSION=1
#    DEFINES += MOVIEMATOR_FREE=1
#    TARGET = "MovieMator Video Editor Pro"
#    QMAKE_INFO_PLIST = ../Info_share.plist
#    ICON = ../icons/moviemator-pro.icns

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
    LIBS += "-framework StoreKit"
    LIBS += -L$$PWD/../../../../build/ecc -lregister

    QMAKE_LFLAGS += -Wl,/usr/lib/libcrypto.0.9.8.dylib
#    INCLUDEPATH += /System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7
#    LIBS += -F/System/Library/Frameworks -framework Python
    QMAKE_RPATHDIR += @executable_path/../Frameworks
#    QT_PLUGIN_PATH += @executable_path/qt_lib/plugins/
#    QT_QPA_PLATFORM_PLUGIN_PATH = @executable_path/qt_lib/plugins/platforms/
}


win32 {
     #中国网站版
    DEFINES += MOVIEMATOR_FREE=1
    DEFINES += SHARE_VERSION=1

    #国外网站版
#    DEFINES += MOVIEMATOR_PRO=1
#    DEFINES += SHARE_VERSION=1

    #steam版
#    DEFINES += STEAM=1
#    DEFINES += MOVIEMATOR_PRO=1
#    DEFINES += SHARE_VERSION=1
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
DEFINES += MOVIEMATOR_VERSION=\\\"3.0.2\\\"
DEFINES += MOVIEMATOR_VERSION=\\\"3.1.0\\\"


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
    qmlfiles.files = $$PWD/qml
    qmlfiles.path = $$PREFIX/share/moviemator
    INSTALLS += qmlfiles
}

mac {
    qmlfiles.files = $$PWD/qml
    qmlfiles.path = Contents/Resources/share/moviemator
    INSTALLS += qmlfiles


#自动copy资源与生成的dylib到app程序包中 xjp
    BUILD_FOLDER = /work/Projects/MovieMator-github/MovieMator/src/build-MovieMator-Desktop_Qt_5_6_1_clang_64bit1-Debug #不同机器，不同编译器，需要修改
    CODE_FOLDER  = /work/Projects/MovieMator-github   #MovieMator相关代码的存放位置

    APP_DEST = $$BUILD_FOLDER/src/MovieMator\ Video\ Editor\ Pro.app/Contents
    CODE_SOURCE = $$CODE_FOLDER/MovieMator/src/MovieMator
    MOVIEMATOR_QML = $$CODE_FOLDER/MovieMator-Resource
    QML = $$CODE_FOLDER/MovieMator/src/MovieMator/src
    TEMPLATE_SOURCE = $$CODE_FOLDER/MovieMator-Resource
    ZIP_RESOURCE_DEST = $$CODE_FOLDER/resource


    #copy libAudioFilterDock
    QMAKE_POST_LINK += cp -f  -a  $$BUILD_FOLDER/AudioFilterDock/libAudioFilterDock.1.0.0.dylib  $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f  -a  $$BUILD_FOLDER/AudioFilterDock/libAudioFilterDock.1.0.dylib  $$APP_DEST/MacOS  &
    QMAKE_POST_LINK += cp -f  -a  $$BUILD_FOLDER/AudioFilterDock/libAudioFilterDock.1.dylib  $$APP_DEST/MacOS  &
    QMAKE_POST_LINK += cp -f  -a  $$BUILD_FOLDER/AudioFilterDock/libAudioFilterDock.dylib  $$APP_DEST/MacOS &

    #copy libBreakpad
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/Breakpad/libBreakpad.1.0.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/Breakpad/libBreakpad.1.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/Breakpad/libBreakpad.1.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/Breakpad/libBreakpad.dylib $$APP_DEST/MacOS &
    #copy libCommonUtil
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/CommonUtil/libCommonUtil.1.0.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/CommonUtil/libCommonUtil.1.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/CommonUtil/libCommonUtil.1.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/CommonUtil/libCommonUtil.dylib $$APP_DEST/MacOS &
    #copy CrashReporter
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/CrashReporter/CrashReporter $$APP_DEST/MacOS &
    #copy libLogger
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/CuteLogger/libLogger.1.0.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/CuteLogger/libLogger.1.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/CuteLogger/libLogger.1.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/CuteLogger/libLogger.dylib $$APP_DEST/MacOS &
    #copy libMltController
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/MltController/libMltController.1.0.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/MltController/libMltController.1.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/MltController/libMltController.1.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/MltController/libMltController.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += install_name_tool -change $$CODE_FOLDER/shotcut/mlt_build/lib/libmlt++.3.dylib @executable_path/lib/libmlt++.3.dylib $$APP_DEST/MacOS/libMltController.1.0.0.dylib &
    QMAKE_POST_LINK += install_name_tool -change $$CODE_FOLDER/shotcut/mlt_build/lib/libmlt.6.dylib   @executable_path/lib/libmlt.6.dylib   $$APP_DEST/MacOS/libMltController.1.0.0.dylib &
    #copy libQmlUtilities
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/QmlUtilities/libQmlUtilities.1.0.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/QmlUtilities/libQmlUtilities.1.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/QmlUtilities/libQmlUtilities.1.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/QmlUtilities/libQmlUtilities.dylib $$APP_DEST/MacOS &
    #copy libResourceDockGenerator
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/ResourceDockGenerator/libResourceDockGenerator.1.0.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/ResourceDockGenerator/libResourceDockGenerator.1.0.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/ResourceDockGenerator/libResourceDockGenerator.1.dylib $$APP_DEST/MacOS &
    QMAKE_POST_LINK += cp -f -a $$BUILD_FOLDER/ResourceDockGenerator/libResourceDockGenerator.dylib $$APP_DEST/MacOS &

    #copy frei0r.txt
    QMAKE_POST_LINK += cp -f -a $$CODE_SOURCE/frei0r.txt $$APP_DEST/Resources/filters &
    #copy cr_zh.qm
    QMAKE_POST_LINK += cp -f -a $$CODE_SOURCE/translations/cr_zh.qm $$APP_DEST/Resources/translations &
    # copy mm_zh.qm
    QMAKE_POST_LINK += cp -f -a $$CODE_SOURCE/translations/mm_zh.qm $$APP_DEST/Resources/translations &
    #QML
    #拷贝moviemator/src/qml到MovieMator Video Editor Pro.app/Contents/Resources/share/moviemator/qml
    QMAKE_POST_LINK += cp -f -a $$CODE_FOLDER/MovieMator/src/MovieMator/src/qml $$APP_DEST/Resources/share/moviemator/qml &
    #mlt源码
    #1 拷贝moviemator_resource/mlt到MovieMator Video Editor Pro.app/Contents/Resources/share/mlt
    QMAKE_POST_LINK += cp -r -f $$TEMPLATE_SOURCE/mlt $$APP_DEST/Resources/share/mlt & #-f -a
    #TODO,MovieMator-Resource
    #1 MovieMator-Resource/template 拷贝到 MovieMator Video Editor Pro.app/Contents/Resources/template
    QMAKE_POST_LINK += cp -r -f $$TEMPLATE_SOURCE/template $$APP_DEST/Resources/template &

    #拷贝qt相关库
    # qt_lib_mac/qml 		--> MovieMator.app/Contents/Resources/qml
    # qt_lib_mac/PlugIns  		--> MovieMator.app/Contents/PlugIns
    # qt_lib_mac/Frameworks		--> MovieMator.app/Contents/Frameworks
    # qt_lib_mac/qt.conf		--> MovieMator.app/Contents/Resources/qt.conf
#    QMAKE_POST_LINK += cd $$TEMPLATE_SOURCE/;unzip $$TEMPLATE_SOURCE/qt_lib_mac.zip &  #解压 qt_lib_mac.zip 文件
#    QMAKE_POST_LINK += cp -r -f $$CODE_FOLDER/MovieMator-Resource/qt_lib_mac/Frameworks/* $$APP_DEST/Frameworks &
#    QMAKE_POST_LINK += cp -r -f $$CODE_FOLDER/MovieMator-Resource/qt_lib_mac/PlugIns/* $$APP_DEST/PlugIns &
#    QMAKE_POST_LINK += cp -r -f $$CODE_FOLDER/MovieMator-Resource/qt_lib_mac/qml/* $$APP_DEST/Resources/qml &
#    QMAKE_POST_LINK += cp -r -f $$CODE_FOLDER/MovieMator-Resource/qt_lib_mac/qt.conf $$APP_DEST/Resources/qt.conf &
#    QMAKE_POST_LINK += rm -r $$CODE_FOLDER/MovieMator-Resource/qt_lib_mac &   #qt_lib_mac.zip解压后，需要删除 qt_lib_mac目录
#    QMAKE_POST_LINK += rm -r $$CODE_FOLDER/MovieMator-Resource/__MACOSX &


  QMAKE_POST_LINK +=install_name_tool -change /work/Projects/MovieMator-github/shotcut/mlt_build/lib/libmlt++.3.dylib @executable_path/lib/libmlt++.3.dylib /work/Projects/MovieMator-github/MovieMator/src/build-MovieMator-Desktop_Qt_5_6_1_clang_64bit1-Debug/src/MovieMator\ Video\ Editor\ Pro.app/Contents/MacOS/MovieMator\ Video\ Editor\ Pro &
  QMAKE_POST_LINK +=install_name_tool -change /work/Projects/MovieMator-github/shotcut/mlt_build/lib/libmlt.6.dylib @executable_path/lib/libmlt.6.dylib /work/Projects/MovieMator-github/MovieMator/src/build-MovieMator-Desktop_Qt_5_6_1_clang_64bit1-Debug/src/MovieMator\ Video\ Editor\ Pro.app/Contents/MacOS/MovieMator\ Video\ Editor\ Pro  &

    QMAKE_BUNDLE_DATA += qmlfiles

}

DISTFILES += \
    qss/mainstylesheet.qss \
    ../Info_share.plist \
    ../translations/mm_en.qm \
    ../translations/mm_zh.qm

include(../win32debug.pri)


