#ifndef QEDGEPLAYER_H
#define QEDGEPLAYER_H

#include <core/IPlayer.h>
#include <core/QEdgeMediaController.h>

class QEdgePlayer: public IPlayer, public IMediaController::IMediaControllerSubscriber
{
public:
    QEdgePlayer();
    ~QEdgePlayer();

    //IPlayer overrides
    virtual void InitPlayer( IPlayerClient* video_recevier, IPlayerClient* audio_receiver ) override;
    virtual void DisconnectFromPlayer( IPlayerClient* client ) override;
    virtual void Start( QString file_name ) override;
    virtual void Stop() override;
    virtual void Seek( int64_t msec ) override;
    virtual void Pause() override;
    virtual void ContinuePlay() override;
    virtual void OnPlayFinished() override;
    virtual void AudioProcessed( AVFrame* frame ) override;
    virtual void VideoProcessed( AVFrame* frame ) override;
    virtual void AudioPresented( long long audio_data_remains ) override;
    virtual void VideoPresented() override;
    virtual void DurationSpecified( int64_t msecs ) override;
    virtual void CurrentTimestampChanged( int64_t msecs ) override;

    //IMediaControllerSubscriber overrides
    virtual void OnFailed( QString err_text ) override;
    virtual void OnNewVideoFrame( AVFrame* frame ) override;
    virtual void OnNewAudioFrame( AVFrame* frame ) override;

private:
    QEdgeMediaController m_media_controller;
    std::unique_ptr<IPlayerClient> m_video_receiver;
    std::unique_ptr<IPlayerClient> m_audio_receiver;
    int64_t m_last_timestamp;
    QString m_file_name;
    bool m_paused;
};

#endif // QEDGEPLAYER_H
