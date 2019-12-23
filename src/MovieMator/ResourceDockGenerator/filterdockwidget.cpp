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
    QString strJsFilePath = getQmlDirPath() + "/views/filter/translateTool.js";
    readTranslatJsFile(strJsFilePath);
}

UnsortMap<QString, BaseItemModel *> *FilterDockWidget::createAllClassesItemModel()
{
    UnsortMap<QString, BaseItemModel *> *pFilterDockListViewItemModel = new UnsortMap<QString, BaseItemModel *>;

    foreach (FilterInfo filterInfo, m_filtersInfo)
    {
        if (!pFilterDockListViewItemModel->contains(filterInfo.strClassification))
        {
            FilterItemModel *pNewFilterItemModel = new FilterItemModel(this);
            pFilterDockListViewItemModel->append(filterInfo.strClassification, pNewFilterItemModel);
        }

        FilterItemModel *pFilterItemModel = qobject_cast<FilterItemModel *>(pFilterDockListViewItemModel->value(filterInfo.strClassification));
        Q_ASSERT(pFilterItemModel);
        if (pFilterItemModel == nullptr)
        {
            return nullptr;
        }

        QStandardItem *pItem = new QStandardItem();

        QString strFileName = filterInfo.strName;
        if ((Settings.language() == "zh_CN") || (Settings.language() == "zh"))
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
        pFilterUserData->strPerviewFilePath = filterInfo.strPerviewSettingFilePath;
        QByteArray userDataByteArray;
        userDataByteArray.append(reinterpret_cast<char *>(pFilterUserData), sizeof(FilterUserData));
        pItem->setData(userDataByteArray, Qt::UserRole);

        pFilterItemModel->appendRow(pItem);
    }

    return pFilterDockListViewItemModel;
}

void FilterDockWidget::addItemToTimeline(const QStandardItem *pItem)
{
    QVariant userDataVariant        = pItem->data(Qt::UserRole);
    QByteArray userByteArray        = userDataVariant.value<QByteArray>();
    FilterUserData *pFilterUserData = reinterpret_cast<FilterUserData *>(userByteArray.data());

    Q_ASSERT(m_pMainInterface);
    if (m_pMainInterface)
    {
        m_pMainInterface->addFilter(pFilterUserData->nFilterIndex);
    }
}

void FilterDockWidget::preview(const QStandardItem *pItem)
{
    QVariant userDataVariant        = pItem->data(Qt::UserRole);
    QByteArray userByteArray        = userDataVariant.value<QByteArray>();
    FilterUserData *pFilterUserData = reinterpret_cast<FilterUserData *>(userByteArray.data());

    Q_ASSERT(m_pMainInterface);
    if (m_pMainInterface)
    {
        previewFilter(pFilterUserData->strPerviewFilePath);
    }
}

void FilterDockWidget::setFiltersInfo(QList<FilterInfo> filtersInfo)
{
    m_filtersInfo = filtersInfo;
}

void FilterDockWidget::readTranslatJsFile(QString jsFilePath)
{
    QString strResult = "";
    QFile scriptFile(jsFilePath);
    if (!scriptFile.open(QIODevice::ReadOnly))
    {
        strResult.clear();
        qDebug()<<"Errpr::js file open error!!!";
        return;
    }

    QTextStream out(&scriptFile);
    out.setCodec("UTF-8");
    QString strContents = out.readAll();
    scriptFile.close();

    m_jsEngine.evaluate(strContents);
}

QString FilterDockWidget::getQmlDirPath()
{
    QDir dir(qApp->applicationDirPath());

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
        dir.cdUp();
    #elif defined(Q_OS_MAC)
        dir.cdUp();
        dir.cd("Resources");
    #endif

    dir.cd("share");
    dir.cd("moviemator");
    dir.cd("qml");
    return dir.absolutePath();
}

void FilterDockWidget::previewFilter(QString strPerviewSettingFilePath)
{
    FILE_HANDLE mltSettingFile = m_pMainInterface->openFile(strPerviewSettingFilePath);
    m_pMainInterface->playFile(mltSettingFile);
    m_pMainInterface->destroyFileHandle(mltSettingFile);
}

static FilterDockWidget *pVideoDockInstance = nullptr;

QDockWidget *RDG_CreateVideoFilterDock(MainInterface *pMainInterface)
{
    if(pMainInterface && (pVideoDockInstance == nullptr))
    {
        pVideoDockInstance = new FilterDockWidget(0, pMainInterface);
    }
    return pVideoDockInstance;
}

void RDG_SetVideoFiltersInfo(const QList<FilterInfo> filtersInfo)
{
    Q_ASSERT(pVideoDockInstance);
    if (pVideoDockInstance == nullptr)
    {
        return;
    }

    pVideoDockInstance->setFiltersInfo(filtersInfo);
    pVideoDockInstance->setupUi();
}

static FilterDockWidget *pAudioDockInstance = nullptr;

QDockWidget *RDG_CreateAudioFilterDock(MainInterface *pMainInterface)
{
    if(pMainInterface && (pAudioDockInstance == nullptr))
    {
        pAudioDockInstance = new FilterDockWidget(1, pMainInterface);
    }
    return pAudioDockInstance;
}

void RDG_SetAudioFiltersInfo(const QList<FilterInfo> filtersInfo)
{
    Q_ASSERT(pAudioDockInstance);
    if (pAudioDockInstance == nullptr)
    {
        return;
    }

    pAudioDockInstance->setFiltersInfo(filtersInfo);
    pAudioDockInstance->setupUi();
}
