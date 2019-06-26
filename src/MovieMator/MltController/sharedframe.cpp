/*
 * Copyright (c) 2015 Meltytech, LLC
 * Author: Brian Matherly <code@brianmatherly.com>
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
#include "sharedframe.h"

#pragma pack(4)
/**
  * 消除字节对齐警告：warning: padding class 'FrameData' with 4 bytes to align 'f'
  * 由于系统目前的对齐模数是8个字节（#pragma pack(show) == 8），而FrameData中只有一个成员Mlt::Frame f，
  * Mlt::Frame的对齐方式是4个字节，因此会按照4个字节来处理FrameData的对齐，因此会报警告
  * 目前的改法是不使用系统的8字节对齐，手动设置对齐为4个字节（#pragma pack(4)）
  */
class FrameData : public QSharedData
{
public:
    FrameData() : f(static_cast<mlt_frame>(nullptr)) {}
    FrameData(Mlt::Frame& frame) : f(frame) {}
    ~FrameData() {}

    Mlt::Frame f;
private:
    Q_DISABLE_COPY(FrameData)
};
#pragma pack()

SharedFrame::SharedFrame()
  : d(new FrameData)
{
}

SharedFrame::SharedFrame(Mlt::Frame& frame)
  : d(new FrameData(frame))
{
}

SharedFrame::SharedFrame(const SharedFrame& other)
  : d(other.d)
{
}

SharedFrame::~SharedFrame()
{
}

SharedFrame& SharedFrame::operator=(const SharedFrame& other)
{
   d = other.d;
   return *this;
}

bool SharedFrame::is_valid() const
{
    Q_ASSERT(d);
    return d->f.is_valid();
}

Mlt::Frame SharedFrame::clone(bool audio, bool image, bool alpha) const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());
    // TODO: Consider moving this implementation into MLT.
    // It could be added to mlt_frame as an alternative to:
    //     mlt_frame mlt_frame_clone( mlt_frame self, int is_deep );
    // It could also be added to Mlt::Frame as a const function.
    void* data = nullptr;
    void* copy = nullptr;
    int size = 0;
    Mlt::Frame cloneFrame(mlt_frame_init( nullptr ));
    cloneFrame.inherit(d->f);
    cloneFrame.set("_producer", d->f.get_data("_producer", size), 0, nullptr, nullptr);
    cloneFrame.set("movit.convert", d->f.get_data("movit.convert", size), 0, nullptr, nullptr);
    cloneFrame.get_frame()->convert_image = d->f.get_frame()->convert_image;
    cloneFrame.get_frame()->convert_audio = d->f.get_frame()->convert_audio;

    data = d->f.get_data("audio", size);
    if (audio && data) {
        if (!size) {
            size = mlt_audio_format_size(get_audio_format(),
                                         get_audio_samples(),
                                         get_audio_channels());
        }
        copy = mlt_pool_alloc(size);
        memcpy(copy, data, size_t(size));
        cloneFrame.set("audio", copy, size, mlt_pool_release);
    } else {
        cloneFrame.set("audio", 0);
        cloneFrame.set("audio_format", mlt_audio_none);
        cloneFrame.set("audio_channels", 0);
        cloneFrame.set("audio_frequency", 0);
        cloneFrame.set("audio_samples", 0);
    }

    data = d->f.get_data("image", size );
    if (image && data)
    {
        if (!size) {
            size = mlt_image_format_size(get_image_format(),
                                         get_image_width(),
                                         get_image_height(),
                                         nullptr);
        }
        copy = mlt_pool_alloc(size);
        memcpy(copy, data, size_t(size));
        cloneFrame.set("image", copy, size, mlt_pool_release);
    } else {
        cloneFrame.set("image", 0);
        cloneFrame.set("image_format", mlt_image_none);
        cloneFrame.set("width", 0);
        cloneFrame.set("height", 0);
    }

    data = d->f.get_data("alpha", size );
    if (alpha && data)
    {
        if (!size) {
            size = get_image_width() * get_image_height();
        }
        copy = mlt_pool_alloc(size);
        memcpy(copy, data, size_t(size));
        cloneFrame.set("alpha", copy, size, mlt_pool_release);
    } else {
        cloneFrame.set("alpha", 0);
    }

    // Release the reference on the initial frame so that the returned frame
    // only has one reference.
    mlt_frame_close(cloneFrame.get_frame());
    return cloneFrame;
}

int SharedFrame::get_int(const char *name) const
{
    Q_ASSERT(name);

    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return d->f.get_int(name);
}

int64_t SharedFrame::get_int64(const char *name) const
{
    Q_ASSERT(name);

    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return d->f.get_int64(name);
}

double SharedFrame::get_double(const char *name) const
{
    Q_ASSERT(name);

    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return d->f.get_double(name);
}

int SharedFrame::get_position() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return d->f.get_position();
}

mlt_image_format SharedFrame::get_image_format() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return static_cast<mlt_image_format>(d->f.get_int( "format" ));
}

int SharedFrame::get_image_width() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return d->f.get_int( "width" );
}

int SharedFrame::get_image_height() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return d->f.get_int( "height" );
}

const uint8_t* SharedFrame::get_image() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    mlt_image_format format = get_image_format();
    int width = get_image_width();
    int height = get_image_height();
    return static_cast<uint8_t*>(d->f.get_image(format, width, height, 0));
}

mlt_audio_format SharedFrame::get_audio_format() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return static_cast<mlt_audio_format>(d->f.get_int( "audio_format" ));
}

int SharedFrame::get_audio_channels() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return d->f.get_int( "audio_channels" );
}

int SharedFrame::get_audio_frequency() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return d->f.get_int( "audio_frequency" );
}

int SharedFrame::get_audio_samples() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    return d->f.get_int( "audio_samples" );
}

const int16_t* SharedFrame::get_audio() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->f.is_valid());

    mlt_audio_format format = get_audio_format();
    int frequency = get_audio_frequency();
    int channels = get_audio_channels();
    int samples = get_audio_samples();
    return static_cast<int16_t*>(d->f.get_audio(format, frequency, channels, samples));
}
