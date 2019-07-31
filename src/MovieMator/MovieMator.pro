TEMPLATE = subdirs

SUBDIRS = CuteLogger \
    CommonUtil \
    QmlUtilities \
    MltController \
    RecentDock \
    FilterDock \
    EffectDock \
    src \
    Breakpad \
    CrashReporter \
    VideoFilterDock \
    ResourceDockGenerator
cache()
CommonUtil.depends = CuteLogger
QmlUtilities.depends = CommonUtil
MltController.depends = QmlUtilities
RecentDock.depends = CommonUtil
FilterDock.depends = QmlUtilities
EffectDock.depends = CommonUtil
VideoFilterDock.depends = CommonUtil
ResourceDockGenerator.depends = CommonUtil
src.depends = CuteLogger CommonUtil QmlUtilities MltController RecentDock FilterDock EffectDock Breakpad VideoFilterDock ResourceDockGenerator


