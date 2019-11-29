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

#include <QStyle>
#include <QToolButton>

LineEditClear::LineEditClear(QWidget *pParent)
    : QLineEdit(pParent)
{
    m_pClearButton = new QToolButton(this);
    m_pClearButton->setIcon(QIcon::fromTheme("edit-clear", QIcon(":/icons/oxygen/32x32/actions/edit-clear.png")));
    m_pClearButton->setCursor(Qt::ArrowCursor);
    m_pClearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    m_pClearButton->hide();

    connect(m_pClearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));

    QSize miniSize  = minimumSizeHint();
    int nFrameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setMinimumSize(qMax(miniSize.width(), m_pClearButton->sizeHint().width() + nFrameWidth * 2 + 2),
                   qMax(miniSize.height(), m_pClearButton->sizeHint().height()/* + nFrameWidth * 2 + 2*/));

    QString strDefaultStyle = "QLineEdit {padding-right: 20px; background-image: url(:/icons/light/32x32/search-icon.png); "
                                "background-repeat: norepeat; background-position: center; border: 1px; border-radius: 3px; "
                                "background-color: rgb(82,82,82) ; }";
    QString strEditingStyle = "QLineEdit {padding-right: 20px; border: 1px; border-radius: 3px; background-color: rgb(82,82,82) ; }";
    setStyleSheets(strDefaultStyle, strEditingStyle);

    QFont font = this->font();
    font.setPointSize(16);
    this->setFont(font);
}

void LineEditClear::resizeEvent(QResizeEvent *)
{
    QSize buttonSize = m_pClearButton->sizeHint();
    int nFrameWidth  = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    m_pClearButton->move(rect().right() - nFrameWidth - buttonSize.width(),
                                (rect().bottom() + 1 - buttonSize.height()) / 2);
}

void LineEditClear::onTextChanged(const QString& strText)
{
    m_pClearButton->setVisible(!strText.isEmpty());

    if (strText.isEmpty())
    {
        setStyleSheet(m_strDefaultStyleSheet);
    }
    else
    {
        setStyleSheet(m_strEditingStyleSheet);
    }
}

void LineEditClear::setStyleSheets(const QString &strDefaultStyleSheet,
                                   const QString &strEditingStyleSheet)
{
    setStyleSheet(strDefaultStyleSheet);

    m_strDefaultStyleSheet = strDefaultStyleSheet;
    m_strEditingStyleSheet = strEditingStyleSheet;
}
