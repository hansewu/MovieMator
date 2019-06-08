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
#include "configurationdock.h"
#include <qmlutilities.h>
#include <QQmlContext>
#include <qmlview.h>
#include <qmlmetadata.h>
#include <QQmlEngine>
#include <QQuickItem>

ConfigurationDock::ConfigurationDock(QWidget *parent) : QDockWidget (tr("Configuration"), parent)
  , m_qview(QmlUtilities::sharedEngine(), this)
{
    setObjectName("ConfigurationDock");
    m_qview.setFocusPolicy(Qt::StrongFocus);
    setWidget(&m_qview);

    QmlUtilities::setCommonProperties(m_qview.rootContext());
    m_qview.rootContext()->setContextProperty("view", new QmlView(m_qview.quickWindow()));

    m_qview.rootContext()->setContextProperty("configurationDock", this);

//    connect(m_qview.quickWindow(), SIGNAL(sceneGraphInitialized()), SLOT(resetQview()));

    resetQview();
}

void ConfigurationDock::setCurrentFilter(QObject* filter, QmlMetadata* meta, int index)
{
    Q_UNUSED(filter);
    Q_UNUSED(meta);
//    m_qview.rootContext()->setContextProperty("filter", filter);
//    m_qview.rootContext()->setContextProperty("metadata", meta);
//    m_qview.setSource(meta->qmlFilePath());
    QMetaObject::invokeMethod(m_qview.rootObject(), "setCurrentFilter", Q_ARG(QVariant, QVariant(index)));
}

void ConfigurationDock::resetQview()
{
//    if (m_qview.status() != QQuickWidget::Null) {
//        QObject* root = m_qview.rootObject();
//        QObject::disconnect(root, SIGNAL(currentFilterRequested(int)),
//                            this, SIGNAL(currentFilterRequested(int)));

//        m_qview.setSource(QUrl(""));
//    }

    QDir viewPath = QmlUtilities::qmlDir();
    viewPath.cd("views");
    viewPath.cd("filter");
    m_qview.engine()->addImportPath(viewPath.path());

    QDir modulePath = QmlUtilities::qmlDir();
    modulePath.cd("modules");
    m_qview.engine()->addImportPath(modulePath.path());

    m_qview.setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qview.quickWindow()->setColor(palette().window().color());
    QUrl source = QUrl::fromLocalFile(viewPath.absoluteFilePath("FilterConfigView.qml"));
    m_qview.setSource(source);

//    QObject::connect(m_qview.rootObject(), SIGNAL(currentFilterRequested(int)),
//        SIGNAL(currentFilterRequested(int)));
//    emit currentFilterRequested(0);
}

QRect ConfigurationDock::dockPosition()
{
    return this->geometry();
}
