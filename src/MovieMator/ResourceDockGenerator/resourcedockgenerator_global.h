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
    QString strPerviewSettingFilePath;//滤镜预览文件路径
    int nIndexOfMetadataModel;//保存filter在metadataModel中索引
    bool bVisible;
} FilterInfo;

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *RDG_CreateTextDock(MainInterface *pMainInterface);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *RDG_CreateStickerDock(MainInterface *pMainInterface);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *RDG_CreateRecentDock(MainInterface *pMainInterface);
RESOURCEDOCKGENERATORSHARED_EXPORT void RDG_AddFileToRecentDock(const QString &strFilePath);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *RDG_CreateAudioFilterDock(MainInterface *pMainInterface);
RESOURCEDOCKGENERATORSHARED_EXPORT void RDG_SetAudioFiltersInfo(const QList<FilterInfo> filtersInfo);

RESOURCEDOCKGENERATORSHARED_EXPORT QDockWidget *RDG_CreateVideoFilterDock(MainInterface *pMainInterface);
RESOURCEDOCKGENERATORSHARED_EXPORT void RDG_SetVideoFiltersInfo(const QList<FilterInfo> filtersInfo);

#endif // RESOURCEDOCKGENERATOR_GLOBAL_H
