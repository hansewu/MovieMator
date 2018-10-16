#ifndef TEXTLISTDOCK_H
#define TEXTLISTDOCK_H

#include <QStandardItemModel>
#include <QDockWidget>
#include <QQuickWidget>
#include "../models/textmetadatamodel.h"

#include "qmltypes/qmlfilter.h"
#include "models/attachedfiltersmodel.h"

namespace Ui {
class TextlistDock;
}

class TextlistDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit TextlistDock(AttachedFiltersModel*model,  QWidget *parent = 0);
    ~TextlistDock();

    TextMetadataModel* model() {
         return &m_model;
     }

     void loadTextMetadata();
     void addTextToTimeline(int index);
     void showTextSettingWidget(Mlt::Producer *textProcucer);

signals:
     void selectedTextOnTimelineChanged(QmlFilter* filter, QmlMetadata* meta, int index);
     void currentTextAboutToChange();

 private:
     void addTextMetadata(QmlMetadata* meta);
     QmlMetadata *metadataForService(Mlt::Service *service);
private:
    Ui::TextlistDock *ui;
    TextMetadataModel m_model;
    AttachedFiltersModel* m_attachedmodel;

     QQuickWidget m_filterSettingsView;

//     QQuickWidget m_webvfxFilterView;


};

#endif // TEXTLISTDOCK_H
