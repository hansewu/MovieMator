TEMPLATE = subdirs

SUBDIRS = CuteLogger \
    mvcp \
    CommonUtil \
    QmlUtilities \
    MltController \
    RecentDock \
    FilterDock \
    PlaylistDock \
    ConfigurationDock \
    EffectDock \
    TemplateEditorDock \
    src
cache()
CommonUtil.depends = CuteLogger
QmlUtilities.depends = CommonUtil
MltController.depends = QmlUtilities
RecentDock.depends = CommonUtil
PlaylistDock.depends = MltController
ConfigurationDock.depends = QmlUtilities
FilterDock.depends = QmlUtilities
EffectDock.depends = CommonUtil
TemplateEditorDock.depends = QmlUtilities
src.depends = CuteLogger mvcp CommonUtil QmlUtilities MltController RecentDock FilterDock PlaylistDock ConfigurationDock EffectDock TemplateEditorDock


