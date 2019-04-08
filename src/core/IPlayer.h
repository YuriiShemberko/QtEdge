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
    };

    virtual void ConnectToPlayer( IPlayerClient* client ) = 0;
    virtual void DisconnectFromPlayer( IPlayerClient* client ) = 0;
    virtual void Start( QString file_name ) = 0;
    virtual void Stop() = 0;
    virtual void Seek( int msec ) = 0;
};

class CNullPlayer : public IPlayer
{
public:
    virtual void ConnectToPlayer( IPlayerClient* client )
    {
        Q_UNUSED( client );
    }

    virtual void DisconnectFromPlayer( IPlayerClient* client )
    {
        Q_UNUSED( client );
    }

    virtual void Start( QString file_name ) override
    {
        Q_UNUSED( file_name );
    }

    virtual void Stop() override {}

    virtual void Seek( int msec ) override
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
