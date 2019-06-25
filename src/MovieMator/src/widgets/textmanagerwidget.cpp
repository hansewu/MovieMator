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

#include <QQmlContext>
#include <QQmlComponent>
#include "textmanagerwidget.h"
#include <qmlutilities.h>
#include <qmlview.h>
#include "models/metadatamodel.h"
#include "models/attachedfiltersmodel.h"

#include "qmlmetadata.h"
#include "settings.h"
#include "mltcontroller.h"
#include "../mainwindow.h"

TextManagerWidget::TextManagerWidget(QObject *parent)
    :QQuickWidget(QmlUtilities::sharedEngine(), qobject_cast<QWidget *>(parent))
{
    loadTextMetadata();
    QmlUtilities::setCommonProperties(rootContext());
    this->rootContext()->setContextProperty("view", new QmlView(quickWindow()));
    this->rootContext()->setContextProperty("textmetadatamodel", &m_textmetadataModel);
//    this->rootContext()->setContextProperty("attachedfiltersmodel", attachedModel);
    this->rootContext()->setContextProperty("textManagerWidget", this);
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setFocusPolicy(Qt::StrongFocus);
//    setCurrentFilter(0, 0, -1);
//    connect(quickWindow(), SIGNAL(sceneGraphInitialized()), SLOT(resetQview()));

//    m_producer = new Mlt::Producer(MLT.profile(), "/Users/gdb/Desktop/video/AVSEQ04.DAT");
    m_lastAttachedFilter = nullptr;
}

TextManagerWidget::~TextManagerWidget()
{

}


void TextManagerWidget::previewText(QmlMetadata *meta)
{
    Q_UNUSED(meta)
//    static int lastAttachedIndex = -1;
//    //删除滤镜
//    if (lastAttachedIndex >= 0)
//    {
//        Mlt::Filter *filterToDetach = m_producer->filter(lastAttachedIndex);
//        m_producer->detach(*filterToDetach);
//        lastAttachedIndex = -1;
//    }


//    int n = m_producer->filter_count();
//        Mlt::Filter *filterToDetach = m_producer->filter(n - 1);
//        char *service = filterToDetach->get("mlt_service");
//        m_producer->detach(*filterToDetach);
//        delete filterToDetach;

    if (m_lastAttachedFilter)
    {
        m_producer->detach(*m_lastAttachedFilter);
        delete m_lastAttachedFilter;
        m_lastAttachedFilter = nullptr;
    }

//    m_lastAttachedFilter = new Mlt::Filter(MLT.profile(), meta->mlt_service().toUtf8().constData());
//    m_producer->attach(*m_lastAttachedFilter);
 //  emit textPreview(new Mlt::Producer(m_producer));
}

void TextManagerWidget::addTextToTimeline(QmlMetadata *textmeta)
{
    Q_UNUSED(textmeta)
    QmlMetadata *meta = m_textmetadataModel.get(0);
    Mlt::Producer *producer;
    Mlt::Filter* filter = new Mlt::Filter(MLT.profile(), meta->mlt_service().toUtf8().constData());
    producer = new Mlt::Producer(MLT.profile(), nullptr, "noise");
    producer->attach(*filter);
    producer->set("meta.fx_cut", 1);
    producer->set("in", 0);
    producer->set("out", 100);
    producer->set("resource", meta->name().toUtf8().constData());
    producer->set("thumbnail", meta->thumbnail().toUtf8().constData());

    MAIN.onTextAddToTimeline(producer);

 //   emit textAddToTimeline(producer);

}

void TextManagerWidget::loadTextMetadata() {
    QDir dir = QmlUtilities::qmlDir();
    dir.cd("text");
    foreach (QString dirName, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Executable)) {
        QDir subdir = dir;
        subdir.cd(dirName);
        subdir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
        subdir.setNameFilters(QStringList("textmeta*.qml"));
        foreach (QString fileName, subdir.entryList()) {
          //  LOG_DEBUG() << "reading filter metadata" << dirName << fileName;
            QQmlComponent component(QmlUtilities::sharedEngine(), subdir.absoluteFilePath(fileName));
            QmlMetadata *meta = qobject_cast<QmlMetadata*>(component.create());
            if (meta) {
                // Check if mlt_service is available.
                if (MLT.repository()->filters()->get_data(meta->mlt_service().toLatin1().constData())) {
                //    meta->loadSettings();
                 //   meta->setPath(subdir);
                //    meta->setParent(0);
                    addTextMetadata(meta);
                }
            } else if (!meta) {
      //          qDebug() << component.errorString();
            }
        }
    }
}


void TextManagerWidget::addTextMetadata(QmlMetadata* textmeta)
{
    m_textmetadataModel.addTextMetadata(textmeta);
}
