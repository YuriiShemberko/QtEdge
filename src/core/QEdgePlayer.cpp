#include <core/QEdgePlayer.h>

QEdgePlayer::QEdgePlayer() :
    m_video_receiver( CNullPlayerClient::Instance() ),
    m_audio_receiver( CNullPlayerClient::Instance() )
{
    m_media_controller.ConnectToController( this );
}

QEdgePlayer::~QEdgePlayer() {}

void QEdgePlayer::InitPlayer( IPlayerClient *video_receiver, IPlayerClient* audio_receiver )
{
    m_video_receiver.reset( video_receiver );
    m_audio_receiver.reset( audio_receiver );
}

void QEdgePlayer::DisconnectFromPlayer( IPlayerClient *client )
{
    if( client == m_audio_receiver.get() )
    {
        m_audio_receiver.reset( CNullPlayerClient::Instance() );
    }

    if( client == m_video_receiver.get() )
    {
        m_video_receiver.reset( CNullPlayerClient::Instance() );
    }
}

void QEdgePlayer::Start( QString file_name )
{
    m_media_controller.Start( file_name );
    m_video_receiver->PlayerStarted();
    m_audio_receiver->PlayerStarted();
}

void QEdgePlayer::Stop()
{
    m_media_controller.Stop();
    m_video_receiver->PlayerStopped();
    m_audio_receiver->PlayerStopped();
}

void QEdgePlayer::Seek( int msec )
{
    m_media_controller.Seek( msec );
}

void QEdgePlayer::OnPlayFinished()
{
    m_video_receiver->OnFinished();
    m_audio_receiver->OnFinished();
}

void QEdgePlayer::AudioProcessed( AVFrame *frame )
{
    m_media_controller.AudioFrameProcessed( frame );
}

void QEdgePlayer::VideoProcessed( AVFrame *frame )
{
    m_media_controller.VideoFrameProcessed( frame );
}

void QEdgePlayer::OnFailed( QString err_text )
{
    m_video_receiver->OnFailed( err_text );
    m_audio_receiver->OnFailed( err_text );
}

void QEdgePlayer::OnNewVideoFrame( AVFrame* frame )
{
    m_video_receiver->OnVideo( frame );
}

void QEdgePlayer::OnNewAudioFrame( AVFrame* frame )
{
    m_audio_receiver->OnAudio( frame );
}
