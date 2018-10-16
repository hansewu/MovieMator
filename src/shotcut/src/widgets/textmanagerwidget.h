#ifndef TEXTMANAGERWIDGET_H
#define TEXTMANAGERWIDGET_H
//#include <Mlt.h>

//#include <QQuickWidget>

//class MetadataModel;
//class AttachedFiltersModel;
//class QmlMetadata;

//class QmlTextMetaData;


//class TextManagerWidget : public QQuickWidget
//{
//     Q_OBJECT
//public:
//    TextManagerWidget(MetadataModel* metadataModel, AttachedFiltersModel* attachedModel, QObject *parent = 0);
//    ~TextManagerWidget();



//public slots:
//    void previewText(QmlMetadata *meta);
//    void addTextToTimeline(QmlMetadata *meta);

//signals:
//    void textPreview(void *producer);
//    void textAddToTimeline(void *producer);
// //  void textToTimeline();

//private:
//    Mlt::Producer *m_producer;
//    Mlt::Producer *m_filterProducer;
//    Mlt::Filter *m_lastAttachedFilter;



//};
#include <QObject>
#include <QQuickWidget>
#include <Mlt.h>
#include "models/textmetadatamodel.h"
#include "qmltypes/qmltextmetadata.h"
//class MetadataModel;
//class AttachedFiltersModel;
//class QmlMetadata;

//class QmlTextMetaData;


class TextManagerWidget : public QQuickWidget
{
\

public:
    TextManagerWidget(QObject *parent = 0);
    ~TextManagerWidget();
    void loadTextMetadata();


signals:
    void textPreview(void *producer);
    void textAddToTimeline(void *producer);


public slots:
    void previewText(QmlMetadata *meta);
    void addTextToTimeline(QmlMetadata *meta);


private:
    void addTextMetadata(QmlMetadata* meta);
    Mlt::Producer *m_producer;
    Mlt::Producer *m_filterProducer;
    Mlt::Filter *m_lastAttachedFilter;
    TextMetadataModel m_textmetadataModel;

};

#endif // TEXTMANAGERWIDGET_H



