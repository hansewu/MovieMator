/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: WanYuanCN <ebthon@hotmail.com>
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

#ifndef EFFECTLISTVIEW_H
#define EFFECTLISTVIEW_H

#include <QListView>

class EffectListView : public QListView
{
public:
    EffectListView(QWidget *parent = nullptr);
    ~EffectListView();
    void setMimeData(QMimeData *mimeData, const QString &mimeType);

    void keyPressEvent(QKeyEvent*);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    QPoint m_dragStart;
    bool m_canStartDrag;

    QMimeData *m_mimeData;
    QString m_mimeType;
};

#endif // EFFECTLISTVIEW_H
