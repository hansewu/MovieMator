#ifndef CONFIGURATIONDOCK_H
#define CONFIGURATIONDOCK_H

#include "configurationdock_global.h"

#include <QDockWidget>
#include <QQuickWidget>

class QmlMetadata;

class CONFIGURATIONDOCKSHARED_EXPORT ConfigurationDock : public QDockWidget
{
    Q_OBJECT

    Q_PROPERTY(QRect dockPosition READ dockPosition NOTIFY dockPositionChanged)
public:
    explicit ConfigurationDock(QWidget *parent = 0);
    QRect dockPosition();

public slots:
    void setCurrentFilter(QObject* filter, QmlMetadata* meta, int index);

private slots:
    void resetQview();

private:
    QQuickWidget m_qview;

signals:
    void dockPositionChanged();
};

#endif // CONFIGURATIONDOCK_H
