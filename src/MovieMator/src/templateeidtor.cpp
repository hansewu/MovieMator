/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "templateeidtor.h"
#include "maininterface.h"
#include "mltcontroller.h"
#include <templateeditordockinterface.h>

TemplateEidtor::TemplateEidtor(QObject *parent)
    : QObject(parent)
    , m_templateProducer(0)
{

}

TemplateEidtor::~TemplateEidtor()
{
    if (m_templateProducer)
        delete  m_templateProducer;
}


int TemplateEidtor::setProducer(Mlt::Producer *producer)
{
    if (m_templateProducer)
        delete m_templateProducer;
    m_templateProducer = new Mlt::Producer(producer);

    QList<FILE_HANDLE> replaceableClipList;

    Q_ASSERT(m_templateProducer && m_templateProducer->is_valid());
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
                        replaceableClipList.append((FILE_HANDLE)new Mlt::Producer(clipProducer));
                    }
                }
            }
        }
        //test
        //resetFileToTemplateDefault(0);
    }

    //初始化替换模板文件界面
    TemplateEditorDock_initTemplateEditor(replaceableClipList);

    //test
    //Mlt::Producer *newProducer = new Mlt::Producer(MLT.profile(), "C:\\Users\\gdbwin\\Videos\\Wildlife.wmv");
    //replaceFileInTemplate(1, newProducer);
    return 0;
}

int TemplateEidtor::replaceFileInTemplate(int index, Mlt::Producer *producer)
{
    Q_ASSERT(index >= 0 && index < m_clipList.count());
    Q_ASSERT(m_templateProducer && m_templateProducer->is_valid());

    int ret = -1;
    if (index < 0 || index > m_clipList.count() - 1)
        return ret;

    if (!m_templateProducer || !m_templateProducer->is_valid())
        return ret;
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
        if (originalPlaylistClipInfo)
        {
            Mlt::Producer *originalPlaylistClipProducer = originalPlaylistClipInfo->producer;
            //set same in/out/length as original
            producer->set("length", originalPlaylistClipInfo->frame_count);
            producer->set("in", originalPlaylistClipInfo->frame_in);
            producer->set("out", originalPlaylistClipInfo->frame_out);
            MLT.copyFilters(*originalPlaylistClipProducer, *producer);
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
            ret = 0;
        }
    }
    return ret;
}

FILE_HANDLE TemplateEidtor::resetFileToTemplateDefault(int index)
{
    Q_ASSERT(index >= 0 && index < m_clipList.count());

    Mlt::Producer *resultProducer = 0;
    //create a new producer from template file
    char *templateFile = m_templateProducer->get("resource");
    Mlt::Producer *tractorProducer = new Mlt::Producer(MLT.profile(), templateFile);
    Q_ASSERT(tractorProducer && tractorProducer->is_valid());
    if (!tractorProducer || !tractorProducer->is_valid())
        return 0;
    tractorProducer->set("resource", "<tractor>");
    Mlt::Tractor tractor(*tractorProducer);

    TEMPLATE_CLIP_INFO clipInfo = m_clipList[index];
    QScopedPointer<Mlt::Producer> track(tractor.track(clipInfo.trackIndex));
    if (track)
    {
        Mlt::Playlist playlist(*track);
        QScopedPointer<Mlt::ClipInfo> info(playlist.clip_info(clipInfo.clipIndex));
        Q_ASSERT(info);
        if (info)
        {
            int ret = replaceFileInTemplate(index, info->producer);
            if (ret == 0)
                resultProducer = new Mlt::Producer(info->producer);
        }
    }
    delete tractorProducer;

    return resultProducer;
}


