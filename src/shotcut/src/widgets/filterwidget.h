#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H
#include <QQuickWidget>
#include <Mlt.h>

class MetadataModel;
class AttachedFiltersModel;
class QmlMetadata;

class FilterWidget : public QQuickWidget
{
    Q_OBJECT

public:
    FilterWidget(MetadataModel* metadataModel, AttachedFiltersModel* attachedModel, QObject *parent = 0);
    ~FilterWidget();


signals:
    void filterPreview(void *producer);
    void filterAddToTimeline(void *producer);
    void testsignals();

public slots:
    void previewFilter(QmlMetadata *meta);
    void addFilterToTimeline(QmlMetadata *meta);

private:
    Mlt::Producer *m_producer;
    Mlt::Producer *m_filterProducer;
    Mlt::Filter *m_lastAttachedFilter;

};

#endif // FILTERWIDGET_H
