#include <core/QEdgeSynchronizer.h>
#include <thread>
#include <QDebug>

QEdgeSynchronizer::QEdgeSynchronizer( QObject* parent ) :
    QObject( parent ),
    m_next_delay( AV_NOSYNC_DELAY ),
    m_receiver( CNullMediaControllerSubscriber::Instance() )
{
}

void QEdgeSynchronizer::SessionStarted( const AVStream *video_stream, const AVStream *audio_stream )
{
    m_audio_params.time_base = video_stream->time_base;
    m_video_params.time_base = video_stream->time_base;
    m_audio_params.format = audio_stream->codecpar->format;
    m_video_params.format = video_stream->codecpar->format;
    m_audio_params.sample_rate = audio_stream->codecpar->sample_rate;
    m_video_params.sample_rate = video_stream->codecpar->sample_rate;
    m_audio_params.channels = audio_stream->codecpar->channels;

    m_video_rate = 0;
    m_audio_rate = 0;
    m_next_delay = AV_NOSYNC_DELAY;
    m_frame_timer = 0;
    m_frame_last_delay = 0;
    m_frame_last_pts = 0;
    m_current_frame_pts = 0;
    m_audio_remains = 0;
    m_audio_buffer_full = false;
    m_video_started = false;
    m_audio_started = false;
}

void QEdgeSynchronizer::ProcessVideo( AVFrame *frame )
{
    UpdateVideoRate( frame );
    PushNextFrame( frame );
}

void QEdgeSynchronizer::OnAudioPresented( long long buffer_remains )
{
    m_audio_remains = buffer_remains;
    if( !m_audio_started )
    {
        std::lock_guard<std::mutex> audio_started_lock( m_audio_wait_mtx );
        m_audio_started = true;
        m_audio_started_condition.notify_one();
        Q_UNUSED( audio_started_lock );
    }

    m_audio_buffer_full = buffer_remains >= AV_MAX_AUDIO_BUF_SIZE;
}

void QEdgeSynchronizer::OnVideoPresented()
{
    SyncVideo();
}

void QEdgeSynchronizer::PushNextFrame( AVFrame* frame )
{
    if( !m_audio_started )
    {
        std::unique_lock<std::mutex> audio_started_lock( m_audio_wait_mtx );
        m_audio_started_condition.wait( audio_started_lock );
        audio_started_lock.unlock();
    }

    m_receiver->OnNewVideoFrame( frame );

    int64_t pts = frame->pts;
    if( pts == AV_NOPTS_VALUE )
    {
        pts = frame->pkt_dts;
    }
    pts = utils::TimebaseUnitsToMsecs( m_video_params.time_base, pts );

    if( pts == AV_NOPTS_VALUE )
    {
        pts = m_current_frame_pts * 1000;
    }

    m_receiver->CurrentTimestampChanged( pts  );
    utils::QEdgeSleep( GetMsDelay( m_next_delay ) );
}

void QEdgeSynchronizer::ProcessAudio( AVFrame *frame )
{
    UpdateAudioRate( frame );

    while( m_audio_buffer_full )
    {
        utils::QEdgeSleep( 10 );
    }

    m_receiver->OnNewAudioFrame( frame );
}

int QEdgeSynchronizer::GetMsDelay( double delay )
{
    return int( delay * 1000.0 + 0.5 );
}

double QEdgeSynchronizer::GetAudioRate()
{
    double pts = m_audio_rate;
    int bytes_per_sec = m_audio_params.sample_rate *
                        m_audio_params.channels *
                        av_get_bytes_per_sample( (AVSampleFormat)m_audio_params.format );

    long long buffer_size = m_audio_remains;

    if( bytes_per_sec != 0 )
    {
        pts -= (double)buffer_size / bytes_per_sec;
    }

    return pts;
}

void QEdgeSynchronizer::SyncVideo()
{
    if( !m_video_started )
    {
        m_video_started = true;
        m_sync_timer.restart();
    }

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

    double audio_clock = GetAudioRate();
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
}

void QEdgeSynchronizer::UpdateAudioRate( AVFrame *frame )
{
    if( frame->pts != AV_NOPTS_VALUE )
    {
        m_audio_rate += av_q2d( m_audio_params.time_base ) * frame->pts;
    }

    else
    {
        m_audio_rate += frame->nb_samples / frame->channels * frame->sample_rate;
    }
}

void QEdgeSynchronizer::UpdateVideoRate( AVFrame *frame )
{
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
       m_video_rate = curr_pts;
    }

    else
    {
        curr_pts = m_video_rate;
    }

    double frame_delay = av_q2d( m_video_params.time_base );
    frame_delay += frame->repeat_pict * (frame_delay * 0.5);
    m_video_rate += frame_delay;

    m_current_frame_pts = curr_pts;
}

void QEdgeSynchronizer::BindMediaReceiver( IMediaController::IMediaControllerSubscriber *receiver )
{
    m_receiver.reset( receiver );
}
