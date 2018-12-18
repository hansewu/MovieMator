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
