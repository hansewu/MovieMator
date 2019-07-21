#ifndef RESOURCEDOCKGENERATOR_GLOBAL_H
#define RESOURCEDOCKGENERATOR_GLOBAL_H

#include <QtCore/qglobal.h>

#include <QDockWidget>
#include <maininterface.h>

#if defined(RESOURCEDOCKGENERATOR_LIBRARY)
#  define RESOURCEDOCKGENERATORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define RESOURCEDOCKGENERATORSHARED_EXPORT Q_DECL_IMPORT
#endif

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createTextDock(MainInterface *pMainInterface);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createFilterDock(MainInterface *pMainInterface);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createStickerDock(MainInterface *pMainInterface);

#endif // RESOURCEDOCKGENERATOR_GLOBAL_H
