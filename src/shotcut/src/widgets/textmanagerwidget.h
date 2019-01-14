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

#ifndef TEXTMANAGERWIDGET_H
#define TEXTMANAGERWIDGET_H
//#include <Mlt.h>

//#include <QQuickWidget>

//class MetadataModel;
//class AttachedFiltersModel;
//class QmlMetadata;

//class QmlTextMetaData;


//class TextManagerWidget : public QQuickWidget
//{
//     Q_OBJECT
//public:
//    TextManagerWidget(MetadataModel* metadataModel, AttachedFiltersModel* attachedModel, QObject *parent = 0);
//    ~TextManagerWidget();



//public slots:
//    void previewText(QmlMetadata *meta);
//    void addTextToTimeline(QmlMetadata *meta);

//signals:
//    void textPreview(void *producer);
//    void textAddToTimeline(void *producer);
// //  void textToTimeline();

//private:
//    Mlt::Producer *m_producer;
//    Mlt::Producer *m_filterProducer;
//    Mlt::Filter *m_lastAttachedFilter;



//};
#include <QObject>
#include <QQuickWidget>
#include <Mlt.h>
#include "models/textmetadatamodel.h"
#include "qmltypes/qmltextmetadata.h"
//class MetadataModel;
//class AttachedFiltersModel;
//class QmlMetadata;

//class QmlTextMetaData;


class TextManagerWidget : public QQuickWidget
{
\

public:
    TextManagerWidget(QObject *parent = 0);
    ~TextManagerWidget();
    void loadTextMetadata();


signals:
    void textPreview(void *producer);
    void textAddToTimeline(void *producer);


public slots:
    void previewText(QmlMetadata *meta);
    void addTextToTimeline(QmlMetadata *meta);


private:
    void addTextMetadata(QmlMetadata* meta);
    Mlt::Producer *m_producer;
    Mlt::Producer *m_filterProducer;
    Mlt::Filter *m_lastAttachedFilter;
    TextMetadataModel m_textmetadataModel;

};

#endif // TEXTMANAGERWIDGET_H



