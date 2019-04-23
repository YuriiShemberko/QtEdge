#ifndef IPLAYER_H
#define IPLAYER_H

#include <QString>

class AVFrame;
class QString;

class IPlayer
{
public:
    class IPlayerClient
    {
    public:
        virtual void Init( IPlayer* player ) = 0;
        virtual void OnVideo( AVFrame* frame ) = 0;
        virtual void OnAudio( AVFrame* frame ) = 0;
        virtual void OnFailed( QString err_text ) = 0;
        virtual void OnFinished() = 0;
        virtual void PlayerStarted() = 0;
        virtual void PlayerStopped() = 0;
        virtual void DurationSpecified( int64_t msecs ) = 0;
        virtual void CurrentTimestampChanged( int64_t msecs ) = 0;
    };

    virtual void VideoProcessed( AVFrame* frame ) = 0;
    virtual void AudioProcessed( AVFrame* frame ) = 0;
    virtual void VideoPresented() = 0;
    virtual void AudioPresented( long long audio_data_remains ) = 0;
    virtual void InitPlayer( IPlayerClient* video_receiver, IPlayerClient* audio_receiver ) = 0;
    virtual void DisconnectFromPlayer( IPlayerClient* client ) = 0;
    virtual void Start( QString file_name ) = 0;
    virtual void Stop() = 0;
    virtual void Seek( int64_t msec ) = 0;
};

class CNullPlayer : public IPlayer
{
public:
    virtual void InitPlayer( IPlayerClient* video_receiver, IPlayerClient* audio_receiver ) override
    {
        Q_UNUSED( video_receiver );
        Q_UNUSED( audio_receiver );
    }

    virtual void DisconnectFromPlayer( IPlayerClient* client ) override
    {
        Q_UNUSED( client );
    }

    virtual void VideoProcessed( AVFrame* frame ) override
    {
        Q_UNUSED( frame );
    }

    virtual void AudioProcessed( AVFrame* frame ) override
    {
        Q_UNUSED( frame );
    }

    virtual void VideoPresented() override {}

    virtual void AudioPresented( long long audio_data_remains ) override
    {
        Q_UNUSED( audio_data_remains );
    }

    virtual void Start( QString file_name ) override
    {
        Q_UNUSED( file_name );
    }

    virtual void Stop() override {}

    virtual void Seek( int64_t msec ) override
    {
        Q_UNUSED( msec );
    }

    static CNullPlayer* Instance()
    {
        static CNullPlayer null_player;
        return &null_player;
    }

private:
    CNullPlayer() {}
};

class CNullPlayerClient: public IPlayer::IPlayerClient
{
public:
    virtual void Init( IPlayer* player ) override
    {
        Q_UNUSED( player );
    }
    virtual void OnVideo( AVFrame* frame ) override
    {
        Q_UNUSED( frame );
    }
    virtual void OnAudio( AVFrame* frame ) override
    {
        Q_UNUSED( frame );
    }

    virtual void DurationSpecified( int64_t msecs ) override
    {
        Q_UNUSED( msecs );
    }

    virtual void CurrentTimestampChanged( int64_t msecs ) override
    {
        Q_UNUSED( msecs );
    }

    virtual void PlayerStarted() override {}
    virtual void PlayerStopped() override {}

    virtual void OnFailed( QString err_text ) override
    {
        Q_UNUSED( err_text );
    }

    virtual void OnFinished() override {}

    static CNullPlayerClient* Instance()
    {
        static CNullPlayerClient null_player_client;
        return &null_player_client;
    }

private:
    CNullPlayerClient() {}
};
#endif // IPLAYER_H
