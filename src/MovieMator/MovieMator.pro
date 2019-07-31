TEMPLATE = subdirs

SUBDIRS = CuteLogger \
    CommonUtil \
    QmlUtilities \
    MltController \
    EffectDock \
    src \
    Breakpad \
    CrashReporter \
    ResourceDockGenerator
cache()
CommonUtil.depends = CuteLogger
QmlUtilities.depends = CommonUtil
MltController.depends = QmlUtilities
EffectDock.depends = CommonUtil
ResourceDockGenerator.depends = CommonUtil
src.depends = CuteLogger CommonUtil QmlUtilities MltController EffectDock Breakpad ResourceDockGenerator


