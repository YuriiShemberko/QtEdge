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
}

void QEdgeMediaController::Start( QString file_name )
{
    m_audio_finished = false;
    m_video_finished = false;

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
    m_synchronizer.DelayVideo();
}

void QEdgeMediaController::AudioPresented( long long audio_data_remains )
{
    m_synchronizer.UpdateAudioRemains( audio_data_remains );
}

void QEdgeMediaController::Stop()
{
    m_demuxer.Interrupt();
    m_audio_decoder.StopDecode( true );
    m_video_decoder.StopDecode( true );
}

void QEdgeMediaController::Seek( int msec )
{
    Q_UNUSED( msec );
    //TODO
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

void QEdgeMediaController::PreprocessVideo( AVFrame* frame )
{
    m_synchronizer.PreprocessVideo( frame );
}

void QEdgeMediaController::PreprocessAudio( AVFrame *frame )
{
    m_synchronizer.PreprocessAudio( frame );
}

void QEdgeMediaController::OnNewFrame( IDecoder *sender, AVFrame *frame )
{
    if( sender == &m_video_decoder )
    {
        PreprocessVideo( frame );
        m_subscriber->OnNewVideoFrame( frame );
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
    m_video_decoder.StopDecode( false );
    m_audio_decoder.StopDecode( false );
}

void QEdgeMediaController::InitStream( AVStream *video_stream, AVStream *audio_stream )
{
    m_synchronizer.Start( audio_stream, video_stream );
    m_audio_decoder.Init( audio_stream, this );
    m_video_decoder.Init( video_stream, this );
}

void QEdgeMediaController::OnAudioPacket( AVPacket *packet )
{
    m_audio_decoder.OnNewPacket( packet );
}

void QEdgeMediaController::OnVideoPacket( AVPacket *packet )
{
    m_video_decoder.OnNewPacket( packet );
}

void QEdgeMediaController::QEdgeSynchronizer::PreprocessAudio( AVFrame *frame )
{
    //std::lock_guard<std::mutex> sync_guard( m_sync_mtx );

    if( frame->pts != AV_NOPTS_VALUE )
    {
        m_audio_clock += av_q2d( m_audio_params.time_base ) * frame->pts;
    }
    else
    {
        m_audio_clock += frame->nb_samples / frame->channels * frame->sample_rate;
    }

    //Q_UNUSED( sync_guard );
}

void QEdgeMediaController::QEdgeSynchronizer::UpdateAudioRemains( long long data_remains )
{
    m_audio_remains = data_remains;
}

double QEdgeMediaController::QEdgeSynchronizer::GetAudioClock()
{
    double pts = m_audio_clock;
    int bytes_per_sec = m_audio_params.sample_rate *
                        m_audio_params.channels *
                        av_get_bytes_per_sample( (AVSampleFormat)m_audio_params.format );

    int buffer_size = m_audio_remains;

    if( bytes_per_sec != 0 )
    {
        pts -= (double)buffer_size / bytes_per_sec;
    }

    return pts;
}

void QEdgeMediaController::QEdgeSynchronizer::PreprocessVideo( AVFrame *frame )
{   
    m_sync_mtx.lock();

    double curr_pts = frame->pts;

    if( curr_pts == AV_NOPTS_VALUE )
    {
        curr_pts = frame->pkt_dts;
    }

    if( curr_pts == AV_NOPTS_VALUE )
    {
        curr_pts = 0;
    }

    curr_pts *= av_q2d( m_video_params.time_base );

    if( curr_pts != 0 )
    {
       m_video_clock = curr_pts;
    }

    else
    {
        curr_pts = m_video_clock;
    }

    double frame_delay = av_q2d( m_video_params.time_base );
    frame_delay += frame->repeat_pict * (frame_delay * 0.5);
    m_video_clock += frame_delay;

    m_current_frame_pts = curr_pts;

    std::this_thread::sleep_for( std::chrono::microseconds( (long long)(m_next_delay * 1000000 + 0.5) ) );
}

void QEdgeMediaController::QEdgeSynchronizer::Start( AVStream* audio_stream, AVStream* video_stream )
{
    m_sync_timer.restart();

    m_video_clock = (double)m_sync_timer.elapsed() / 1000.0;
    m_audio_clock = (double)m_sync_timer.elapsed() / 1000.0;
    m_frame_timer = 0;
    m_frame_last_delay = 0;
    m_frame_last_pts = 0;
    m_current_frame_pts = 0;
    m_next_delay = 0.03;
    m_audio_remains = 0;

    m_audio_params.time_base = audio_stream->time_base;
    m_video_params.time_base = video_stream->time_base;

    m_audio_params.format = audio_stream->codecpar->format;
    m_video_params.format = video_stream->codecpar->format;

    m_audio_params.sample_rate = audio_stream->codecpar->sample_rate;
    m_video_params.sample_rate = video_stream->codecpar->sample_rate;

    m_audio_params.channels = audio_stream->codecpar->channels;
}

void QEdgeMediaController::QEdgeSynchronizer::DelayVideo()
{
    double curr_pts = m_current_frame_pts;
    double delay = curr_pts - m_frame_last_pts;

    if( delay <= 0.0 || delay >= 1.0 )
    {
        // Delay incorrect - use previous one
        delay = m_frame_last_delay;
    }

    // Save for next time
    m_frame_last_pts = curr_pts;
    m_frame_last_delay = delay;

    double audio_clock = GetAudioClock();
    double diff = curr_pts - audio_clock;
    double sync_threshold = std::max( AV_SYNC_THRESHOLD, delay );

    if( fabs( diff ) < AV_NOSYNC_THRESHOLD)
    {
        if( diff <= -sync_threshold )
        {
            delay = 0;
        }

        else if( diff >= sync_threshold )
        {
            delay *= 2;
        }
    }

    m_frame_timer += delay;

    double actual_delay = m_frame_timer - (double)m_sync_timer.elapsed() / 1000.0;

    if( actual_delay < 0.010 )
    {
        //minimal delay
        actual_delay = 0.010;
    }

    m_next_delay = actual_delay;

    m_sync_mtx.unlock();
}
