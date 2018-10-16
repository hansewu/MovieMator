#include "textlistdock.h"
#include "ui_textlistdock.h"
#include <QQmlContext>
#include <QQmlComponent>
#include "mainwindow.h"
#include "settings.h"
#include "qmltypes/qmlutilities.h"
#include "qmltypes/qmltextmetadata.h"
#include "shotcut_mlt_properties.h"
#include <commands/playlistcommands.h>
#include <QMenu>
#include <Logger.h>
#include <QUrl>
#include "qmltypes/qmlfilter.h"


TextlistDock::TextlistDock(AttachedFiltersModel *model, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TextlistDock),
    m_filterSettingsView(QmlUtilities::sharedEngine(), 0)
//    m_webvfxFilterView(QmlUtilities::sharedEngine(), 0)
{
    ui->setupUi(this);
    m_attachedmodel = model;

    ui->listView->setViewMode(QListView::IconMode);

    ui->listView->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->listView->setModel(&m_model);
    //   ui->listView->setModelColumn(3);
    ui->listView->setDragEnabled(true);
    ui->listView->setDragDropMode(QAbstractItemView::DragOnly);

    ui->listView->setStyleSheet("selection-background-color: rgb(57,156,221)");

    loadTextMetadata();

}

TextlistDock::~TextlistDock()
{
    delete ui;
}

void TextlistDock::loadTextMetadata() {
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
                    meta->setPath(subdir);
                    meta->setParent(0);
                    addTextMetadata(meta);
                }
            } else if (!meta) {
                qDebug() << component.errorString();
            }
        }
    }
}

void TextlistDock::addTextMetadata(QmlMetadata* textmeta)
{
    m_model.addTextMetadata(textmeta);
}


void TextlistDock::addTextToTimeline(int index)
{

   // qDebug()<<"text index is " << index;
    QmlMetadata *meta = m_model.get(index);

//    m_attachedTextModel.add(meta);

    Mlt::Producer *producer;

    Mlt::Filter* mltfilter = new Mlt::Filter(MLT.profile(), meta->mlt_service().toUtf8().constData());
    mltfilter->set(kShotcutFilterProperty, meta->uniqueId().toUtf8().constData());



    QmlFilter *qmlFilter = new QmlFilter(mltfilter, meta);
    qmlFilter->setIsNew(true);



    m_filterSettingsView.rootContext()->setContextProperty("filter", qmlFilter);
  //  m_filterSettingsView.rootContext()->setContextProperty("time1", m_time);
    m_filterSettingsView.setGeometry(0,0,800, 600);
    m_filterSettingsView.setSource(meta->qmlFilePath());
    m_filterSettingsView.show();

//    m_webvfxFilterView.setSource(QUrl("/work/Projects/MacVideoEditor/trunk/shotcut/src/qml/text/9/Filter.qml"));
//    m_webvfxFilterView.rootContext()->setContextProperty("filter1", qmlFilter);



    emit currentTextAboutToChange();
    emit selectedTextOnTimelineChanged(qmlFilter, meta, 0);



    producer = new Mlt::Producer(MLT.profile(), NULL, "noise");
    producer->attach(*mltfilter);

    producer->set("meta.fx_cut", 1);

    producer->set("in", 0);
    producer->set("out", 100);
    producer->set("resource", meta->name().toUtf8().constData());
    producer->set("thumbnail", meta->thumbnail().toUtf8().constData());


    MAIN.onTextAddToTimeline(producer);

//    emit textAddToTimeline(producer);

}

QmlMetadata *TextlistDock::metadataForService(Mlt::Service *service)
{
    QmlMetadata* meta = 0;
    int rowCount = m_model.rowCount();
    QString uniqueId = service->get(kShotcutFilterProperty);

    // Fallback to mlt_service for legacy filters
    if (uniqueId.isEmpty()) {
        uniqueId = service->get("mlt_service");
    }

    for (int i = 0; i < rowCount; i++) {
        QmlMetadata* tmpMeta = m_model.get(i);
        if (tmpMeta->uniqueId() == uniqueId) {
            meta = tmpMeta;
            break;
        }
    }

    return meta;

}

void TextlistDock::showTextSettingWidget(Mlt::Producer *textProcucer)
{
//    int trackIndex = 2;
//    Mlt::Producer *producer = producerForClip(trackIndex, textclipIndex);

    Mlt::Filter *filter = textProcucer->filter(0);
    QmlMetadata *metadata = metadataForService(filter);
    QmlFilter *qmlFilter = new QmlFilter(filter, metadata);
 //   qmlFilter->setIsNew(true);
    m_filterSettingsView.rootContext()->setContextProperty("filter", qmlFilter);
//    m_filterSettingsView.setGeometry(0,0,800, 600);
    m_filterSettingsView.setSource(metadata->qmlFilePath());
    m_filterSettingsView.show();
    emit selectedTextOnTimelineChanged(qmlFilter, metadata, 0);



}
