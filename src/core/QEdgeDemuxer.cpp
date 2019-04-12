#include <core/QEdgeDemuxer.h>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <QDebug>

QEdgeDemuxer::QEdgeDemuxer() : m_running( false ) {}

void QEdgeDemuxer::Init( IDemuxerSubscriber* subscriber )
{
    m_subscriber = subscriber;
}

bool QEdgeDemuxer::Start( QString url )
{
    if( url.isEmpty() || m_running )
    {
        Q_ASSERT( 0 );
        return false;
    }

    m_url = url;
    m_running = true;

    m_thread.reset( new std::thread( QEdgeDemuxer::DemuxInThread, this ) );

    return true;
}

void QEdgeDemuxer::Interrupt()
{
    m_running = false;
    m_thread.release();
}

void QEdgeDemuxer::OnFailed( QString err_text )
{
    m_subscriber->OnDemuxerFailed( err_text );
}

void QEdgeDemuxer::OnFinished()
{
    m_subscriber->OnDemuxerFinished();
}

void QEdgeDemuxer::DemuxInThread( void *ctx )
{
    QEdgeDemuxer* host = static_cast<QEdgeDemuxer*>( ctx );

    std::string target_url = host->m_url.toStdString();
    const char* url = target_url.c_str();

    AVFormatContext* format_context = avformat_alloc_context();
    int error_code = avformat_open_input( &format_context, url, nullptr, nullptr );

    if( error_code < 0 )
    {
        QString err_text = "ffmpeg: unable to open input file: " + QString( url ) + ". Error code: " + QString::number( error_code );
        host->OnFailed( err_text );
        return;
    }

    error_code = avformat_find_stream_info( format_context, nullptr );

    if( error_code < 0 )
    {
        QString err_text = "ffmpeg: unable to find stream info, input file: " + QString( url ) + ". Error code: " + QString::number( error_code );
        host->OnFailed( err_text );
        return;
    }

    int video_stream_index = -1;
    int audio_stream_index = -1;
    AVStream* video_stream = nullptr;
    AVStream* audio_stream = nullptr;

    for( uint i = 0; i < format_context->nb_streams; ++i )
    {
        if( format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO )
        {
            video_stream_index = int(i);
            video_stream = format_context->streams[i];
        }

        else if( format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO )
        {
            audio_stream_index = int(i);
            audio_stream = format_context->streams[i];
        }
    }

    if( !video_stream )
    {
        QString err_text = "ffmpeg: unable to find video stream";
        host->OnFailed( err_text );
        return;
    }

    if( !audio_stream )
    {
        QString err_text = "ffmpeg: unable to find audio stream";
        host->OnFailed( err_text );
        return;
    }

    if( error_code < 0 )
    {
        QString err_text = "ffmpeg: unable to open codec";
        host->OnFailed( err_text );
        return;
    }

    host->m_subscriber->InitStream( video_stream, audio_stream );

    int res = 0;
    while( res >= 0 )
    {
        AVPacket* packet = av_packet_alloc();
        res =  av_read_frame( format_context, packet );

        if( packet->stream_index == video_stream_index )
        {
            host->m_subscriber->OnVideoPacket( packet );
        }

        else if( packet->stream_index == audio_stream_index )
        {
            host->m_subscriber->OnAudioPacket( packet );
        }

        else
        {
            av_packet_unref( packet );
        }
    }

    avformat_close_input( &format_context );
    host->OnFinished();
}
