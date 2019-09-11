/*
 * Copyright (c) 2011-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
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

#include "mltcontroller.h"
#include <QWidget>
#include <QPalette>
#include <QMetaType>
#include <QFileInfo>
#include <QUuid>
#include <Logger.h>
#include <Mlt.h>
#include "glwidget.h"
#include "settings.h"
#include "shotcut_mlt_properties.h"
//#include "mltqtmodule.h"
#include "qmlutilities.h"
#include <util.h>

namespace Mlt {

static Controller* instance = nullptr;
const QString XmlMimeType("application/mlt+xml");

static int alignWidth(int width)
{
    return (width + 7) / 8 * 8;
}

Controller::Controller()
    : m_producer(nullptr)
    , m_consumer(nullptr)
    , m_jackFilter(nullptr)
    , m_volume(1.0)
{
    LOG_DEBUG() << "begin";
    m_repo = Mlt::Factory::init();
 //   mlt_register_qtmodule( mlt_factory_repository() );
    m_profile = new Mlt::Profile("atsc_720p_24");
    Q_ASSERT(m_profile);
    updateAvformatCaching(0);
    LOG_DEBUG() << "end";
}

Controller& Controller::singleton(QObject *parent)
{
    if (!instance) {
        qRegisterMetaType<Mlt::Frame>("Mlt::Frame");
        qRegisterMetaType<SharedFrame>("SharedFrame");
        instance = new GLWidget(parent);
    }
    return *instance;
}

Controller::~Controller()
{
    close();
    closeConsumer();
    delete m_profile;
    m_profile = nullptr;
}

void Controller::destroy()
{
    delete instance;
}

int Controller::setProducer(Mlt::Producer* producer, bool)
{
    int error = 0;

    if (producer != m_producer)
        close();
    if (producer && producer->is_valid()) {
        m_producer = producer;
    }
    else {
        // Cleanup on error
        error = 1;
        delete producer;
    }
    return error;
}

int Controller::open(const QString &url)
{
    LOG_DEBUG() << url;
    int error = 0;

    close();

    if (Settings.playerGPU() && !profile().is_explicit())
        // Prevent loading normalizing filters, which might be Movit ones that
        // may not have a proper OpenGL context when requesting a sample frame.
    {
        qDebug()<<"open GPU";
        m_producer = new Mlt::Producer(profile(), "abnormal", url.toUtf8().constData());
        Q_ASSERT(m_producer);
    }
    else
    {
        qDebug()<<"no gpu";
        m_producer = new Mlt::Producer(profile(), url.toUtf8().constData());
        Q_ASSERT(m_producer);
        //    QString xml = MLT.XML(m_producer);
         //   LOG_DEBUG()<<xml;
//            for(int index=0; index < tmpplaylist.count(); index++)
//            {
//                    Mlt::ClipInfo* i = tmpplaylist.clip_info(index);
//                    LOG_DEBUG()<<"1";
//                    if (i) {

//                           // MAIN.setPauseAfterOpen(false);
//                            QString xml = MLT.XML(i->producer);
//                            //LOG_DEBUG()<<xml;
//                           if(xml.contains("INVALID"))
//                            {
//                                LOG_DEBUG()<<"This clip is invalid"<<i->resource;

//                                break;

//                            }
//                }
//            }

        qDebug()<<url.toUtf8().constData();
        if(m_producer->is_valid())
            qDebug()<<"create producer success";
        else
            qDebug()<<"create producer failed";
    }
    if (m_producer->is_valid()) {
        double fps = profile().fps();
        if (!profile().is_explicit()) {
            profile().from_producer(*m_producer);
            profile().set_width(alignWidth(profile().width()));
        }
        if (!qFuzzyCompare(profile().fps(), fps) || (Settings.playerGPU() && !profile().is_explicit())) {
            // Reload with correct FPS or with Movit normalizing filters attached.
            delete m_producer;
            m_producer = new Mlt::Producer(profile(), url.toUtf8().constData());
            Q_ASSERT(m_producer);
        }
        // Convert avformat to avformat-novalidate so that XML loads faster.
        if (!qstrcmp(m_producer->get("mlt_service"), "avformat")) {
            m_producer->set("mlt_service", "avformat-novalidate");
            m_producer->set("mute_on_pause", 0);
        }
        if (m_url.isEmpty() && QString(m_producer->get("xml")) == "was here") {
            if (m_producer->get_int("_original_type") != tractor_type ||
               (m_producer->get_int("_original_type") == tractor_type && m_producer->get("moviemator")))
                m_url = url;
        }
        setImageDurationFromDefault(m_producer);
    }
    else {
        delete m_producer;
        m_producer = nullptr;
        error = 1;
    }
    return error;
}

bool Controller::openXML(const QString &filename)
{
    LOG_DEBUG() << filename;
    bool error = true;
    close();
    Producer* producer = new Mlt::Producer(profile(), "xml", filename.toUtf8().constData());
    Q_ASSERT(producer);
    if (producer->is_valid()) {
        double fps = profile().fps();
        if (!profile().is_explicit()) {
            profile().from_producer(*producer);
            profile().set_width(alignWidth(profile().width()));
        }
        if (!qFuzzyCompare(profile().fps(), fps)) {
            // reopen with the correct fps
            delete producer;
            producer = new Mlt::Producer(profile(), "xml", filename.toUtf8().constData());
            Q_ASSERT(producer);
        }
        producer->set(kShotcutVirtualClip, 1);
        producer->set("resource", filename.toUtf8().constData());
        setProducer(new Producer(producer));
        error = false;
    }
    delete producer;
    return error;
}

void Controller::close()
{
    Q_ASSERT(m_profile);

    LOG_DEBUG() << "begin";
    if (m_profile->is_explicit()) {
        pause();
    } else if (m_consumer && !m_consumer->is_stopped()) {
        m_consumer->stop();
    }
    if (isSeekableClip()) {
        setSavedProducer(m_producer);
    }
    delete m_producer;
    m_producer = nullptr;

    LOG_DEBUG() << "end";
}

void Controller::closeConsumer()
{
    LOG_DEBUG() << "begin";
    if (m_consumer)
        m_consumer->stop();
    delete m_consumer;
    m_consumer = nullptr;
    delete m_jackFilter;
    m_jackFilter = nullptr;
    LOG_DEBUG() << "end";
}

void Controller::play(double speed)
{
    LOG_DEBUG() << "begin";
    if (m_producer)
        m_producer->set_speed(speed);
    if (m_consumer) {
        // Restore real_time behavior and work-ahead buffering
        if (!Settings.playerGPU())
        if (m_consumer->get_int("real_time") != realTime()) {
            m_consumer->set("real_time", realTime());
            m_consumer->set("buffer", 25);
            m_consumer->set("prefill", 1);
            // Changes to real_time require a consumer restart if running.
            if (!m_consumer->is_stopped())
                m_consumer->stop();
        }
        m_consumer->start();
        refreshConsumer(Settings.playerScrubAudio());
    }
    if (m_jackFilter)
        m_jackFilter->fire_event("jack-start");
    setVolume(m_volume);

    LOG_DEBUG() << "end";
}

void Controller::pause()
{
    LOG_DEBUG() << "begin";
    if (m_producer && !qFuzzyIsNull(m_producer->get_speed())) {
        if (!Settings.playerGPU())
        if (m_consumer && m_consumer->is_valid()) {
            // Disable real_time behavior and buffering for frame accurate seeking.
            m_consumer->set("real_time", -1);
            m_consumer->set("buffer", 0);
            m_consumer->set("prefill", 0);
        }
        m_producer->set_speed(0);
        m_producer->seek(m_consumer->position() + 1);
        if (m_consumer && m_consumer->is_valid()) {
            m_consumer->purge();
//            m_consumer->start();
        }
    }
    if (m_jackFilter)
        m_jackFilter->fire_event("jack-stop");
    setVolume(m_volume);
    LOG_DEBUG() << "end";
}

void Controller::stop()
{
    LOG_DEBUG() << "begin";
    if (m_consumer && !m_consumer->is_stopped())
        m_consumer->stop();
    if (m_producer)
        m_producer->seek(0);
    if (m_jackFilter)
        m_jackFilter->fire_event("jack-stop");
    LOG_DEBUG() << "end";
}

void Controller::on_jack_started(mlt_properties, void* object, mlt_position *position)
{
    if (object && position)
        (static_cast<Controller*>(object))->onJackStarted(*position);
}

void Controller::onJackStarted(int position)
{
    if (m_producer) {
        m_producer->set_speed(1);
        m_producer->seek(position);
        refreshConsumer();
    }
}

void Controller::on_jack_stopped(mlt_properties, void* object, mlt_position *position)
{
    if (object && position)
        (static_cast<Controller*>(object))->onJackStopped(*position);
}

void Controller::onJackStopped(int position)
{
    if (m_producer) {
        if (!qFuzzyIsNull(m_producer->get_speed())) {
            Q_ASSERT(m_consumer);
            Event *event = m_consumer->setup_wait_for("consumer-sdl-paused");
            int result = m_producer->set_speed(0);
            if (result == 0 && m_consumer->is_valid() && !m_consumer->is_stopped())
                m_consumer->wait_for(event);
            delete event;
        }
        m_producer->seek(position);
    }
    if (m_consumer && m_consumer->get_int("real_time") >= -1)
        m_consumer->purge();
    refreshConsumer();
}

bool Controller::enableJack(bool enable)
{
	if (!m_consumer)
		return true;
	if (enable && !m_jackFilter) {
		m_jackFilter = new Mlt::Filter(profile(), "jackrack");
        Q_ASSERT(m_jackFilter);
		if (m_jackFilter->is_valid()) {
			m_consumer->attach(*m_jackFilter);
			m_consumer->set("audio_off", 1);
			if (isSeekable()) {
                m_jackFilter->listen("jack-started", this, reinterpret_cast<mlt_listener>(on_jack_started));
                m_jackFilter->listen("jack-stopped", this, reinterpret_cast<mlt_listener>(on_jack_stopped));
			}
		}
		else {
			delete m_jackFilter;
            m_jackFilter = nullptr;
			return false;
		}
	}
	else if (!enable && m_jackFilter) {
		m_consumer->detach(*m_jackFilter);
		delete m_jackFilter;
        m_jackFilter = nullptr;
		m_consumer->set("audio_off", 0);
		m_consumer->stop();
		m_consumer->start();
	}
	return true;
}


void Controller::setVolume(double volume, bool muteOnPause)
{
    m_volume = volume;

    // Keep the consumer muted when paused
    if (muteOnPause && m_producer && qFuzzyIsNull(m_producer->get_speed())) {
        volume = 0.0;
    }

    if (m_consumer) {
        if (m_consumer->get("mlt_service") == QString("multi")) {
            m_consumer->set("0.volume", volume);
        } else {
            m_consumer->set("volume", volume);
        }
    }

}

double Controller::volume() const
{
    return m_volume;
}

void Controller::onWindowResize()
{
    refreshConsumer();
}

void Controller::seek(int position)
{
    LOG_DEBUG() << "begin";
    setVolume(m_volume, false);
    if (m_producer) {
        // Always pause before seeking (if not already paused).
        if (!Settings.playerGPU())
        if (m_consumer && m_consumer->is_valid() && !qFuzzyIsNull(m_producer->get_speed())) {
            // Disable real_time behavior and buffering for frame accurate seeking.
            m_consumer->set("real_time", -1);
            m_consumer->set("buffer", 0);
            m_consumer->set("prefill", 0);
        }
        m_producer->set_speed(0);
        m_producer->seek(position);
        if (m_consumer && m_consumer->is_valid()) {
            if (m_consumer->is_stopped()) {
                m_consumer->start();
            } else {
                m_consumer->purge();
                refreshConsumer(Settings.playerScrubAudio());
            }
        }
    }
    if (m_jackFilter)
        mlt_events_fire(m_jackFilter->get_properties(), "jack-seek", &position, NULL);
    LOG_DEBUG() << "end";
}

void Controller::refreshConsumer(bool scrubAudio)
{
    LOG_DEBUG() << "begin";
    if (m_consumer) {
        // need to refresh consumer when paused
        m_consumer->set("scrub_audio", scrubAudio);
        m_consumer->set("refresh", 1);
    }
    LOG_DEBUG() << "end";
}

void Controller::saveXML(const QString& filename, Service* service, bool withRelativePaths)
{
    Q_ASSERT(m_producer);
    LOG_DEBUG() << "begin";
    Consumer c(profile(), "xml", filename.toUtf8().constData());
    Service s(service? service->get_service() : m_producer->get_service());
    if (s.is_valid()) {
        int ignore = s.get_int("ignore_points");
        if (ignore)
            s.set("ignore_points", 0);
//        c.set("time_format", "clock");

        c.set("no_meta", 1);

        c.set("store", "moviemator");

        //wzq remove the bug in libmltxml consumer_xml.c
        //if ( resource != NULL && mlt_properties_get( properties, "root" ) == NULL )
        //{
            // Get the current working directory
            //char *cwd = getcwd( NULL, 0 );
            //mlt_properties_set( MLT_SERVICE_PROPERTIES( service ), "root", cwd );
            //free( cwd );
        //}
        // getcwd 在WINDOWS上getcwd得到的中文路径是ANSI编码，导致存储的XML和UTF-8冲突，导致LIBXML解析错误
        c.set( "root", "MovieMator" );

        if (withRelativePaths) {
            c.set("root", QFileInfo(filename).absolutePath().toUtf8().constData());
            //c.set("no_root", 1);
        }
        c.set("title", QString("MovieMator version ").append("2"/*MOVIEMATOR_VERSION*/).toUtf8().constData());



        c.connect(s);
        c.start();
        if (ignore)
            s.set("ignore_points", ignore);
    }
    LOG_DEBUG() << "end";
}

QString Controller::XML(Service* service)
{
    Q_ASSERT(m_producer);

    LOG_DEBUG() << "begin";
    static const char* propertyName = "string";
    Consumer c(profile(), "xml", propertyName);
    Service s(service? service->get_service() : m_producer->get_service());
    if (!s.is_valid())
        return "";
    int ignore = s.get_int("ignore_points");
    if (ignore)
        s.set("ignore_points", 0);
    c.set("no_meta", 1);
    c.set("store", "moviemator");

//    c.set("time_format", "clock");


    //wzq remove the bug in libmltxml consumer_xml.c
    //if ( resource != NULL && mlt_properties_get( properties, "root" ) == NULL )
    //{
        // Get the current working directory
        //char *cwd = getcwd( NULL, 0 );
        //mlt_properties_set( MLT_SERVICE_PROPERTIES( service ), "root", cwd );
        //free( cwd );
    //}
    // getcwd 在WINDOWS上getcwd得到的中文路径是ANSI编码，导致存储的XML和UTF-8冲突，导致LIBXML解析错误
    c.set( "root", "MovieMator" );

    c.connect(s);
    c.start();
    if (ignore)
        s.set("ignore_points", ignore);

    LOG_DEBUG() << "end";
    return QString::fromUtf8(c.get(propertyName));
}

QString Controller::getXMLWithoutProfile(QString &strXml)
{
    int nStart  = strXml.indexOf("<producer ", 0, Qt::CaseInsensitive);
    int nEnd    = strXml.lastIndexOf("</mlt>", -1, Qt::CaseInsensitive);

    if((nStart < 0) || (nEnd < 0) || (nStart >= nEnd))
    {
        return QString();
    }

    return strXml.mid(nStart, nEnd - nStart);
}

void Controller::saveXMLWithoutProfile(const QString& strFilename, Service* pService, bool bWithRelativePaths)
{
    saveXML(strFilename, pService, bWithRelativePaths);

    QFile file(strFilename);
    if(!file.exists())
    {
        return;
    }

    if(file.open(QIODevice::ReadWrite))
    {
        QString strXml      = file.readAll();
        QString strContent  = getXMLWithoutProfile(strXml);
        if(file.resize(0))      // 清空 file文件内容
        {
            file.write(strContent.toUtf8().constData());
        }
    }
    file.close();
}


int Controller::consumerChanged()
{
    LOG_DEBUG() << "begin";
    int error = 0;
    if (m_consumer) {
        bool jackEnabled = m_jackFilter != nullptr;
        m_consumer->stop();
        delete m_consumer;
        m_consumer = nullptr;
        delete m_jackFilter;
        m_jackFilter= nullptr;
        error = reconfigure(false);
        if (m_consumer) {
            enableJack(jackEnabled);
            setVolume(m_volume);
            m_consumer->start();
        }
    }
    LOG_DEBUG() << "end";
    return error;
}

void Controller::setProfile(const QString& profile_name)
{
    Q_ASSERT(m_profile);
    LOG_DEBUG() << "setting to profile" << (profile_name.isEmpty()? "Automatic" : profile_name);
    if (!profile_name.isEmpty()) {
        Mlt::Profile tmp(profile_name.toLatin1().constData());
        m_profile->set_colorspace(tmp.colorspace());
        m_profile->set_frame_rate(tmp.frame_rate_num(), tmp.frame_rate_den());
        m_profile->set_height(tmp.height());
        m_profile->set_progressive(tmp.progressive());
        m_profile->set_sample_aspect(tmp.sample_aspect_num(), tmp.sample_aspect_den());
        m_profile->set_display_aspect(tmp.display_aspect_num(), tmp.display_aspect_den());
        m_profile->set_width(alignWidth(tmp.width()));
        m_profile->set_explicit(true);
    } else {
        m_profile->set_explicit(false);
        if (m_producer) {
            m_profile->from_producer(*m_producer);
            m_profile->set_width(alignWidth(m_profile->width()));
        }
    }
}

QString Controller::resource() const
{
    QString resource;
    if (!m_producer)
        return resource;
    resource = QString(m_producer->get("resource"));
    return resource;
}

bool Controller::isSeekable() const
{
    bool seekable = false;
    if (m_producer && m_producer->is_valid()) {
        if (m_producer->get("force_seekable")) {
            seekable = m_producer->get_int("force_seekable");
        } else {
            seekable = m_producer->get_int("seekable");
            if (!seekable && m_producer->get("mlt_type")) {
                // XXX what was this for?
                seekable = !strcmp(m_producer->get("mlt_type"), "mlt_producer");
            }
            if (!seekable) {
                // These generators can take an out point to define their length.
                // TODO: Currently, these max out at 15000 frames, which is arbitrary.
                QString service(m_producer->get("mlt_service"));
                seekable = (service == "color") || service.startsWith("frei0r.") || (service =="tone") || (service =="xml");// (service =="xml")，临时解决：打开含有flv视频的工程文件不能seek的问题
            }
        }
    }
    return seekable;
}

bool Controller::isClip() const
{
    return !isPlaylist() && !isMultitrack();
}

bool Controller::isSeekableClip()
{
    return isClip() && isSeekable();
}

bool Controller::isPlaylist() const
{
    return m_producer && m_producer->is_valid() &&
          !m_producer->get_int(kShotcutVirtualClip) &&
            (m_producer->get_int("_original_type") == playlist_type || resource() == "<playlist>");
}

bool Controller::isMultitrack() const
{
    return m_producer && m_producer->is_valid()
        && !m_producer->get_int(kShotcutVirtualClip)
        && (m_producer->get_int("_original_type") == tractor_type || resource() == "<tractor>")
            && (m_producer->get("moviemator"));
}

bool Controller::isImageProducer(Service* service) const
{
    if (service && service->is_valid()) {
        QString serviceName = service->get("mlt_service");
        return (serviceName == "pixbuf" || serviceName == "qimage");
    }
    return false;
}

void Controller::rewind()
{
    if (!m_producer || !m_producer->is_valid())
        return;
    // Starting the consumer when producer at its end fails. So, first seek to
    // frame before last.
    if (m_producer->position() >= m_producer->get_length() - 1)
        m_producer->seek(m_producer->get_length() - 2);
    if (m_producer->get_speed() >= 0)
        play(-1.0);
    else
        m_producer->set_speed(m_producer->get_speed() * 2);
}

void Controller::fastForward()
{
    if (!m_producer || !m_producer->is_valid())
        return;
    if (m_producer->get_speed() <= 0)
        play();
    else
        m_producer->set_speed(m_producer->get_speed() * 2);
}

void Controller::previous(int currentPosition)
{
    if (!m_producer) return;
    if (isMultitrack()) return;
    if (currentPosition > m_producer->get_out())
        seek(MLT.producer()->get_out());
    else if (currentPosition <= m_producer->get_in())
        seek(0);
    else
        seek(m_producer->get_in());
}

void Controller::next(int currentPosition)
{
    if (!m_producer) return;
    if (isMultitrack()) return;
    if (currentPosition < m_producer->get_in())
        seek(m_producer->get_in());
    else if (currentPosition >= m_producer->get_out())
        seek(m_producer->get_length() - 1);
    else
        seek(m_producer->get_out());
}

void Controller::setIn(int in)
{
    if (m_producer && m_producer->is_valid()) {
        m_producer->set("in", in);

        // Adjust all filters that have an explicit duration.
        int n = m_producer->filter_count();
        for (int i = 0; i < n; i++) {
            Filter* filter = m_producer->filter(i);
            if (filter && filter->is_valid() && filter->get_length() > 0) {
                if (QString(filter->get(kShotcutFilterProperty)).startsWith("fadeIn")
                        || QString(filter->get("mlt_service")).contains("webvfx")) {
                    filter->set_in_and_out(in, in + filter->get_length() - 1);
                }
            }
            delete filter;
        }
    }
}

void Controller::setOut(int out)
{
    if (m_producer && m_producer->is_valid()) {
        m_producer->set("out", out);

        // Adjust all filters that have an explicit duration.
        int n = m_producer->filter_count();
        for (int i = 0; i < n; i++) {
            Filter* filter = m_producer->filter(i);
            if (filter && filter->is_valid() && filter->get_length() > 0) {
                if (QString(filter->get(kShotcutFilterProperty)).startsWith("fadeOut")
                        || QString(filter->get("mlt_service")).contains("webvfx")) {
                    int in = out - filter->get_length() + 1;
                    filter->set_in_and_out(in, out);
                }
            }
            delete filter;
        }
    }
}

void Controller::restart()
{
    if (!m_consumer) return;
    if (m_producer && m_producer->is_valid() && !qFuzzyIsNull(m_producer->get_speed())) {
        // Update the real_time property if not paused.
        m_consumer->set("real_time", realTime());
    }
    const char* position = m_consumer->frames_to_time(m_consumer->position());
    double speed = m_producer->get_speed();
    QString xml = XML();
    stop();
    if (!setProducer(new Mlt::Producer(profile(), "xml-string", xml.toUtf8().constData()))) {
#ifdef Q_OS_WIN
        play(speed);
        if (m_producer && m_producer->is_valid())
            m_producer->seek(position);
        // Windows needs an extra kick here when not paused!
        if (speed != 0.0)
            play(speed);
#else
        if (m_producer && m_producer->is_valid())
            m_producer->seek(position);
        play(speed);
#endif
    }
}

void Controller::resetURL()
{
    m_url = QString();
}

QImage Controller::image(Mlt::Frame* frame, int width, int height)
{
    LOG_DEBUG() << "begin";
    QImage result(width, height, QImage::Format_ARGB32);
    if (frame && frame->is_valid()) {
        if (width > 0 && height > 0) {
            frame->set("rescale.interp", "bilinear");
            frame->set("deinterlace_method", "onefield");
            frame->set("top_field_first", -1);
        }
        mlt_image_format format = mlt_image_rgb24a;
        const uchar *image = frame->get_image(format, width, height);
        if (image) {
            QImage temp(width, height, QImage::Format_ARGB32);
            memcpy(temp.scanLine(0), image, size_t(width * height * 4));
            result = temp.rgbSwapped();
        }
    } else {
        result.fill(QColor(Qt::red).rgb());
    }

    LOG_DEBUG() << "end";
    return result;
}

QImage Controller::image(Producer& producer, int frameNumber, int width, int height)
{
    LOG_DEBUG() << "begin";
    QImage result;
    if (frameNumber > producer.get_length() - 3) {
        producer.seek(frameNumber - 2);
        Mlt::Frame* frame = producer.get_frame();
        result = image(frame, width, height);
        delete frame;
        frame = producer.get_frame();
        result = image(frame, width, height);
        delete frame;
        frame = producer.get_frame();
        result = image(frame, width, height);
        delete frame;
    } else {
        producer.seek(frameNumber);
        Mlt::Frame* frame = producer.get_frame();
        result = image(frame, width, height);
        delete frame;
    }

    LOG_DEBUG() << "end";
    return result;
}

void Controller::updateAvformatCaching(int trackCount)
{
    int i = QThread::idealThreadCount() + trackCount;
    mlt_service_cache_set_size(nullptr, "producer_avformat", qMax(4, i));
}

bool Controller::isAudioFilter(const QString &name)
{
    Q_ASSERT(m_repo);
    QScopedPointer<Properties> metadata(m_repo->metadata(filter_type, name.toLatin1().constData()));
    Q_ASSERT(metadata);
    if (metadata->is_valid()) {
        Properties tags(metadata->get_data("tags"));
        if (tags.is_valid()) {
            for (int j = 0; j < tags.count(); ++j) {
                if (!qstricmp(tags.get(j), "Audio"))
                    return true;
            }
        }
    }
    return false;
}

int Controller::realTime() const
{
    int realtime = 1;
    if (!Settings.playerRealtime()) {
        if (Settings.playerGPU()) {
            return -1;
        } else {
            int threadCount = QThread::idealThreadCount();
            threadCount = threadCount > 2? qMin(threadCount - 1, 4) : 1;
            realtime = -threadCount;
        }
    }
    return realtime;
}

void Controller::setImageDurationFromDefault(Service* service) const
{
    Q_ASSERT(m_profile);
    if (service && service->is_valid()) {
        if (isImageProducer(service)) {
            service->set("ttl", 1);
            service->set("length", qRound(m_profile->fps() * 600));
            service->set("out", qRound(m_profile->fps() * Settings.imageDuration()) - 1);

        }
    }
}

QUuid Controller::uuid(Mlt::Properties &properties) const
{
    Q_ASSERT(properties.is_valid());
    return QUuid(properties.get(kUuidProperty));
}

void Controller::setUuid(Mlt::Properties &properties, QUuid uid) const
{
    Q_ASSERT(properties.is_valid());
    properties.set(kUuidProperty,
            (uid.toByteArray() + '\n').data());
}

QUuid Controller::ensureHasUuid(Mlt::Properties& properties) const
{
    Q_ASSERT(properties.is_valid());
    if (properties.get(kUuidProperty)) {
        return uuid(properties);
    } else {
        QUuid newUid = QUuid::createUuid();
        setUuid(properties, newUid);
        return newUid;
    }
}

void Controller::copyFilters(Producer& fromProducer, Producer& toProducer)
{
    Q_ASSERT(fromProducer.is_valid());
    Q_ASSERT(toProducer.is_valid());
    LOG_DEBUG() << "begin";
    int count = fromProducer.filter_count();
    for (int i = 0; i < count; i++) {
        QScopedPointer<Mlt::Filter> filter(fromProducer.filter(i));
        if (filter && filter->is_valid() && !filter->get_int("_loader")) {
            toProducer.attach(*filter);
        }
    }
    LOG_DEBUG() << "end";
}

void Controller::setSavedProducer(Mlt::Producer* producer)
{
    m_savedProducer.reset(new Mlt::Producer(producer));
}

QString Controller::getHash(Mlt::Properties& properties) const
{
    Q_ASSERT(properties.is_valid());
    QString hash = properties.get(kShotcutHashProperty);
    if (hash.isEmpty()) {
        QString service = properties.get("mlt_service");
        QString resource = QString::fromUtf8(properties.get("resource"));

        if (service == "timewarp")
            resource = QString::fromUtf8(properties.get("warp_resource"));
        else if (service == "vidstab")
            resource = QString::fromUtf8(properties.get("filename"));
        QString hash = Util::getFileHash(resource);
        if (!hash.isEmpty())
            properties.set(kShotcutHashProperty, hash.toLatin1().constData());
    }
    return hash;
}


const QString& Controller::MltXMLMimeType()
{
    return XmlMimeType;
}

void TransportControl::play(double speed)
{
    MLT.play(speed);
}

void TransportControl::pause()
{
    MLT.pause();
}

void TransportControl::stop()
{
    MLT.stop();
}

void TransportControl::seek(int position)
{
    MLT.seek(position);
}

void TransportControl::rewind()
{
    MLT.rewind();
}

void TransportControl::fastForward()
{
    MLT.fastForward();
}

void TransportControl::previous(int currentPosition)
{
    MLT.previous(currentPosition);
}

void TransportControl::next(int currentPosition)
{
    MLT.next(currentPosition);
}

void TransportControl::setIn(int in)
{
    MLT.setIn(in);
}

void TransportControl::setOut(int out)
{
    MLT.setOut(out);
}


} // namespace
