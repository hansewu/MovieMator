/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
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
#ifndef CONFIGURATIONDOCK_H
#define CONFIGURATIONDOCK_H

#include "configurationdock_global.h"

#include <QDockWidget>
#include <QQuickWidget>

class QmlMetadata;

class CONFIGURATIONDOCKSHARED_EXPORT ConfigurationDock : public QDockWidget
{
    Q_OBJECT

    Q_PROPERTY(QRect dockPosition READ dockPosition NOTIFY dockPositionChanged)
public:
    explicit ConfigurationDock(QWidget *parent = 0);
    QRect dockPosition();

public slots:
    void setCurrentFilter(QObject* filter, QmlMetadata* meta, int index);

private slots:
    void resetQview();

private:
    QQuickWidget m_qview;

signals:
    void dockPositionChanged();
};

#endif // CONFIGURATIONDOCK_H
