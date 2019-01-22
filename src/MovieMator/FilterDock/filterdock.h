#ifndef FILTERDOCK_H
#define FILTERDOCK_H
#include <QDockWidget>
#include <QQuickWidget>
#include <maininterface.h>
#include "filterdock_global.h"
#include "filterdockinterface.h"


class FilterItemInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filterType READ filterType WRITE setFilterType)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString imageSourcePath READ imageSourcePath WRITE setImageSourcePath)
    Q_PROPERTY(QString visible READ visible WRITE setVisible)

public:

    explicit FilterItemInfo(QObject *parent = NULL){}

    Q_INVOKABLE QString filterType() const { return m_filterType; }
    Q_INVOKABLE void setFilterType(const QString strType){m_filterType = strType;}
    Q_INVOKABLE QString name() const { return m_name; }
    Q_INVOKABLE void setName(const QString strName){m_name = strName;}
    Q_INVOKABLE QString imageSourcePath() const { return m_imageSourcePath; }
    Q_INVOKABLE void setImageSourcePath(const QString strImageSourcePath){m_imageSourcePath = strImageSourcePath;}
    Q_INVOKABLE QString visible() const { return m_bVisible; }
    Q_INVOKABLE void setVisible(const QString visible){m_bVisible = visible;}

private:
    QString m_filterType;
    QString m_name;
    QString m_imageSourcePath;
    QString m_bVisible;
};

class FiltersInfo: public QObject
{
    Q_OBJECT
public:
    FiltersInfo(QList<FilterItemInfo*>* inList){m_filterInfoList = inList;}
    ~FiltersInfo(){}

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QHash<int, QByteArray> roleNames() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    Q_INVOKABLE int rowCount(){return m_filterInfoList->size();}
    Q_INVOKABLE FilterItemInfo* get(int index) const {return m_filterInfoList->at(index);}

private:
    typedef QList<FilterItemInfo*> FiltersList;
    FiltersList * m_filterInfoList;
};

class FILTERDOCKSHARED_EXPORT FilterDock: public QDockWidget
{
    Q_OBJECT

public:
    explicit FilterDock(MainInterface *main=0, QWidget *parent = 0);
    ~FilterDock();

    int UpdateFilters();
    void resetQview();
    Q_INVOKABLE void addFilterItem(int index);

private:
    MainInterface *m_mainWindow;
    QQuickWidget m_qview;
    FiltersInfo *m_pFilterInfo;
};



#endif // FILTERDOCK_H

