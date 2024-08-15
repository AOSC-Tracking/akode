/*  aKode: FFMPEG Decoder

    Copyright (C) 2005 Allan Sandfeld Jensen <kde@carewolf.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#define __STDC_CONSTANT_MACROS

#include "akodelib.h"
// #ifdef HAVE_FFMPEG

#include "file.h"
#include "audioframe.h"
#include "decoder.h"

#include <assert.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
}

#include "ffmpeg_decoder.h"
#include <iostream>

#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(57, 33, 100)
#define CODECPAR codec
#else
#define CODECPAR codecpar
#endif

// FFMPEG callbacks
extern "C" {
    static int akode_read(void* opaque, uint8_t *buf, int size)
    {
        aKode::File *file = (aKode::File*)opaque;
        return file->read((char*)buf, size);
    }
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(61, 0, 100)
    static int akode_write(void* opaque, uint8_t *buf, int size)
#else
    static int akode_write(void* opaque, const uint8_t *buf, int size)
#endif
    {
        aKode::File *file = (aKode::File*)opaque;
        return file->write((char*)buf, size);
    }
    static off_t akode_seek(void* opaque, off_t pos, int whence)
    {
        aKode::File *file = (aKode::File*)opaque;
        return file->lseek(pos, whence);
    }
}

namespace aKode {


bool FFMPEGDecoderPlugin::canDecode(File* /*src*/) {
    // ### FIXME
    return true;
}

extern "C" { FFMPEGDecoderPlugin ffmpeg_decoder; }

#define FILE_BUFFER_SIZE 8192

struct FFMPEGDecoder::private_data
{
    private_data() : audioStream(-1), videoStream(-1), packetSize(0), position(0),
                     eof(false), error(false), initialized(false), retries(0),
                     packet(NULL), audioStream_ctx(NULL), videoStream_ctx(NULL) {};

    AVFormatContext* ic;
    AVCodec* codec;
    AVInputFormat *fmt;
    AVIOContext* stream;

    int audioStream;
    int videoStream;
    AVCodecContext *audioStream_ctx;
    AVCodecContext *videoStream_ctx;

    AVPacket *packet;
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 12, 100)
    AVPacket _packet;
#endif
    uint8_t* packetData;
    int packetSize;

    File *src;
    AudioConfiguration config;

    long position;

    bool eof, error;
    bool initialized;
    int retries;

    unsigned char *file_buffer;
    unsigned char **buffer;
    int buffer_size;
};

FFMPEGDecoder::FFMPEGDecoder(File *src) {
    d = new private_data;

    d->src = src;
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58,9,100)
    av_register_all();
#endif
}

FFMPEGDecoder::~FFMPEGDecoder() {
    closeFile();
    delete d;
}


static bool setAudioConfiguration(AudioConfiguration *config, AVCodecContext *codec_context)
{
    config->sample_rate = codec_context->sample_rate;
# if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(59, 24, 100)
    config->channels = codec_context->channels;
# else
    config->channels = codec_context->ch_layout.nb_channels;
# endif
    // I do not know FFMPEGs surround channel ordering
    if (config->channels > 2) return false;
    config->channel_config = MonoStereo;
    // avcodec.h says sample_fmt is not used. I guess it means it is always S16
    switch(codec_context->sample_fmt) {
        case AV_SAMPLE_FMT_U8:
        case AV_SAMPLE_FMT_U8P:
            config->sample_width = 8; // beware unsigned!
            break;
        case AV_SAMPLE_FMT_S16:
        case AV_SAMPLE_FMT_S16P:
            config->sample_width = 16;
            break;
/*        disabled because I don't know byte ordering
        case SAMPLE_FMT_S24:
            config->sample_width = 24;
            break;
            */
        case AV_SAMPLE_FMT_S32:
        case AV_SAMPLE_FMT_S32P:
            config->sample_width = 32;
            break;
        case AV_SAMPLE_FMT_FLT:
        case AV_SAMPLE_FMT_FLTP:
            config->sample_width = -32;
            break;
        default:
            return false;;
     }
     return true;
}


bool FFMPEGDecoder::openFile() {
    d->src->openRO();
    d->src->fadvise();

    // The following duplicates what av_open_input_file would normally do

    // url_fdopen
    d->file_buffer = (unsigned char*)av_malloc(FILE_BUFFER_SIZE);
    d->stream = avio_alloc_context(d->file_buffer, FILE_BUFFER_SIZE, 0, d->src, akode_read, akode_write, akode_seek);
    if (!d->stream) {
        return false;
    }
    d->stream->seekable = d->src->seekable();
    d->stream->max_packet_size = FILE_BUFFER_SIZE;
    d->ic = avformat_alloc_context();
    if (!d->ic) {
        return false;
    }
    d->ic->pb = d->stream;

    if (avformat_open_input(&d->ic, d->src->filename, NULL, NULL) != 0)
    {
        closeFile();
        return false;
    }

    avformat_find_stream_info( d->ic, NULL );

    // Find the first a/v streams
    d->audioStream = -1;
    d->videoStream = -1;
    for (int i = 0; i < d->ic->nb_streams; i++) {
        if (d->ic->streams[i]->CODECPAR->codec_type == AVMEDIA_TYPE_AUDIO)
            d->audioStream = i;
        else
        if (d->ic->streams[i]->CODECPAR->codec_type == AVMEDIA_TYPE_VIDEO)
            d->videoStream = i;
    }
    if (d->audioStream == -1)
    {
        std::cerr << "akode: FFMPEG: Audio stream not found\n";
        // for now require an audio stream
        closeFile();
        return false;
    }

    av_dump_format(d->ic, d->audioStream, d->src->filename, 0);

    d->codec = (AVCodec *)avcodec_find_decoder(d->ic->streams[d->audioStream]->CODECPAR->codec_id);
    if (!d->codec) {
        std::cerr << "akode: FFMPEG: Codec not found\n";
        closeFile();
        return false;
    }

#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 33, 100)
    // allocate a codec context
    d->audioStream_ctx = avcodec_alloc_context3(d->codec);
    if (d->audioStream_ctx) {
        avcodec_parameters_to_context(d->audioStream_ctx, d->ic->streams[d->audioStream]->codecpar);
    }
    else {
        std::cerr << "akode: failed to allocate an audio codec context\n";
        closeFile();
        return false;
    }
#else
    d->audioStream_ctx = d->ic->streams[d->audioStream]->codec;
#endif

    // Set config
    if (!setAudioConfiguration(&d->config, d->audioStream_ctx))
    {
        closeFile();
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 33, 100)
        avcodec_free_context(&d->audioStream_ctx);
#else
        d->audioStream_ctx = NULL;
#endif
        return false;
    }

    avcodec_open2( d->audioStream_ctx, d->codec, NULL );

    double ffpos = (double)d->ic->streams[d->audioStream]->start_time / (double)AV_TIME_BASE;
    d->position = (long)(ffpos * d->config.sample_rate);

    return true;
}

void FFMPEGDecoder::closeFile() {
    if ( d->stream ) {
        if (d->stream->buffer)
            av_free(d->stream->buffer);
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(57, 80, 100)
        av_free(d->stream);
#else
        avio_context_free(&d->stream);
#endif
    }
    if( d->packetSize > 0 ) {
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 7, 0)
        av_free_packet(&d->_packet);
#else
        av_packet_unref( d->packet );
#endif
        d->packet = NULL;
        d->packetSize = 0;
    }

    if( d->codec ) {
        avcodec_close( d->audioStream_ctx );
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 33, 100)
        avcodec_free_context(&d->audioStream_ctx);
#else
        d->audioStream_ctx = NULL;
#endif
        d->codec = 0;
    }
    if( d->ic ) {
        // make sure av_close_input_file doesn't actually close the file
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(59, 0, 100)
        d->ic->iformat->flags = d->ic->iformat->flags | AVFMT_NOFILE;
#endif
        avformat_close_input( &d->ic );
        d->ic = 0;
    }
    if (d->src) {
        d->src->close();
    }
}

bool FFMPEGDecoder::readPacket() {
    do {
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57, 12, 100)
        d->packet = av_packet_alloc();
#else
        av_init_packet(&d->_packet);
        d->packet = &d->_packet;
#endif
        if ( av_read_frame(d->ic, d->packet) < 0 ) {
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 7, 0)
            av_free_packet(&d->_packet);
#else
            av_packet_unref( d->packet );
#endif
            d->packet = NULL;
            d->packetSize = 0;
            d->packetData = 0;
            return false;
        }
        if (d->packet->stream_index == d->audioStream) {
            d->packetSize = d->packet->size;
            d->packetData = d->packet->data;
            return true;
        }
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 7, 0)
        av_free_packet(&d->_packet);
#else
        av_packet_unref(d->packet);
#endif
        d->packet = NULL;
    } while (true);

    return false;
}

template<typename T>
static long demux(FFMPEGDecoder::private_data* d, AudioFrame* frame) {
    int channels = d->config.channels;
    long length = d->buffer_size/(channels*sizeof(T));
    frame->reserveSpace(&d->config, length);

    T offset = 0;
    if (frame->sample_width == 8) offset = -128; // convert unsigned to signed

    // Demux into frame
    T** buffer = (T**)d->buffer;
    T** data = (T**)frame->data;
    for(int i=0; i<length; i++)
        for(int j=0; j<channels; j++)
            data[j][i] = buffer[j][i] + offset;
    return length;
}

bool FFMPEGDecoder::readFrame(AudioFrame* frame)
{
    if (!d->initialized) {
        if (!openFile()) {
            d->error = true;
            return false;
        }
        d->initialized = true;
    }

    if( d->packetSize <= 0 )
        if (!readPacket()) {
            std::cerr << "akode: FFMPEG: EOF guessed\n";
            d->eof = true;
            return false;
        }

    assert(d->packet->stream_index == d->audioStream);

retry:
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 45, 101)
    AVFrame *decodeFrame = avcodec_alloc_frame();
#else
    AVFrame *decodeFrame = av_frame_alloc();
#endif
    if (!decodeFrame) {
        return false;
    }
    int decoded;

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57, 106, 100)
    int len = avcodec_receive_frame(d->audioStream_ctx, decodeFrame);
    if (len == 0) {
        decoded = 1;
    }
    else if (len == AVERROR(EAGAIN)) {
        len = 0;
    }
                    
    if (len == 0) {
        len = avcodec_send_packet(d->audioStream_ctx, d->packet);
        if (len == AVERROR(EAGAIN)) {
            len = 0;
        }
        else
        {
            len = d->packet->size;
            d->packetSize = d->packet->size;
        }
    }
#else
    int len = avcodec_decode_audio4( d->audioStream_ctx,
                                    decodeFrame, &decoded,
                                    d->packet );
#if !defined(FFMPEG_AVFRAME_HAVE_PKT_SIZE)
    d->packetSize = d->packet->size;
#else
    d->packetSize = decodeFrame->pkt_size;
#endif
#endif

    d->buffer = decodeFrame->data;
#if !defined(FFMPEG_AVFRAME_HAVE_CHANNELS)
# if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(59, 24, 100)
    d->buffer_size = decodeFrame->nb_samples * d->ic->streams[d->audioStream]->CODECPAR->channels * av_get_bytes_per_sample(d->ic->streams[d->audioStream]->codec->sample_fmt);
# else
    d->buffer_size = decodeFrame->nb_samples * d->ic->streams[d->audioStream]->CODECPAR->ch_layout.nb_channels * av_get_bytes_per_sample((AVSampleFormat)d->ic->streams[d->audioStream]->codecpar->format);
# endif
#else
    d->buffer_size = decodeFrame->nb_samples * decodeFrame->channels * av_get_bytes_per_sample(d->audioStream_ctx->sample_fmt);
#endif

    if (len <= 0) {
        d->retries++;
        if (d->retries > 8) {
            std::cerr << "akode: FFMPEG: Decoding failure\n";
            d->error = true;
            return false;
        }
        goto retry;
    } else
        d->retries = 0;

    d->packetSize -= len;
    d->packetData += len;
    long length = 0;
    switch (d->config.sample_width) {
        case 8:
            length = demux<int8_t>(d,frame);
            break;
        case 16:
            length = demux<int16_t>(d,frame);
            break;
        case 32:
            length = demux<int32_t>(d,frame);
            break;
        case -32:
            length = demux<float>(d,frame);
            break;
        default:
            assert(false);
    }
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54, 28, 0)
    av_free(decodeFrame);
#elif LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 45, 101)
    avcodec_free_frame(&decodeFrame);
#else
    av_frame_free(&decodeFrame);
#endif
    if (length == 0) return readFrame(frame);
    // std::cout << "akode: FFMPEG: Frame length: " << length << "\n";

    if( d->packetSize <= 0 )
    {
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 7, 0)
        av_free_packet(&d->_packet);
#else
        av_packet_unref( d->packet );
#endif
    d->packet = NULL;
    }

    frame->pos = (d->position*1000)/d->config.sample_rate;
    d->position += length;
    return true;
}

long FFMPEGDecoder::length() {
    if (!d->initialized) return -1;
    // ### Returns only the length of the first stream
    double ffmpeglen = (double)d->ic->streams[d->audioStream]->duration / (double)AV_TIME_BASE;
    return (long)(ffmpeglen*1000.0);
}

long FFMPEGDecoder::position() {
    if (!d->initialized) return -1;
    // ### Replace with FFMPEG native call
    return (d->position*1000)/d->config.sample_rate;
}

bool FFMPEGDecoder::eof() {
    return d->eof;
}

bool FFMPEGDecoder::error() {
    return d->error;
}

bool FFMPEGDecoder::seekable() {
    return d->src->seekable();
}

bool FFMPEGDecoder::seek(long pos) {
    if (!d->initialized) return false;
    AVRational time_base = d->ic->streams[d->audioStream]->time_base;
    std::cout<< "time base is " << time_base.num << "/" << time_base.den << "\n";
    long ffpos = 0;
    {
        int div = (pos / (time_base.num*1000)) * time_base.den;
        int rem = (pos % (time_base.num*1000)) * time_base.den;
        ffpos = div + rem / (time_base.num*1000);
    }
    std::cout<< "seeking to " << pos << "ms\n";
//     long ffpos = (long)((pos/1000.0)*AV_TIME_BASE);
    std::cout<< "seeking to " << ffpos << "pos\n";
    bool res = av_seek_frame(d->ic, d->audioStream, ffpos, 0);
    if (res < 0) return false;
    else {
        d->position = (pos * d->config.sample_rate)/1000;
        return true;
    }
}

const AudioConfiguration* FFMPEGDecoder::audioConfiguration() {
    if (!d->initialized) return 0;
    return &d->config;
}

} // namespace

// #endif // HAVE_FFMPEG
