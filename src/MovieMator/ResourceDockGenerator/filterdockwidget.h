#ifndef FILTERDOCKWIDGET_H
#define FILTERDOCKWIDGET_H

#include "basedockwidget.h"
#include "resourcedockgenerator_global.h"

#include <qscriptengine.h>

class FilterDockWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    // 构造函数
    explicit FilterDockWidget(int nFilterDockType = 0, MainInterface *pMainInterface = nullptr, QWidget *pParent = nullptr);
    void setFiltersInfo(QList<FilterInfo> filtersInfo);

protected:
    QUnsortMap<QString, BaseItemModel *> *createAllClassesItemModel();
    void addItemToTimeline(const QStandardItem *pItem);
    void preview(const QStandardItem *pItem);

private:
    void readTranslatJsFile(QString jsFilePath);
    QString getQmlDirPath();
    void previewFilter(QString strPerviewSettingFilePath);

private:
    MainInterface *m_pMainInterface;
    int m_nFilterDockType;//0：为视频滤镜dock，1：为音频滤镜dock
    QList<FilterInfo> m_filtersInfo;
    QScriptEngine m_jsEngine;
};

#endif // FILTERDOCKWIDGET_H
