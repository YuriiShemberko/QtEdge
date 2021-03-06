#include <core/QEdgeDemuxer.h>
#include <core/QEdgeUtils.h>
#include <core/Global.h>

#include <QDebug>

QEdgeDemuxer::QEdgeDemuxer() : m_running( false ) {}

void QEdgeDemuxer::Init( IDemuxerSubscriber* subscriber )
{
    m_subscriber = subscriber;
}

bool QEdgeDemuxer::Start( QString url, int seek_ms )
{
    if( url.isEmpty() || m_running )
    {
        Q_ASSERT( 0 );
        return false;
    }

    m_url = url;
    m_running = true;

    m_seek_ms = seek_ms;

    m_thread.reset( new std::thread( QEdgeDemuxer::DemuxInThread, this ) );

    return true;
}

void QEdgeDemuxer::Interrupt()
{
    m_running = false;
    if( m_thread.get() )
    {
        m_thread->join();
    }
    m_thread.release();
}

void QEdgeDemuxer::Seek( int msec )
{
    Interrupt();
    Start( m_url, msec );
}

void QEdgeDemuxer::OnFailed( QString err_text )
{
    m_subscriber->OnDemuxerFailed( err_text );
}

void QEdgeDemuxer::OnFinished()
{
    m_running = false;
    m_thread.release();
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

    if( host->m_seek_ms )
    {
        int res = 0;

        int64_t stream_idx = av_find_default_stream_index( format_context );
        int64_t frame_idx = utils::MsecsToTimebaseUnits( format_context->streams[stream_idx]->time_base, host->m_seek_ms );

        res = avformat_seek_file( format_context, stream_idx, 0, frame_idx, frame_idx, AVSEEK_FLAG_FRAME );

        if( res < 0 )
        {
            QString err_text = "ffmpeg: unable to seek video";
            host->OnFailed( err_text );
            return;
        }
    }

    int res = 0;
    while( res >= 0 && host->m_running )
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
            av_packet_free( &packet );
        }
    }

    avformat_flush( format_context );
    avformat_close_input( &format_context );
    avformat_free_context( format_context );
    host->OnFinished();
}
