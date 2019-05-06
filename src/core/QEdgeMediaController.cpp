#include <core/QEdgeMediaController.h>
#include <QDebug>
#include <time.h>
#include <unistd.h>
#include <QTimer>

QEdgeMediaController::QEdgeMediaController() :
    m_subscriber( CNullMediaControllerSubscriber::Instance() ),
    m_audio_finished( false ),
    m_video_finished( false )
{}

void QEdgeMediaController::ConnectToController( IMediaController::IMediaControllerSubscriber *subscriber )
{
    m_subscriber = subscriber;
    m_synchronizer.BindMediaReceiver( subscriber );
}

void QEdgeMediaController::Start( QString file_name )
{
    m_audio_finished = false;
    m_video_finished = false;
    m_presentation_started = false;

    m_demuxer.Init( this );

    if( !m_demuxer.Start( file_name ) )
    {
        OnDemuxerFailed( "cannot start demuxer" );
        return;
    }
}

void QEdgeMediaController::VideoFrameProcessed( AVFrame* frame )
{
    av_frame_unref( frame );
    av_frame_free( &frame );
}

void QEdgeMediaController::AudioFrameProcessed( AVFrame* frame )
{
    av_frame_unref( frame );
    av_frame_free( &frame );
}

void QEdgeMediaController::VideoPresented()
{
    m_synchronizer.OnVideoPresented();
}

void QEdgeMediaController::AudioPresented( long long audio_data_remains )
{
    m_synchronizer.OnAudioPresented( audio_data_remains );
}

void QEdgeMediaController::Stop()
{
    m_demuxer.Interrupt();
    m_audio_decoder.StopDecode( true );
    m_video_decoder.StopDecode( true );
}

void QEdgeMediaController::Seek( int64_t msec )
{
    m_audio_finished = false;
    m_video_finished = false;
    m_demuxer.Seek( msec );
}

void QEdgeMediaController::OnDecoderFinished( IDecoder *sender )
{
    if( sender == &m_audio_decoder )
    {
        m_audio_finished = true;
    }

    if( sender == &m_video_decoder )
    {
        m_video_finished = true;
    }

    if( m_video_finished && m_audio_finished )
    {
        m_subscriber->OnPlayFinished();
    }
}

void QEdgeMediaController::OnDecoderFailed( IDecoder *sender, QString err_text )
{
    if( sender == &m_video_decoder )
    {
        qDebug() << "[QEdgeMediaController] Video decoder failed: " << err_text;
    }
    else if( sender == &m_audio_decoder )
    {
        qDebug() << "[QEdgeMediaController] Audio decoder failed: " << err_text;
    }
    else
    {
        Q_ASSERT( 0 );
    }
}

void QEdgeMediaController::OnNewFrame( IDecoder *sender, AVFrame *frame )
{
    if( sender == &m_video_decoder )
    {
        m_synchronizer.ProcessVideo( frame );
    }
    else if( sender == &m_audio_decoder )
    {
        m_synchronizer.ProcessAudio( frame );
    }
}

void QEdgeMediaController::OnDemuxerFailed( QString err_text )
{
    qDebug() << "[QEdgeMediaController] Demuxer failed: " << err_text;
}

void QEdgeMediaController::OnDemuxerFinished()
{
    m_video_decoder.StopDecode( false );
    m_audio_decoder.StopDecode( false );
}

void QEdgeMediaController::InitStream( AVStream *video_stream, AVStream *audio_stream )
{
    m_synchronizer.SessionStarted( video_stream, audio_stream );
    m_audio_decoder.Init( audio_stream, this );
    m_video_decoder.Init( video_stream, this );

    m_subscriber->DurationSpecified( utils::TimebaseUnitsToMsecs( video_stream->time_base, video_stream->duration) );
}

void QEdgeMediaController::OnAudioPacket( AVPacket *packet )
{
    m_audio_decoder.OnNewPacket( packet );
}

void QEdgeMediaController::OnVideoPacket( AVPacket *packet )
{
    m_video_decoder.OnNewPacket( packet );
}
