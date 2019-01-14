/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 * Author: Brian Matherly <code@brianmatherly.com>
 *
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
#include <qmlutilities.h>
#include <qmlview.h>
#include "models/metadatamodel.h"
#include "models/attachedfiltersmodel.h"
#include "mltcontroller.h"
#include "mainwindow.h"

FiltersDock::FiltersDock(MetadataModel* metadataModel, AttachedFiltersModel* attachedModel, QWidget *parent) :
    QDockWidget(tr("Filters"), parent),
    m_quickView(QmlUtilities::sharedEngine(), qobject_cast<QWindow *>(this))
//    m_qview(QmlUtilities::sharedEngine(), this)
{
    LOG_DEBUG() << "begin";
    setObjectName("FiltersDock");
    QIcon filterIcon = QIcon::fromTheme("view-filter", QIcon(":/icons/oxygen/32x32/actions/view-filter.png"));
    setWindowIcon(filterIcon);
    toggleViewAction()->setIcon(windowIcon());
    setMinimumWidth(300);
//    m_qview.setFocusPolicy(Qt::StrongFocus);
//    setWidget(&m_qview);

    m_containerView = QWidget::createWindowContainer(&m_quickView, this);
    m_containerView->setFocusPolicy(Qt::StrongFocus);
    setWidget(m_containerView);

    QmlUtilities::setCommonProperties(m_quickView.rootContext());
    m_quickView.rootContext()->setContextProperty("view", new QmlView(&m_quickView));
    m_quickView.rootContext()->setContextProperty("metadatamodel", metadataModel);
    m_quickView.rootContext()->setContextProperty("attachedfiltersmodel", attachedModel);
    m_quickView.rootContext()->setContextProperty("filterDock", this);

//    QmlUtilities::setCommonProperties(m_qview.rootContext());
////    m_qview.rootContext()->setContextProperty("mainwindow", &MAIN);
//    m_qview.rootContext()->setContextProperty("view", new QmlView(m_qview.quickWindow()));
//    m_qview.rootContext()->setContextProperty("metadatamodel", metadataModel);
//    m_qview.rootContext()->setContextProperty("attachedfiltersmodel", attachedModel);

//    m_qview.rootContext()->setContextProperty("filterDock", this);

    setCurrentFilter(0, 0, -1);
//    connect(m_qview.quickWindow(), SIGNAL(sceneGraphInitialized()), SLOT(resetQview()));
//    connect(&m_quickView, SIGNAL(sceneGraphInitialized()), SLOT(resetQview()));
    resetQview();

    LOG_DEBUG() << "end";
}

void FiltersDock::clearCurrentFilter()
{
    MLT.pause();
    disconnect(this, SIGNAL(positionChanged()), 0, 0);
//    m_qview.rootContext()->setContextProperty("metadata", 0);
//    QMetaObject::invokeMethod(m_qview.rootObject(), "clearCurrentFilter");
    m_quickView.rootContext()->setContextProperty("metadata", 0);
    QMetaObject::invokeMethod(m_quickView.rootObject(), "clearCurrentFilter");
    disconnect(this, SIGNAL(changed()));
    m_qmlFilter = NULL;
}

void FiltersDock::setCurrentFilter(QObject* filter, QmlMetadata* meta, int index)
{
    QmlFilter *qmlFilter = (QmlFilter *)filter;
//    m_qview.rootContext()->setContextProperty("filter", qmlFilter);
//    m_qview.rootContext()->setContextProperty("metadata", meta);
//    QMetaObject::invokeMethod(m_qview.rootObject(), "setCurrentFilter", Q_ARG(QVariant, QVariant(index)));
    m_quickView.rootContext()->setContextProperty("filter", qmlFilter);
    m_quickView.rootContext()->setContextProperty("metadata", meta);
    QMetaObject::invokeMethod(m_quickView.rootObject(), "setCurrentFilter", Q_ARG(QVariant, QVariant(index)));
    if (qmlFilter)
    {
     //   qDebug()<<"filter m_path is "<<filter->path();
        connect(qmlFilter, SIGNAL(changed()), SIGNAL(changed()));
    }
    m_qmlFilter = qmlFilter;

    emit currentFilterChanged();
}

void FiltersDock::setFadeInDuration(int duration)
{
//    QObject* filterUi = m_qview.rootObject()->findChild<QObject*>("fadeIn");
    QObject* filterUi = m_quickView.rootObject()->findChild<QObject*>("fadeIn");
    if (filterUi) {
        filterUi->setProperty("duration", duration);
    }
}

void FiltersDock::setFadeOutDuration(int duration)
{
//    QObject* filterUi = m_qview.rootObject()->findChild<QObject*>("fadeOut");
    QObject* filterUi = m_quickView.rootObject()->findChild<QObject*>("fadeOut");
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
//    if (m_qview.status() != QQuickWidget::Null) {
//        QObject* root = m_qview.rootObject();
//        QObject::disconnect(root, SIGNAL(currentFilterRequested(int)),
//                            this, SIGNAL(currentFilterRequested(int)));

//        m_qview.setSource(QUrl(""));
//    }

    if (m_quickView.status() != QQuickView::Null) {
        QObject* root = m_quickView.rootObject();
        QObject::disconnect(root, SIGNAL(currentFilterRequested(int)),
                            this, SIGNAL(currentFilterRequested(int)));

        m_quickView.setSource(QUrl(""));
    }

    QDir viewPath = QmlUtilities::qmlDir();
    viewPath.cd("views");
    viewPath.cd("filter");
//    m_qview.engine()->addImportPath(viewPath.path());
    m_quickView.engine()->addImportPath(viewPath.path());

    QDir modulePath = QmlUtilities::qmlDir();
    modulePath.cd("modules");
//    m_qview.engine()->addImportPath(modulePath.path());
    m_quickView.engine()->addImportPath(modulePath.path());

//    m_qview.setResizeMode(QQuickWidget::SizeRootObjectToView);
//    m_qview.quickWindow()->setColor(palette().window().color());
    m_quickView.setResizeMode(QQuickView::SizeRootObjectToView);
    m_quickView.setColor(palette().window().color());
    QUrl source = QUrl::fromLocalFile(viewPath.absoluteFilePath("filterview.qml"));
//    m_qview.setSource(source);
    m_quickView.setSource(source);

//    QObject::connect(m_qview.rootObject(), SIGNAL(currentFilterRequested(int)),
//        SIGNAL(currentFilterRequested(int)));
    QObject::connect(m_quickView.rootObject(), SIGNAL(currentFilterRequested(int)),
        SIGNAL(currentFilterRequested(int)));
    emit currentFilterRequested(0);

    LOG_DEBUG() << "end";
}

//#ifdef MOVIEMATOR_PRO
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
//#endif



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

void FiltersDock::setExtraQmlContextProperty(QString name, QObject *object)
{
//    m_qview.rootContext()->setContextProperty(name, object);
    m_quickView.rootContext()->setContextProperty(name, object);
}


QRect FiltersDock::dockPosition()
{
    return this->geometry();
}


