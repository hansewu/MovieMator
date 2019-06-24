TEMPLATE = subdirs

SUBDIRS = CuteLogger \
    mvcp \
    CommonUtil \
    QmlUtilities \
    MltController \
    RecentDock \
    FilterDock \
    AudioFilterDock \
    EffectDock \
    TemplateEditorDock \
    src \
    Breakpad \
    CrashReporter
cache()
CommonUtil.depends = CuteLogger
QmlUtilities.depends = CommonUtil
MltController.depends = QmlUtilities
RecentDock.depends = CommonUtil
FilterDock.depends = QmlUtilities
AudioFilterDock.depends = QmlUtilities
EffectDock.depends = CommonUtil
TemplateEditorDock.depends = QmlUtilities
src.depends = CuteLogger mvcp CommonUtil QmlUtilities MltController RecentDock FilterDock AudioFilterDock EffectDock TemplateEditorDock Breakpad


