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
#ifndef TWOLINEBUTTON_H
#define TWOLINEBUTTON_H
#include <QPushButton>

class TwoLineButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QString mainTitle READ mainTitle WRITE setMainTitle)
    Q_PROPERTY(QString subTitle READ subTitle WRITE setSubTitle)
    Q_PROPERTY(QString accessoryTitle READ accessoryTitle WRITE setAccessoryTitle)

public:
    TwoLineButton(QWidget *parent = Q_NULLPTR);
    ~TwoLineButton();


    void setMainTitle(const QString &strText);
    QString mainTitle() const;

    void setSubTitle(const QString &strText);
    QString subTitle() const;

    void setAccessoryTitle(const QString &strText);
    QString accessoryTitle() const;

protected:
      void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
      QString m_strMainTitle;
      QString m_strSubTitle;
      QString m_strAccessoryTitle;
};

#endif // TWOLINEBUTTON_H
