#ifndef QEDGEPLAYER_H
#define QEDGEPLAYER_H

#include <list>
#include <core/IPlayer.h>
#include <core/QEdgeMediaController.h>

class QEdgePlayer: public IPlayer, public IMediaController::IMediaControllerSubscriber
{
public:
    QEdgePlayer();
    ~QEdgePlayer();

    //IPlayer overrides
    virtual void ConnectToPlayer( IPlayerClient* client ) override;
    virtual void DisconnectFromPlayer( IPlayerClient* client ) override;
    virtual void Start( QString file_name ) override;
    virtual void Stop() override;
    virtual void Seek( int msec ) override;

    //IMediaControllerSubscriber overrides
    virtual void OnFailed( QString err_text ) override;
    virtual void OnNewVideoFrame( AVFrame* frame ) override;
    virtual void OnNewAudioFrame( AVFrame* frame ) override;

private:
    QEdgeMediaController m_media_controller;
    std::list<IPlayerClient*> m_clients;
};

#endif // QEDGEPLAYER_H
