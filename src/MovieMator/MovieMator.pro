TEMPLATE = subdirs

SUBDIRS = CuteLogger \
    mvcp \
    CommonUtil \
    QmlUtilities \
    MltController \
    RecentDock \
    FilterDock \
    AudioFilterDock \
    PlaylistDock \
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
PlaylistDock.depends = MltController
FilterDock.depends = QmlUtilities
AudioFilterDock.depends = QmlUtilities
EffectDock.depends = CommonUtil
TemplateEditorDock.depends = QmlUtilities
src.depends = CuteLogger mvcp CommonUtil QmlUtilities MltController RecentDock FilterDock AudioFilterDock PlaylistDock EffectDock TemplateEditorDock Breakpad


