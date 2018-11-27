TEMPLATE = subdirs

SUBDIRS = CuteLogger mvcp src \
    RecentDock \
    PlaylistDock
cache()
src.depends = CuteLogger mvcp RecentDock PlaylistDock
RecentDock.depends = CuteLogger


