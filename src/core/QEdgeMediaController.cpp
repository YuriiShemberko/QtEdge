#include <core/QEdgeMediaController.h>
#include <QDebug>
#include <time.h>

QEdgeMediaController::QEdgeMediaController() :
    m_subscriber( CNullMediaControllerSubscriber::Instance() ),
    m_audio_stream(nullptr),
    m_video_stream(nullptr)
{}

void QEdgeMediaController::ConnectToController( IMediaController::IMediaControllerSubscriber *subscriber )
{
    m_subscriber = subscriber;
}

void QEdgeMediaController::Start( QString file_name )
{
    m_demuxer.Init( this );

    if( !m_demuxer.Start( file_name ) )
    {
        OnDemuxerFailed( "cannot start demuxer" );
        return;
    }

    m_sync_timer.restart();

    m_sync_ctx.frame_timer = 0;
    m_sync_ctx.video_clock = m_sync_timer.elapsed() / 1000.0;
    m_sync_ctx.audio_clock = 0;
    m_sync_ctx.frame_last_delay = 0;
    m_sync_ctx.frame_last_pts = 0;
}

void QEdgeMediaController::Stop()
{
    m_demuxer.Interrupt();
    m_audio_decoder.StopDecode();
    m_video_decoder.StopDecode();
}

void QEdgeMediaController::Seek( int msec )
{
    Q_UNUSED( msec );
    //TODO
}

void QEdgeMediaController::OnDecoderFinished( IDecoder *sender )
{
    Q_UNUSED( sender );
    //TODO
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

double QEdgeMediaController::ComputeDelay( double curr_pts )
{
    double delay = curr_pts - m_sync_ctx.frame_last_pts;

    if (delay <= 0.0 || delay >= 1.0)
    {
        // Delay incorrect - use previous one
        delay = m_sync_ctx.frame_last_pts;
    }

    // Save for next time
    m_sync_ctx.frame_last_pts = curr_pts;
    m_sync_ctx.frame_last_delay = delay;

    m_sync_ctx.frame_timer += delay;

    int elapsed = m_sync_timer.elapsed();
    double actual_delay = m_sync_ctx.frame_timer - m_sync_timer.elapsed() / 10000.0;

    if(actual_delay < 0.010)
    {
        /* Really it should skip the picture instead */
        actual_delay = 0.010;
    }
    return actual_delay;
}

void QEdgeMediaController::PreprocessVideo( AVFrame* frame )
{
    double pts = frame->pts;
    if( pts == AV_NOPTS_VALUE )
    {
        pts = frame->pkt_dts;
    }
    if( pts == AV_NOPTS_VALUE )
    {
        pts = 0;
    }

    pts *= av_q2d( m_video_stream->time_base );
    pts = SyncVideoFrame( frame, pts );

    double delay = ComputeDelay( pts );
    std::this_thread::sleep_for( std::chrono::milliseconds( 40 ) );

    m_subscriber->OnNewVideoFrame( frame );
}

void QEdgeMediaController::PreprocessAudio( AVFrame *frame )
{
    if( frame->pts != AV_NOPTS_VALUE )
    {
        m_sync_ctx.audio_clock += av_q2d( m_audio_stream->time_base ) * frame->pts;
    }
    else
    {
        m_sync_ctx.audio_clock += frame->nb_samples / frame->channels * frame->sample_rate;
    }
}

double QEdgeMediaController::SyncVideoFrame( AVFrame *frame, double pts )
{
    if(pts != 0)
    {
        m_sync_ctx.video_clock = pts;
    }
    else
    {
        pts = m_sync_ctx.video_clock;
    }
    double frame_delay = av_q2d( m_video_stream->time_base );

    frame_delay += frame->repeat_pict * (frame_delay * 0.5);
    m_sync_ctx.video_clock += frame_delay;

    return pts;
}

void QEdgeMediaController::OnNewFrame( IDecoder *sender, AVFrame *frame )
{
    if( sender == &m_video_decoder )
    {
        PreprocessVideo( frame );
        //new video
    }
    else if( sender == &m_audio_decoder )
    {
        PreprocessAudio( frame );
        m_subscriber->OnNewAudioFrame( frame );
        //new audio
    }
}

void QEdgeMediaController::OnDemuxerFailed( QString err_text )
{
    qDebug() << "[QEdgeMediaController] Demuxer failed: " << err_text;
}

void QEdgeMediaController::OnDemuxerFinished()
{

}

void QEdgeMediaController::InitStream( AVStream *video_stream, AVStream *audio_stream )
{
    m_video_stream = video_stream;
    m_audio_stream = audio_stream;

    m_audio_decoder.Init( audio_stream, this );
    m_video_decoder.Init( video_stream, this );
}

void QEdgeMediaController::OnAudioPacket( AVPacket *packet )
{
    m_audio_decoder.OnNewAudioPacket( packet );
}

void QEdgeMediaController::OnVideoPacket( AVPacket *packet )
{
    m_video_decoder.OnNewVideoPacket( packet );
}
