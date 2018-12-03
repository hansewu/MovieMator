TEMPLATE = subdirs

SUBDIRS = CuteLogger \
    mvcp \
    CommonUtil \
    QmlUtilities \
    MltController \
    RecentDock \
    PlaylistDock \
    src
cache()
CommonUtil.depends = CuteLogger
QmlUtilities.depends = CommonUtil
MltController.depends = QmlUtilities
RecentDock.depends = CommonUtil
PlaylistDock.depends = MltController
src.depends = CuteLogger mvcp CommonUtil QmlUtilities MltController RecentDock PlaylistDock


