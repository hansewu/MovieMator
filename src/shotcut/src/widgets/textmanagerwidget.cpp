#include <QQmlContext>
#include <QQmlComponent>
#include "textmanagerwidget.h"
#include <qmlutilities.h>
#include "qmltypes/qmlview.h"
#include "models/metadatamodel.h"
#include "models/attachedfiltersmodel.h"

#include "qmltypes/qmlmetadata.h"
#include "settings.h"
#include "mltcontroller.h"
#include "../mainwindow.h"

TextManagerWidget::TextManagerWidget(QObject *parent)
    :QQuickWidget(QmlUtilities::sharedEngine(), (QWidget *)parent)
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
    m_lastAttachedFilter = NULL;
}

TextManagerWidget::~TextManagerWidget()
{

}


void TextManagerWidget::previewText(QmlMetadata *meta)
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

//    m_lastAttachedFilter = new Mlt::Filter(MLT.profile(), meta->mlt_service().toUtf8().constData());
//    m_producer->attach(*m_lastAttachedFilter);
 //  emit textPreview(new Mlt::Producer(m_producer));
}

void TextManagerWidget::addTextToTimeline(QmlMetadata *textmeta)
{

    QmlMetadata *meta = m_textmetadataModel.get(0);
    Mlt::Producer *producer;
    Mlt::Filter* filter = new Mlt::Filter(MLT.profile(), meta->mlt_service().toUtf8().constData());
    producer = new Mlt::Producer(MLT.profile(), NULL, "noise");
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
