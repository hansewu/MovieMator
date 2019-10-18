TEMPLATE = subdirs

SUBDIRS = CuteLogger \
    CommonUtil \
    QmlUtilities \
    MltController \
    src \
    Breakpad \
    CrashReporter \
    ResourceDockGenerator \
    QuaZip
cache()
CommonUtil.depends = CuteLogger
QmlUtilities.depends = CommonUtil
MltController.depends = QmlUtilities
ResourceDockGenerator.depends = CommonUtil
src.depends = CuteLogger CommonUtil QmlUtilities MltController Breakpad ResourceDockGenerator QuaZip


TRANSLATIONS += \
     ./translations/mm_en.ts \
     ./translations/mm_zh.ts
