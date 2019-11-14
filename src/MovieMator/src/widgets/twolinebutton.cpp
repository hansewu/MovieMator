/*
 * Copyright (c) 2019 EffectMatrix Inc.
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

#include "twolinebutton.h"
#include <QPainter>
#include <QApplication>

TwoLineButton::TwoLineButton(QWidget *parent)
    : QPushButton(parent)
    , m_strMainTitle("")
    , m_strSubTitle("")
    , m_strAccessoryTitle("")
{

}

TwoLineButton::~TwoLineButton()
{

}

void TwoLineButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    setText("");

    int nMarginLeft = 12;

    QPainter paint(this);
    paint.save();

    QFont sub(QApplication::font());

    //子标题绘制，按钮下半部分
    sub.setPointSize(sub.pointSize());
    paint.setFont(sub);
    QFontMetrics fontMetrics = paint.fontMetrics();
    int nTextHeight = fontMetrics.height();
    paint.drawText(QPoint(nMarginLeft, height()/2 + nTextHeight + 4), subTitle());//height()/2 + nTextHeight + 4 中间往下4像素

    //按钮上半部分主标题绘制
    sub.setPointSize(sub.pointSize() + 7);
    paint.setFont(sub);
    paint.drawText(QPoint(nMarginLeft, height()/2 - 4), mainTitle()); //height()/2 - 4 中间往上4像素

    //主标题右边文字
    fontMetrics = paint.fontMetrics();
    int nTextWidth = fontMetrics.width(accessoryTitle());
    paint.drawText(QPoint(width() - nTextWidth - nMarginLeft, height()/2 - 4), accessoryTitle());


    paint.restore();
}

void TwoLineButton::setMainTitle(const QString &strText)
{
    m_strMainTitle = strText;
}

QString TwoLineButton::mainTitle() const
{
    return m_strMainTitle;
}

void TwoLineButton::setSubTitle(const QString &strText)
{
    m_strSubTitle = strText;
}

QString TwoLineButton::subTitle() const
{
    return m_strSubTitle;
}

void TwoLineButton::setAccessoryTitle(const QString &strText)
{
    m_strAccessoryTitle = strText;
}

QString TwoLineButton::accessoryTitle() const
{
    return m_strAccessoryTitle;
}

