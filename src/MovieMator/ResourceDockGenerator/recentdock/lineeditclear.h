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

#ifndef LINEEDITCLEAR_H
#define LINEEDITCLEAR_H

#include <QLineEdit>

class LineEditClear : public QLineEdit
{
    Q_OBJECT

public:
    LineEditClear(QWidget *pParent = nullptr);
    // 设定 lineEdit的样式表为 defaultStyleSheet
    // 设置 m_defaultStyleSheet的值为 defaultStyleSheet
    // 设置 m_editingStyleSheet的值为 editingStyleSheet
    void setStyleSheets(const QString &strDefaultStyleSheet,
                        const QString &strEditingStyleSheet);

protected:
    // 样式表的大小变化事件
    void resizeEvent(QResizeEvent *);

private slots:
    // 切换 lineEdit中的清除按钮样式
    // 并切换 lineEdit的样式
    void onTextChanged(const QString &strText);

private:
    // 清除 lineEdit内容按钮
    QToolButton *m_pClearButton;
    // 默认样式表
    QString m_strDefaultStyleSheet;
    // 编辑 lineEdit内容时的样式表
    QString m_strEditingStyleSheet;
};

#endif // LINEEDITCLEAR_H
