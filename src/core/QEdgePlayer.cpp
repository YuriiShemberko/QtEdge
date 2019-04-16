#include <core/QEdgePlayer.h>

QEdgePlayer::QEdgePlayer()
{
    m_media_controller.ConnectToController( this );
}

QEdgePlayer::~QEdgePlayer() {}

void QEdgePlayer::ConnectToPlayer( IPlayerClient *client )
{
    m_clients.push_back( client );
}

void QEdgePlayer::DisconnectFromPlayer( IPlayerClient *client )
{
    for( auto it = m_clients.begin(); it != m_clients.end(); ++it )
    {
        if( client == *it )
        {
            m_clients.erase( it );
            return;
        }
    }
}

void QEdgePlayer::Start( QString file_name )
{
    for( auto it: m_clients )
    {
        it->PlayerStarted();
    }

    m_media_controller.Start( file_name );
}

void QEdgePlayer::Stop()
{
    for( auto it: m_clients )
    {
        it->PlayerStopped();
    }

    m_media_controller.Stop();
}

void QEdgePlayer::Seek( int msec )
{
    m_media_controller.Seek( msec );
}

void QEdgePlayer::OnPlayFinished()
{
    for( auto it = m_clients.begin(); it != m_clients.end(); ++it )
    {
        (*it)->OnFinished();
    }
}

void QEdgePlayer::OnFailed( QString err_text )
{
    for( auto it = m_clients.begin(); it != m_clients.end(); ++it )
    {
        (*it)->OnFailed( err_text );
    }
}

void QEdgePlayer::OnNewVideoFrame( AVFrame* frame )
{
    for( auto it = m_clients.begin(); it != m_clients.end(); ++it )
    {
        (*it)->OnVideo( frame );
    }
}

void QEdgePlayer::OnNewAudioFrame( AVFrame* frame )
{
    for( auto it = m_clients.begin(); it != m_clients.end(); ++it )
    {
        (*it)->OnAudio( frame );
    }
}
