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
#ifndef TEXTLISTDOCK_H
#define TEXTLISTDOCK_H

#include <QStandardItemModel>
#include <QDockWidget>
#include <QQuickWidget>
#include "../models/textmetadatamodel.h"

#include "qmltypes/qmlfilter.h"
#include "models/attachedfiltersmodel.h"

namespace Ui {
class TextlistDock;
}

class TextlistDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit TextlistDock(AttachedFiltersModel*model,  QWidget *parent = nullptr);
    ~TextlistDock();

    TextMetadataModel* model() {
         return &m_model;
     }

     void loadTextMetadata();
     void addTextToTimeline(int index);
     void showTextSettingWidget(Mlt::Producer *textProcucer);

signals:
     void selectedTextOnTimelineChanged(QmlFilter* filter, QmlMetadata* meta, int index);
     void currentTextAboutToChange();

 private:
     void addTextMetadata(QmlMetadata* meta);
     QmlMetadata *metadataForService(Mlt::Service *service);
private:
    Ui::TextlistDock *ui;
    TextMetadataModel m_model;
    AttachedFiltersModel* m_attachedmodel;

     QQuickWidget m_filterSettingsView;

//     QQuickWidget m_webvfxFilterView;


};

#endif // TEXTLISTDOCK_H
