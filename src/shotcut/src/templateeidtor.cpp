#include "templateeidtor.h"
#include "maininterface.h"
#include "mltcontroller.h"

TemplateEidtor::TemplateEidtor(QObject *parent)
    : QObject(parent)
    , m_templateProducer(0)
{

}

TemplateEidtor::~TemplateEidtor()
{

}


int TemplateEidtor::setProducer(Mlt::Producer *producer)
{
    m_templateProducer = producer;

    QList<Mlt::Producer *> replaceableClipList;

    if (m_templateProducer && m_templateProducer->is_valid())
    {
        char resource[512];
        strcpy(resource, m_templateProducer->get("resource"));
        m_templateProducer->set("mlt_type", "mlt_producer");
        m_templateProducer->set("resource", "<tractor>");
        Mlt::Tractor transitionTractor(*m_templateProducer);
        m_templateProducer->set("resource", resource);

        int trackCount = transitionTractor.count();

        for (int i = 0; i < trackCount; i++)
        {
            QScopedPointer<Mlt::Producer> track(transitionTractor.track(i));
            if (track) {
                Mlt::Playlist playlist(*track);
                int clipCount = playlist.count();
                for (int j = 0; j < clipCount; j++)
                {
                    QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(j));
                    //Mlt::ClipInfo *info = playlist.clip_info(j);
                    Mlt::Producer *clipProducer = info->producer;

                    QString mltService(clipProducer->parent().get("mlt_service"));
                    if (!mltService.isEmpty()
                            && mltService != "color"
                            && mltService != "colour"
                            && mltService != "blank"
                            && mltService != "tractor"
                            && mltService != "gifdec")
                    {
                        TEMPLATE_CLIP_INFO templateClipInfo;
                        templateClipInfo.trackIndex = i;
                        templateClipInfo.clipIndex = j;
                        //templateClipInfo.producer = new Mlt::Producer(clipProducer);
                        m_clipList.append(templateClipInfo);
                        replaceableClipList.append(clipProducer);
                    }
                }
            }
        }
        //test
        //resetFileToTemplateDefault(0);
    }

    //初始化替换模板文件界面
    //TemplateEditorDock_initTemplateEditor(replaceableClipList);

    //test
    //Mlt::Producer *newProducer = new Mlt::Producer(MLT.profile(), "C:\\Users\\gdbwin\\Videos\\Wildlife.wmv");
    //replaceFileInTemplate(1, newProducer);
    return 0;
}

int TemplateEidtor::replaceFileInTemplate(int index, Mlt::Producer *producer)
{
    Q_ASSERT(index >= 0 && index < m_clipList.count());
    Q_ASSERT(m_templateProducer != 0);

    //create a tractor
    char *resource = m_templateProducer->get("resource");
    m_templateProducer->set("resource", "<tractor>");
    Mlt::Tractor tractor(*m_templateProducer);
    m_templateProducer->set("resource", resource);

    TEMPLATE_CLIP_INFO clipInfo = m_clipList[index];
    QScopedPointer<Mlt::Producer> track(tractor.track(clipInfo.trackIndex));
    if (track)
    {
        Mlt::Playlist playlist(*track);
        QScopedPointer<Mlt::ClipInfo> originalPlaylistClipInfo(playlist.clip_info(clipInfo.clipIndex));
        Mlt::Producer *originalPlaylistClipProducer = originalPlaylistClipInfo->producer;
        //set same in/out/length as original
        producer->set("length", originalPlaylistClipInfo->frame_count);
        producer->set("in", originalPlaylistClipInfo->frame_in);
        producer->set("out", originalPlaylistClipInfo->frame_out);
        playlist.remove(clipInfo.clipIndex);
        playlist.insert(*producer, clipInfo.clipIndex, originalPlaylistClipInfo->frame_in, originalPlaylistClipInfo->frame_out);
        QScopedPointer<Mlt::ClipInfo> newPlaylistClipInfo(playlist.clip_info(clipInfo.clipIndex));
        Mlt::Producer *newPlaylistClipProducer = newPlaylistClipInfo->producer;
        //process transitions
        if (clipInfo.clipIndex > 0)
        {
            QScopedPointer<Mlt::ClipInfo> preClipInfo(playlist.clip_info(clipInfo.clipIndex - 1));
            Mlt::Producer *preClipProducer = preClipInfo->producer;
            QString mltService(preClipProducer->parent().get("mlt_service"));
            if (mltService == "tractor")
            {
                int transitionDuration = preClipInfo->frame_count;
                newPlaylistClipProducer->set("length", newPlaylistClipInfo->frame_count + transitionDuration);
                newPlaylistClipProducer->set("in", newPlaylistClipInfo->frame_in - transitionDuration);
                Mlt::Producer *bTrack = newPlaylistClipProducer->cut(newPlaylistClipInfo->frame_in - transitionDuration, newPlaylistClipInfo->frame_in - 1);
                Mlt::Tractor tractor(*preClipProducer);
                tractor.set_track(*bTrack, 1);
                //playlist.resize_clip(clipInfo.clipIndex, playlistClipInfo->frame_in + transitionDuration, playlistClipInfo->frame_out);
            }
        }
        if (clipInfo.clipIndex < playlist.count() - 1)
        {
            QScopedPointer<Mlt::ClipInfo> nextClipInfo(playlist.clip_info(clipInfo.clipIndex + 1));
            Mlt::Producer *nextClipProducer = nextClipInfo->producer;
            QString mltService(nextClipProducer->parent().get("mlt_service"));
            if (mltService == "tractor")
            {
                int transitionDuration = nextClipInfo->frame_count;
                newPlaylistClipProducer->set("length", newPlaylistClipInfo->frame_count + transitionDuration);
                newPlaylistClipProducer->set("out", newPlaylistClipInfo->frame_out + transitionDuration);
                Mlt::Producer *aTrack = newPlaylistClipProducer->cut(newPlaylistClipInfo->frame_out + 1, newPlaylistClipInfo->frame_out + transitionDuration);
                Mlt::Tractor tractor(*nextClipProducer);
                tractor.set_track(*aTrack, 0);
                //playlist.resize_clip(clipInfo.clipIndex, playlistClipInfo->frame_in, playlistClipInfo->frame_out - transitionDuration + 1);
            }
        }

        MLT.refreshConsumer();
    }
}

int TemplateEidtor::resetFileToTemplateDefault(int index)
{
    Q_ASSERT(index >= 0 && index < m_clipList.count());

    //create a new producer from template file
    char *templateFile = m_templateProducer->get("resource");
    Mlt::Producer *tractorProducer = new Mlt::Producer(MLT.profile(), templateFile);
    if (!tractorProducer || !tractorProducer->is_valid())
        return -1;
    tractorProducer->set("resource", "<tractor>");
    Mlt::Tractor tractor(*tractorProducer);

    TEMPLATE_CLIP_INFO clipInfo = m_clipList[index];
    QScopedPointer<Mlt::Producer> track(tractor.track(clipInfo.trackIndex));
    if (track)
    {
        Mlt::Playlist playlist(*track);
        QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipInfo.clipIndex));
        replaceFileInTemplate(index, info->producer);
    }
    delete tractorProducer;
}


