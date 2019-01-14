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

#ifndef CUSTOMTABWIDGET
#define CUSTOMTABWIDGET

#endif // CUSTOMTABWIDGET

#include <QTabBar>
#include <QTabWidget>
#include <QStylePainter>
#include <QStyleOptionTabV3>

class CustomTabBar : public QTabBar
{
public:
    explicit CustomTabBar(QWidget* parent=0) : QTabBar(parent)
    {
        setIconSize(QSize(80, 40));
    }

protected:
    QSize tabSizeHint(int) const
    {
        return QSize(80, 30);
    }
    void paintEvent(QPaintEvent *)
    {
        QStylePainter p(this);
        for (int index = 0; index < count(); index++)
        {
            QStyleOptionTabV3 tab;
            initStyleOption(&tab, index);

            QIcon tempIcon = tab.icon;
            QString tempText = tab.text;
            tab.icon = QIcon();
            tab.text = QString();

            p.drawControl(QStyle::CE_TabBarTab, tab);

            QPainter painter;
            painter.begin(this);
            QRect tabrect = tabRect(index);
            tabrect.adjust(0, 8, 0, -8);
            painter.drawText(tabrect, Qt::AlignBottom | Qt::AlignHCenter, tempText);
            tempIcon.paint(&painter, 0, tabrect.top(), tab.iconSize.width(), tab.iconSize.height(), Qt::AlignTop | Qt::AlignHCenter);
            painter.end();
        }
    }
};

class CustomTabWidget : public QTabWidget
{
public:
    explicit CustomTabWidget(QWidget *parent = 0) : QTabWidget(parent)
    {
        setTabBar(new CustomTabBar());
    }
};
