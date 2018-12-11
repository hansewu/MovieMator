/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 * Author: Brian Matherly <code@brianmatherly.com>
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

#include "filtersdock.h"
#include <QQuickView>
#include <QQuickItem>
#include <QtWidgets/QScrollArea>
#include <QQmlEngine>
#include <QDir>
#include <QUrl>
#include <QQmlContext>
#include <QAction>
#include <QIcon>
#include <Logger.h>
#include "qmltypes/qmlfilter.h"
#include "qmltypes/qmlutilities.h"
#include "qmltypes/qmlview.h"
#include "models/metadatamodel.h"
#include "models/attachedfiltersmodel.h"
#include "mltcontroller.h"
#include "mainwindow.h"

FiltersDock::FiltersDock(MetadataModel* metadataModel, AttachedFiltersModel* attachedModel, QWidget *parent) :
    QDockWidget(tr("Filters"), parent),
    m_qview(QmlUtilities::sharedEngine(), this)
{
    LOG_DEBUG() << "begin";
    setObjectName("FiltersDock");
    QIcon filterIcon = QIcon::fromTheme("view-filter", QIcon(":/icons/oxygen/32x32/actions/view-filter.png"));
    setWindowIcon(filterIcon);
    toggleViewAction()->setIcon(windowIcon());
    setMinimumWidth(300);
    m_qview.setFocusPolicy(Qt::StrongFocus);
    setWidget(&m_qview);

    QmlUtilities::setCommonProperties(m_qview.rootContext());
    m_qview.rootContext()->setContextProperty("view", new QmlView(m_qview.quickWindow()));
    m_qview.rootContext()->setContextProperty("metadatamodel", metadataModel);
    m_qview.rootContext()->setContextProperty("attachedfiltersmodel", attachedModel);

    m_qview.rootContext()->setContextProperty("filterDock", this);

    setCurrentFilter(0, 0, -1);
    connect(m_qview.quickWindow(), SIGNAL(sceneGraphInitialized()), SLOT(resetQview()));

    LOG_DEBUG() << "end";
}

void FiltersDock::clearCurrentFilter()
{
    MLT.pause();
    disconnect(this, SIGNAL(positionChanged()), 0, 0);
    m_qview.rootContext()->setContextProperty("metadata", 0);
    QMetaObject::invokeMethod(m_qview.rootObject(), "clearCurrentFilter");
    disconnect(this, SIGNAL(changed()));
    m_qmlFilter = NULL;

}

void FiltersDock::setCurrentFilter(QmlFilter* filter, QmlMetadata* meta, int index)
{
    m_qview.rootContext()->setContextProperty("filter", filter);
    m_qview.rootContext()->setContextProperty("metadata", meta);
    QMetaObject::invokeMethod(m_qview.rootObject(), "setCurrentFilter", Q_ARG(QVariant, QVariant(index)));
    if (filter)
    {
     //   qDebug()<<"filter m_path is "<<filter->path();
        connect(filter, SIGNAL(changed()), SIGNAL(changed()));
    }
    m_qmlFilter = filter;
}

void FiltersDock::setFadeInDuration(int duration)
{
    QObject* filterUi = m_qview.rootObject()->findChild<QObject*>("fadeIn");
    if (filterUi) {
        filterUi->setProperty("duration", duration);
    }
}

void FiltersDock::setFadeOutDuration(int duration)
{
    QObject* filterUi = m_qview.rootObject()->findChild<QObject*>("fadeOut");
    if (filterUi) {
        filterUi->setProperty("duration", duration);
    }
}

 void FiltersDock::onChangePosition()
 {
    if(m_qmlFilter)
        emit positionChanged();
 }
bool FiltersDock::event(QEvent *event)
{
    bool result = QDockWidget::event(event);
//    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange) {
//        resetQview();
//    }
    return result;
}

void FiltersDock::resetQview()
{
    LOG_DEBUG() << "begin";
    if (m_qview.status() != QQuickWidget::Null) {
        QObject* root = m_qview.rootObject();
        QObject::disconnect(root, SIGNAL(currentFilterRequested(int)),
                            this, SIGNAL(currentFilterRequested(int)));

        m_qview.setSource(QUrl(""));
    }

    QDir viewPath = QmlUtilities::qmlDir();
    viewPath.cd("views");
    viewPath.cd("filter");
    m_qview.engine()->addImportPath(viewPath.path());

    QDir modulePath = QmlUtilities::qmlDir();
    modulePath.cd("modules");
    m_qview.engine()->addImportPath(modulePath.path());

    m_qview.setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qview.quickWindow()->setColor(palette().window().color());
    QUrl source = QUrl::fromLocalFile(viewPath.absoluteFilePath("filterview.qml"));
    m_qview.setSource(source);

    QObject::connect(m_qview.rootObject(), SIGNAL(currentFilterRequested(int)),
        SIGNAL(currentFilterRequested(int)));
    emit currentFilterRequested(0);

    LOG_DEBUG() << "end";
}

#ifdef MOVIEMATOR_PRO
void FiltersDock::setCurrentClipFrameFromTimeline(int position)
{
    m_position = position;
    emit changePosition(position);

}

void FiltersDock::setPosition(int position)
{
    m_position = position;
    emit changePosition(position);
}
#endif



//int FiltersDock::getCurrentPosition()
//{
//    return m_position;
//}

bool FiltersDock::proVersion()
{
#if MOVIEMATOR_FREE
    return false;
#endif
    return true;
}


void FiltersDock::promptForUpgrade()
{
    //专业版功能，提示升级
    MAIN.showProFeaturePromptDialog();
}



