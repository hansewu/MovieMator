TEMPLATE = subdirs

SUBDIRS = CuteLogger \
    CommonUtil \
    QmlUtilities \
    MltController \
    RecentDock \
    FilterDock \
    AudioFilterDock \
    EffectDock \
    src \
    Breakpad \
    CrashReporter \
    VideoFilterDock \
    TextDock \
    ResourceDockGenerator
cache()
CommonUtil.depends = CuteLogger
QmlUtilities.depends = CommonUtil
MltController.depends = QmlUtilities
RecentDock.depends = CommonUtil
FilterDock.depends = QmlUtilities
AudioFilterDock.depends = QmlUtilities
EffectDock.depends = CommonUtil
VideoFilterDock.depends = CommonUtil
TextDock.depends = CommonUtil
ResourceDockGenerator.depends = CommonUtil
src.depends = CuteLogger CommonUtil QmlUtilities MltController RecentDock FilterDock AudioFilterDock EffectDock Breakpad VideoFilterDock TextDock ResourceDockGenerator


