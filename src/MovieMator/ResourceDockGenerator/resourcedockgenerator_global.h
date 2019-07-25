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
    QString strName;
    QString strClassification;
    QString strThumbnailFilePath;
    int nIndexOfMetadataModel;//保存filter在metadataModel中索引
    bool bVisible;
} FilterInfo;

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createTextDock(MainInterface *pMainInterface);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createStickerDock(MainInterface *pMainInterface);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createRecentDock(MainInterface *pMainInterface);
RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_addFileToRecentDock(const QString &strFilePath);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *g_createFilterDock(MainInterface *pMainInterface, int nFilterDockType = 0);//0：视频滤镜dock，1：音频滤镜dock
RESOURCEDOCKGENERATORSHARED_EXPORT void g_setFiltersInfo(QDockWidget *pDockWidget, const QList<FilterInfo> filtersInfo);

#endif // RESOURCEDOCKGENERATOR_GLOBAL_H
