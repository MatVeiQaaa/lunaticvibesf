#include "video.h"
#include "common/u8.h"

#include <chrono>
#include <functional>
#include <thread>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
}

#include <common/log.h>
#include <common/types.h>

void lunaticvibes::AVFrameDeleter::operator()(AVFrame* avp)
{
    av_frame_free(&avp);
}
void lunaticvibes::AVPacketDeleter::operator()(AVPacket* avp)
{
    av_packet_free(&avp);
}

void video_init() {}

sVideo::~sVideo()
{
    if (haveVideo)
    {
        if (playing)
        {
            stopPlaying();
            decodeEnd.wait();
        }
        unsetVideo();
    }
}

int sVideo::setVideo(const Path& file, double speed, bool loop)
{
    if (!fs::exists(file))
        return -1;
    if (!fs::is_regular_file(file))
        return -2;

    if (haveVideo)
        unsetVideo();
    this->file = file;
    this->speed = speed;
    loop_playback = loop;

    if (int ret = avformat_open_input(&pFormatCtx, lunaticvibes::cs(file.u8string()), NULL, NULL); ret != 0)
    {
        char buf[256];
        av_strerror(ret, buf, 256);
        LOG_WARNING << "[Video] Could not open input stream: " << file << " (" << buf << ")";
        return -1;
    }

    if (int ret = avformat_find_stream_info(pFormatCtx, NULL); ret < 0)
    {
        char buf[256];
        av_strerror(ret, buf, 256);
        LOG_WARNING << "[Video] Could not find stream info of " << file << " (" << buf << ")";
        return -2;
    }

    // if ((videoIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0)) < 0)
    for (size_t i = 0; i < pFormatCtx->nb_streams; ++i)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoIndex = i;
            break;
        }
    }
    if (videoIndex < 0)
    {
        LOG_WARNING << "[Video] Could not find video stream of " << file;
        return -3;
    }

    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoIndex]->codecpar->codec_id);
    if (pCodec == nullptr)
    {
        auto id = pFormatCtx->streams[videoIndex]->codecpar->codec_id;
        auto desc = avcodec_descriptor_get(id);
        if (desc)
            LOG_WARNING << "[Video] Could not find codec " << desc->long_name;
        else
            LOG_WARNING << "[Video] Could not find codec " << (int)id;

        return -3;
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == nullptr)
    {
        LOG_WARNING << "[Video] Could not alloc codec context of " << file;
        return -4;
    }

    if (int ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoIndex]->codecpar); ret < 0)
    {
        char buf[256];
        av_strerror(ret, buf, 256);
        LOG_WARNING << "[Video] Could not convert codec parameters of " << file << " (" << buf << ")";
        return -5;
    }

    haveVideo = true;
    w = pCodecCtx->width;
    h = pCodecCtx->height;
    return 0;
}

int sVideo::unsetVideo()
{
    haveVideo = false;
    finished = false;
    firstFrame = true;
    pPacket.reset();
    pFrame.reset();
    if (pCodecCtx)
        avcodec_free_context(&pCodecCtx);
    if (pFormatCtx)
        avformat_close_input(&pFormatCtx);
    return 0;
}

Texture::PixelFormat sVideo::getFormat()
{
    using fmt = Texture::PixelFormat;
    if (!haveVideo)
        return fmt::UNKNOWN;
    switch (pCodecCtx->pix_fmt)
    {
    case AV_PIX_FMT_RGB24: return fmt::RGB24;
    case AV_PIX_FMT_BGR24: return fmt::BGR24;
    case AV_PIX_FMT_YUV420P: return fmt::IYUV;
    case AV_PIX_FMT_YUYV422: return fmt::YUY2;
    case AV_PIX_FMT_UYVY422: return fmt::UYVY;
    case AV_PIX_FMT_YVYU422: return fmt::YVYU;

    default: return fmt::UNSUPPORTED;
    }
}

void sVideo::startPlaying()
{
    if (playing)
        return;
    if (finished)
        return;
    playing = true;
    startTime = std::chrono::system_clock::now();
    decodeEnd = std::async(std::launch::async, std::bind_front(&sVideo::decodeLoop, this));
}

void sVideo::stopPlaying()
{
    if (!playing)
        return;
    playing = false;
    decodeEnd.wait();
    seek(0);
}

void sVideo::decodeLoop()
{
    if (!haveVideo)
        return;

    valid = false;

    pFrame.reset(av_frame_alloc());
    if (pFrame == nullptr)
    {
        LOG_WARNING << "[Video] Could not alloc frame object of " << file;
        return;
    }

    pPacket.reset(av_packet_alloc());
    if (pPacket == nullptr)
    {
        LOG_WARNING << "[Video] Could not alloc packet object of " << file;
        return;
    }

    if (int ret = avcodec_open2(pCodecCtx, pCodec, NULL); ret < 0)
    {
        char buf[256];
        av_strerror(ret, buf, 256);
        LOG_ERROR << "[Video] Could not open codec of " << file << " (" << buf << ")";
        unsetVideo();
        return;
    }

    decoding = true;

    // timestamps per second
    double tsps = pFormatCtx->streams[videoIndex]->time_base.num == 0
                      ? AV_TIME_BASE
                      : av_q2d(av_inv_q(pFormatCtx->streams[videoIndex]->time_base));
    lunaticvibes::AVFramePtr pFrame1{av_frame_alloc()};

    do
    {
        while (av_read_frame(pFormatCtx, pPacket.get()) == 0 && pPacket != nullptr)
        {
            if (!playing)
                return;

            // Ignore packets from audio streams
            if (pPacket->stream_index != videoIndex)
                continue;

            avcodec_send_packet(pCodecCtx, pPacket.get());

            int ret = avcodec_receive_frame(pCodecCtx, pFrame1.get());
            av_packet_unref(pPacket.get());

            if (ret < 0)
            {
                if (ret == AVERROR(EAGAIN))
                {
                    continue;
                }
                else
                {
                    char buf[128];
                    av_strerror(ret, buf, 128);
                    LOG_ERROR << "[Video] playback error: " << buf;
                    decoding = false;
                    return;
                }
            }

            if (pFrame1->best_effort_timestamp >= 0)
            {
                if (!(pFrame1->flags & AV_FRAME_FLAG_CORRUPT))
                {
                    std::unique_lock l(video_frame_mutex);

                    av_frame_unref(pFrame.get());
                    av_frame_ref(pFrame.get(), pFrame1.get());
                    valid = true;
                }

                using namespace std::chrono;
                auto frameTime_ms =
                    static_cast<long long>(std::round(pFrame1->best_effort_timestamp / tsps * 1000 / speed));
                if (firstFrame)
                {
                    firstFrame = false;
                    startTime -= milliseconds(frameTime_ms);
                }
                else
                {
                    if (duration_cast<milliseconds>(system_clock::now() - startTime).count() < frameTime_ms)
                    {
                        std::this_thread::sleep_until(startTime + milliseconds(frameTime_ms));
                    }
                }
            }
            decoded_frames = pCodecCtx->frame_num;
        }

        if (loop_playback)
        {
            using namespace std::chrono_literals;
            // std::this_thread::sleep_for(33ms);
            decoded_frames = 0;
            seek(0, true);
            startTime = std::chrono::system_clock::now();
        }
        else
        {
            // drain
            avcodec_send_packet(pCodecCtx, NULL);
            {
                // av_frame_unref(frame) is omitted
                if (int ret = avcodec_receive_frame(pCodecCtx, pFrame1.get()))
                {
                    char buf[128];
                    av_strerror(ret, buf, 128);
                    LOG_ERROR << "[Video] playback drain error: " << buf;
                }
                if (pFrame1->best_effort_timestamp >= 0)
                {
                    std::unique_lock l(video_frame_mutex);

                    av_frame_unref(pFrame.get());
                    av_frame_ref(pFrame.get(), pFrame1.get());
                    valid = true;
                }
                decoded_frames = pCodecCtx->frame_num;
            }

            playing = false;
            finished = true;
        }

    } while (playing);

    decoding = false;
}

void sVideo::seek(int64_t second, bool backwards)
{
    if (!haveVideo)
        return;
    if (second == 0)
        firstFrame = true;

    // timestamps per second
    double tsps = pFormatCtx->streams[videoIndex]->time_base.num == 0
                      ? AV_TIME_BASE
                      : av_q2d(av_inv_q(pFormatCtx->streams[videoIndex]->time_base));

    if (int ret = av_seek_frame(pFormatCtx, videoIndex, int64_t(std::round(second / tsps)),
                                backwards ? AVSEEK_FLAG_BACKWARD : 0);
        ret >= 0)
    {
        finished = false;
        avcodec_flush_buffers(pCodecCtx);
    }
    else
    {
        LOG_ERROR << "[Video] seek " << second << "s error (" << file << ")";
    }
}
