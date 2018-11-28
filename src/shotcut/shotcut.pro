TEMPLATE = subdirs

SUBDIRS = CuteLogger mvcp src \
    RecentDock \
    PlaylistDock \
    CommonUtil \
    MltController
cache()
src.depends = CuteLogger mvcp CommonUtil MltController RecentDock PlaylistDock
RecentDock.depends = CuteLogger CommonUtil


