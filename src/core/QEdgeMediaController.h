#ifndef QEDGEMEDIACONTROLLER_H
#define QEDGEMEDIACONTROLLER_H

#include <core/IMediaController.h>
#include <memory>

#include <core/QEdgeAudioDecoder.h>
#include <core/QEdgeVideoDecoder.h>
#include <core/QEdgeDemuxer.h>

#include <QElapsedTimer>

class QEdgeMediaController : public IMediaController, public IDecoder::IDecoderSubscriber, public IDemuxer::IDemuxerSubscriber
{
public:
    QEdgeMediaController();
    virtual void ConnectToController( IMediaControllerSubscriber *subscriber ) override;
    virtual void Start( QString file_name ) override;
    virtual void Stop() override;
    virtual void Seek( int msec ) override;

    virtual void OnDecoderFailed( IDecoder* sender, QString err_text ) override;
    virtual void OnNewFrame( IDecoder* sender, AVFrame* frame ) override;
    virtual void OnDecoderFinished( IDecoder* sender ) override;

    //IDemuxerSubscriber overrides
    virtual void OnDemuxerFailed( QString err_text ) override;
    virtual void OnDemuxerFinished() override;
    virtual void InitStream( AVStream* video_stream, AVStream* audio_stream ) override;
    virtual void OnAudioPacket( AVPacket* packet ) override;
    virtual void OnVideoPacket( AVPacket* packet ) override;

private:
    struct SSyncContext
    {
        double video_clock;
        double audio_clock;

        double frame_timer;
        double frame_last_pts;
        double frame_last_delay;

    } m_sync_ctx;

    void PreprocessVideo( AVFrame* frame );
    void PreprocessAudio( AVFrame* frame );
    double SyncVideoFrame( AVFrame* frame, double pts );

    double ComputeDelay( double current_pts );

    IMediaControllerSubscriber* m_subscriber;
    QEdgeAudioDecoder m_audio_decoder;
    QEdgeVideoDecoder m_video_decoder;
    QEdgeDemuxer m_demuxer;

    AVStream* m_audio_stream;
    AVStream* m_video_stream;

    QElapsedTimer m_sync_timer;
};

#endif //QEDGEMEDIACONTROLLER_H
