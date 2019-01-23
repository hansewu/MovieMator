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

#ifndef TEMPLATEEIDTOR_H
#define TEMPLATEEIDTOR_H

#include <QObject>
#include <Mlt.h>
#include <maininterface.h>

typedef struct {
    int trackIndex;
    int clipIndex;
    Mlt::Producer *producer;
} TEMPLATE_CLIP_INFO;

class TemplateEidtor : public QObject
{
    Q_OBJECT
public:
    explicit TemplateEidtor(QObject *parent = nullptr);
    ~TemplateEidtor();

    int replaceFileInTemplate(int index, Mlt::Producer *producer);
    FILE_HANDLE resetFileToTemplateDefault(int index);

signals:

public slots:
    int setProducer(Mlt::Producer *producer);

private:
    Mlt::Producer *m_templateProducer;
    QList<TEMPLATE_CLIP_INFO> m_clipList;
};

#endif // TEMPLATEEIDTOR_H
