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

#ifndef FILTERDOCK_H
#define FILTERDOCK_H
#include <QDockWidget>
#include <QQuickWidget>
#include <maininterface.h>
#include "filterdock_global.h"
#include "filterdockinterface.h"


class FilterItemInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filterType READ filterType WRITE setFilterType)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString imageSourcePath READ imageSourcePath WRITE setImageSourcePath)
    Q_PROPERTY(bool visible READ visible WRITE setVisible)

public:

    explicit FilterItemInfo(QObject *parent = NULL){}

    Q_INVOKABLE QString filterType() const { return m_filterType; }
    Q_INVOKABLE void setFilterType(const QString strType){m_filterType = strType;}
    Q_INVOKABLE QString name() const { return m_name; }
    Q_INVOKABLE void setName(const QString strName){m_name = strName;}
    Q_INVOKABLE QString imageSourcePath() const { return m_imageSourcePath; }
    Q_INVOKABLE void setImageSourcePath(const QString strImageSourcePath){m_imageSourcePath = strImageSourcePath;}
    Q_INVOKABLE bool visible() const { return m_bVisible; }
    Q_INVOKABLE void setVisible(const bool visible){m_bVisible = visible;}

private:
    QString m_filterType;
    QString m_name;
    QString m_imageSourcePath;
    bool m_bVisible;
};

class FiltersInfo: public QObject
{
    Q_OBJECT
public:
    FiltersInfo(){m_filterInfoList.clear();}
    ~FiltersInfo(){}

    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE int rowCount(){return m_filterInfoList.size();}
    Q_INVOKABLE FilterItemInfo* get(int index) const {return m_filterInfoList.at(index);}

    void addFilterItemInfo(FilterItemInfo* filterInfo);

private:
    typedef QList<FilterItemInfo*> FiltersList;
    FiltersList m_filterInfoList;
};

class FILTERDOCKSHARED_EXPORT FilterDock: public QDockWidget
{
    Q_OBJECT

public:
    explicit FilterDock(MainInterface *main=0, QWidget *parent = 0);
    ~FilterDock();

    int updateFilters(Filter_Info * filterInfos, int nFilterCount);
    int createFilterDockPage();
    void resetQview();

    Q_INVOKABLE void addFilterItem(int index);

private:
    MainInterface *m_mainWindow;
    QQuickWidget m_qview;
    FiltersInfo *m_pFilterInfo;
};



#endif // FILTERDOCK_H

