TEMPLATE = subdirs

SUBDIRS = CuteLogger mvcp src \
    CommonUtil \
    QmlUtilities \
    RecentDock \
    PlaylistDock \
    MltController
cache()
src.depends = CuteLogger mvcp CommonUtil QmlUtilities MltController RecentDock PlaylistDock
RecentDock.depends = CuteLogger CommonUtil


