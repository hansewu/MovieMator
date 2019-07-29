#include "filterdockwidget.h"
#include "util.h"
#include "filteritemmodel.h"

#include <qdebug.h>
#include <qjsonobject.h>
#include <qdir.h>
#include <qapplication.h>
#include <settings.h>
#include <qstandardpaths.h>

FilterDockWidget::FilterDockWidget(int nFilterDockType, MainInterface *pMainInterface, QWidget *pParent) :
    BaseDockWidget(pParent),
    m_pMainInterface(pMainInterface),
    m_nFilterDockType(nFilterDockType)
{
    qDebug()<<"sll-----FilterDockWidget构造---start";
    QString strJsFilePath = getQmlDirPath() + "/views/filter/translateTool.js";
    readTranslatJsFile(strJsFilePath);
    qDebug()<<"sll-----FilterDockWidget构造---end";
}


QMap<QString, BaseItemModel *> *FilterDockWidget::createAllClassesItemModel()
{
    qDebug()<<"sll-----createAllClassesItemModel---start";
    QMap<QString, BaseItemModel *> *pFilterDockListViewItemModel = new QMap<QString, BaseItemModel *>;

    foreach (FilterInfo filterInfo, m_filtersInfo)
    {
        if (!pFilterDockListViewItemModel->contains(filterInfo.strClassification))
        {
            FilterItemModel *pNewFilterItemModel = new FilterItemModel(this);
            pFilterDockListViewItemModel->insert(filterInfo.strClassification, pNewFilterItemModel);
        }

        FilterItemModel *pFilterItemModel = qobject_cast<FilterItemModel *>(pFilterDockListViewItemModel->value(filterInfo.strClassification));
        Q_ASSERT(pFilterItemModel);
        if (pFilterItemModel == nullptr)
        {
            return nullptr;
        }

        QStandardItem *pItem = new QStandardItem();

        QString strFileName = filterInfo.strName;
        if (Settings.language() == "zh_CN")
        {
            QScriptValue transEn2ChFunc = m_jsEngine.globalObject().property("transEn2Ch");
            QScriptValueList args;
            args << QScriptValue(strFileName);
            strFileName = transEn2ChFunc.call(QScriptValue(), args).toString();
        }
        else
        {
            QScriptValue transEn2ShortFunc = m_jsEngine.globalObject().property("transEn2Short");
            QScriptValueList args;
            args << QScriptValue(strFileName);
            strFileName = transEn2ShortFunc.call(QScriptValue(), args).toString();
        }
        pItem->setText(strFileName);

        QIcon icon = QIcon(filterInfo.strThumbnailFilePath);
        pItem->setIcon(icon);

        QString strToolTip = strFileName;
        pItem->setToolTip(strToolTip);

        FilterUserData *pFilterUserData = new FilterUserData;
        pFilterUserData->nFilterIndex = filterInfo.nIndexOfMetadataModel;
        QByteArray userDataByteArray;
        userDataByteArray.append(reinterpret_cast<char *>(pFilterUserData), sizeof(pFilterUserData));
        pItem->setData(userDataByteArray, Qt::UserRole);

        pFilterItemModel->appendRow(pItem);
    }

    qDebug()<<"sll-----createAllClassesItemModel---end";

    return pFilterDockListViewItemModel;
}

bool FilterDockWidget::hasClass()
{
    qDebug()<<"sll-----hasClass---start";
    qDebug()<<"sll-----hasClass---end";
    return (m_nFilterDockType == 0);
}

void FilterDockWidget::addToTimeline(const QStandardItem *pItem)
{
    qDebug()<<"sll-----addToTimeline---start";

    QVariant userDataVariant        = pItem->data(Qt::UserRole);
    QByteArray userByteArray        = userDataVariant.value<QByteArray>();
    FilterUserData *pFilterUserData = reinterpret_cast<FilterUserData *>(userByteArray.data());

    qDebug()<<"sll-----nFilterIndex = "<<pFilterUserData->nFilterIndex;

    Q_ASSERT(m_pMainInterface);
    if (m_pMainInterface)
    {
        m_pMainInterface->addFilter(pFilterUserData->nFilterIndex);
    }

    qDebug()<<"sll-----addToTimeline---end";
}

void FilterDockWidget::preview(const QStandardItem *pItem)
{
    qDebug()<<"sll-----preview---start";

    QVariant userDataVariant        = pItem->data(Qt::UserRole);
    QByteArray userByteArray        = userDataVariant.value<QByteArray>();
    FilterUserData *pFilterUserData = reinterpret_cast<FilterUserData *>(userByteArray.data());

    qDebug()<<"sll-----nFilterIndex = "<<pFilterUserData->nFilterIndex;

    Q_ASSERT(m_pMainInterface);
    if (m_pMainInterface)
    {
        m_pMainInterface->previewFilter(pFilterUserData->nFilterIndex);
    }

    qDebug()<<"sll-----preview---end";
}

void FilterDockWidget::setFiltersInfo(QList<FilterInfo> filtersInfo)
{
    qDebug()<<"sll-----setFiltersInfo---start";
    m_filtersInfo = filtersInfo;
    qDebug()<<"sll-----setFiltersInfo---end";
}

void FilterDockWidget::readTranslatJsFile(QString jsFilePath) {
    QString strResult = "";
    QFile scriptFile(jsFilePath);
    if (!scriptFile.open(QIODevice::ReadOnly))
    {
        strResult.clear();
        qDebug()<<"sll-------------js file open error!!!";
        return;
    }

    QTextStream out(&scriptFile);
    out.setCodec("UTF-8");
    QString strContents = out.readAll();
    scriptFile.close();

    m_jsEngine.evaluate(strContents);
}

QString FilterDockWidget::getQmlDirPath() {
    QDir dir(qApp->applicationDirPath());

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
        dir.cdUp();
    #elif defined(Q_OS_MAC)
        dir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
    #endif

    dir.cd("share");
    dir.cd("moviemator");
    dir.cd("qml");
    return dir.absolutePath();
}

QDockWidget *g_createFilterDock(MainInterface *pMainInterface, int nFilterDockType)
{
    qDebug()<<"sll-----g_createFilterDock---start";
    qDebug()<<"sll-----nFilterDockType = "<<nFilterDockType;
    qDebug()<<"sll-----g_createFilterDock---end";
    return new FilterDockWidget(nFilterDockType, pMainInterface);
}

void g_setFiltersInfo(QDockWidget *pDockWidget, const QList<FilterInfo> filtersInfo)
{
    qDebug()<<"sll-----g_setFiltersInfo---start";
    Q_ASSERT(pDockWidget);
    if (pDockWidget == nullptr)
    {
        return;
    }

    FilterDockWidget *pFilterDock = qobject_cast<FilterDockWidget *>(pDockWidget);
    pFilterDock->setFiltersInfo(filtersInfo);
    pFilterDock->setupUi();
    qDebug()<<"sll-----g_setFiltersInfo---end";
}
