#include "filterdock.h"
#include "ui_filterdock.h"
#include <QQuickWidget>
#include <QQuickView>
#include <QQmlContext>
#include <filterdockinterface.h>
#include <QDir>
/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl1987527 <wyl1987527@163.com>
 * Author: Author: fuyunhuaxin <2446010587@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QStandardPaths>
#include <QtQml>
#include <qmlutilities.h>
#include <qmlview.h>

FilterDock::FilterDock(MainInterface *main, QWidget *parent):
    QDockWidget(parent),
  m_qview(QmlUtilities::sharedEngine(), this)
{
    m_mainWindow = main;
    m_pFilterInfo = nullptr;
    m_qview.setFocusPolicy(Qt::StrongFocus);
    setWidget(&m_qview);

    QmlUtilities::setCommonProperties(m_qview.rootContext());

    createFilterDockPage();
}

FilterDock::~FilterDock()
{
    if(m_pFilterInfo) {delete m_pFilterInfo; m_pFilterInfo = nullptr;}
}

void FilterDock::resetQview()
{
//    Q_ASSERT(&m_qview);
//    if(!(&m_qview)) return;
    QDir viewPath = QmlUtilities::qmlDir();
    viewPath.cd("views");
    viewPath.cd("filter");
    m_qview.engine()->addImportPath(viewPath.path());

    QDir modulePath = QmlUtilities::qmlDir();
    modulePath.cd("modules");
    m_qview.engine()->addImportPath(modulePath.path());

    m_qview.setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qview.quickWindow()->setColor(palette().window().color());
    QUrl source = QUrl::fromLocalFile(viewPath.absoluteFilePath("FiltersUI.qml"));
    m_qview.setSource(source);
}

int FilterDock::createFilterDockPage()
{
//    Q_ASSERT(&m_qview);
//    if(!(&m_qview)) return 1;

    qmlRegisterType<FilterItemInfo>("FilterItemInfo", 1, 0, "FilterItemInfo");

    m_qview.rootContext()->setContextProperty("filtersInfo", m_pFilterInfo);
    m_qview.rootContext()->setContextProperty("filtersResDock", this);

    resetQview();
    return 0;
}

int FilterDock::updateFilters(Filter_Info * filterInfos, int nFilterCount)
{
    Q_ASSERT(filterInfos);
    if(!filterInfos) return 1;
//    Q_ASSERT(&m_qview);
//    if(!(&m_qview)) return 1;

    if(m_pFilterInfo) {delete m_pFilterInfo; m_pFilterInfo = nullptr;}

    m_pFilterInfo = new FiltersInfo();
    for(int i = 0; i < nFilterCount; i++)
    {
        FilterItemInfo *filterInfo = new FilterItemInfo();

        filterInfo->setVisible(filterInfos[i].visible);
        filterInfo->setName(QString(filterInfos[i].name));
        filterInfo->setFilterType(QString(filterInfos[i].type));
        filterInfo->setImageSourcePath(QString(filterInfos[i].imageSourcePath));
        filterInfo->setPerviewSettingFilePath(QString(filterInfos[i].perviewSettingFilePath));

        m_pFilterInfo->addFilterItemInfo(filterInfo);
    }
    return 0;
}



void FilterDock::addFilterItem(int index)
{
    m_mainWindow->addFilter(index);
}

void FilterDock::previewFilter(QString perviewSettingFilePath)
{
    FILE_HANDLE mltSettingFile = m_mainWindow->openFile(perviewSettingFilePath);
    m_mainWindow->playFile(mltSettingFile);
    m_mainWindow->destroyFileHandle(mltSettingFile);
}

void FiltersInfo::addFilterItemInfo(FilterItemInfo *filterInfo)
{
    Q_ASSERT(filterInfo);
    if(!filterInfo) return;
//    Q_ASSERT(&m_filterInfoList);
//    if(!(&m_filterInfoList)) return;
    m_filterInfoList.append(filterInfo);
}


static FilterDock *ftDocInstance = nullptr;
QDockWidget *FilterDock_initModule(MainInterface *main)
{
    if (ftDocInstance == nullptr)
        ftDocInstance = new FilterDock(main);

    return ftDocInstance;
}

void FilterDock_destroyModule()
{
    if(ftDocInstance) delete ftDocInstance;
}

int setFiltersInfo(Filter_Info * filterInfos, int nFilterCount)
{
    Q_ASSERT(filterInfos);
    if(!filterInfos) return 1;
    Q_ASSERT(ftDocInstance);
    if(!ftDocInstance) return 1;
    ftDocInstance->updateFilters(filterInfos, nFilterCount);
    ftDocInstance->createFilterDockPage();

    return 0;
}

int getCurrentSelectedFilterIndex()
{
    return 0;
}
