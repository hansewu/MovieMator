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
#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H
#include <QQuickWidget>
#include <Mlt.h>

class MetadataModel;
class AttachedFiltersModel;
class QmlMetadata;

class FilterWidget : public QQuickWidget
{
    Q_OBJECT

public:
    FilterWidget(MetadataModel* metadataModel, AttachedFiltersModel* attachedModel, QObject *parent = nullptr);
    ~FilterWidget();


signals:
    void filterPreview(void *producer);
    void filterAddToTimeline(void *producer);
    void testsignals();

public slots:
    void previewFilter(QmlMetadata *meta);
    void addFilterToTimeline(QmlMetadata *meta);

private:
    Mlt::Producer *m_producer;
    Mlt::Producer *m_filterProducer;
    Mlt::Filter *m_lastAttachedFilter;

};

#endif // FILTERWIDGET_H
