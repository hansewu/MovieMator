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

typedef struct {
    char strName[256];
    char strType[256];
    char strImageSourcePath[256];
    bool bVisible;
} FilterInfo;

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createTextDock(MainInterface *pMainInterface);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createStickerDock(MainInterface *pMainInterface);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createRecentDock(MainInterface *pMainInterface);
RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_addFilesTRecentDock(const QString &filtePath);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createFilterDock(MainInterface *pMainInterface);
RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_setFiltersInfo(QDockWidget *pFilterDock, FilterInfo *filtersInfo, int nFilterCount);

#endif // RESOURCEDOCKGENERATOR_GLOBAL_H
