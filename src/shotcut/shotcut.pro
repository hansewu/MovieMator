TEMPLATE = subdirs

SUBDIRS = CuteLogger mvcp src#mm
cache()
#mm.depends = CuteLogger mvcp
src.depends = CuteLogger mvcp #mm



