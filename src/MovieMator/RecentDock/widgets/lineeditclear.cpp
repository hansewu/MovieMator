/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/
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
#include "lineeditclear.h"
#include <QToolButton>
#include <QStyle>

LineEditClear::LineEditClear(QWidget *parent)
    : QLineEdit(parent)
{
    clearButton = new QToolButton(this);
    clearButton->setIcon(QIcon::fromTheme("edit-clear", QIcon(":/icons/oxygen/32x32/actions/edit-clear.png")));
//    clearButton->setIconSize(QSize(16, 16));
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    clearButton->hide();
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
//    setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(clearButton->sizeHint().width() + frameWidth + 1));
//    setStyleSheet("QLineEdit {padding-right: 20px; background-image: url(:/icons/light/32x32/search-icon.png); background-repeat: norepeat; background-position: center; border: 1px; border-radius: 3px; background-color: rgb(50,50,50) ; }");
    //ui->lineEdit->setStyleSheet("QLineEdit {color:rgb(255,255,255); background-color:rgb(100,100,100)}");
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
                   qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));
}

void LineEditClear::resizeEvent(QResizeEvent *)
{
    QSize sz = clearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    clearButton->move(rect().right() - frameWidth - sz.width(),
                      (rect().bottom() + 1 - sz.height())/2);
}

void LineEditClear::onTextChanged(const QString& text)
{
    clearButton->setVisible(!text.isEmpty());

    if (text.isEmpty())
        setStyleSheet(m_defaultStyleSheet);
//        setStyleSheet("QLineEdit {padding-right: 20px; background-image: url(:/icons/light/32x32/search-icon.png); background-repeat: norepeat; background-position: center; border: 1px; border-radius: 3px; background-color: rgb(100,100,100) ; }");
    else
        setStyleSheet(m_editingStyleSheet);
//        setStyleSheet("QLineEdit {padding-right: 20px; border: 1px; border-radius: 3px; background-color: rgb(100,100,100) ; }");
}

void LineEditClear::setStyleSheets(const QString &defaultStyleSheet, const QString &editingStyleSheet)
{
    setStyleSheet(defaultStyleSheet);
    m_defaultStyleSheet = defaultStyleSheet;
    m_editingStyleSheet = editingStyleSheet;
}
