#include <QQmlContext>
#include "filterwidget.h"
#include "qmltypes/qmlutilities.h"
#include "qmltypes/qmlview.h"
#include "models/metadatamodel.h"
#include "models/attachedfiltersmodel.h"

#include "qmltypes/qmlmetadata.h"
#include "settings.h"
#include "mltcontroller.h"

FilterWidget::FilterWidget(MetadataModel* metadataModel, AttachedFiltersModel* attachedModel, QObject *parent)
    :QQuickWidget(QmlUtilities::sharedEngine(), (QWidget *)parent)
{
    QmlUtilities::setCommonProperties(rootContext());
    this->rootContext()->setContextProperty("view", new QmlView(quickWindow()));
    this->rootContext()->setContextProperty("metadatamodel", metadataModel);
    this->rootContext()->setContextProperty("attachedfiltersmodel", attachedModel);
    this->rootContext()->setContextProperty("filterWidget", this);
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setFocusPolicy(Qt::StrongFocus);
//    setCurrentFilter(0, 0, -1);
//    connect(quickWindow(), SIGNAL(sceneGraphInitialized()), SLOT(resetQview()));

//    m_producer = new Mlt::Producer(MLT.profile(), "/Users/gdb/Desktop/video/AVSEQ04.DAT");
    m_lastAttachedFilter = NULL;
}

FilterWidget::~FilterWidget()
{

}


void FilterWidget::previewFilter(QmlMetadata *meta)
{

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
        m_lastAttachedFilter = NULL;
    }

    m_lastAttachedFilter = new Mlt::Filter(MLT.profile(), meta->mlt_service().toUtf8().constData());
    m_producer->attach(*m_lastAttachedFilter);
    emit filterPreview(new Mlt::Producer(m_producer));
}

void FilterWidget::addFilterToTimeline(QmlMetadata *meta)
{
    Mlt::Producer *producer;
    Mlt::Filter* filter = new Mlt::Filter(MLT.profile(), meta->mlt_service().toUtf8().constData());
    producer = new Mlt::Producer(MLT.profile(), NULL, "noise");
    producer->attach(*filter);
    producer->set("meta.fx_cut", 1);
    producer->set("in", 0);
    producer->set("out", 100);

    emit filterAddToTimeline(producer);
    emit testsignals();
}
