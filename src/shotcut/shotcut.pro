TEMPLATE = subdirs

SUBDIRS = CuteLogger mvcp src \
    RecentDock \
    PlaylistDock \
    CommonUtil
cache()
src.depends = CuteLogger mvcp RecentDock PlaylistDock CommonUtil
RecentDock.depends = CuteLogger CommonUtil


