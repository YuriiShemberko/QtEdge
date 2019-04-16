#ifndef QEDGEMEDIACONTROLLER_H
#define QEDGEMEDIACONTROLLER_H

#include <core/IMediaController.h>
#include <memory>

#include <core/QEdgeAudioDecoder.h>
#include <core/QEdgeVideoDecoder.h>
#include <core/QEdgeDemuxer.h>

#include <QElapsedTimer>

#define AV_SYNC_THRESHOLD 0.01
#define AV_NOSYNC_THRESHOLD 10.0

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
    class QEdgeSynchronizer
    {
    public:
        void Start( AVRational audio_time_base, AVRational video_time_base );

        double SyncVideo( AVFrame* frame );
        void SyncAudio( AVFrame* frame );

    private:
        double ComputeVideoDelay( AVFrame* frame );

        double m_video_clock;
        double m_audio_clock;

        double m_frame_timer;
        double m_frame_last_pts;
        double m_frame_last_delay;

        QElapsedTimer m_sync_timer;

        std::mutex m_sync_mtx;

        AVRational m_audio_time_base;
        AVRational m_video_time_base;

    } m_synchronizer;

    void PreprocessVideo( AVFrame* frame );
    void PreprocessAudio( AVFrame* frame );

    IMediaControllerSubscriber* m_subscriber;
    QEdgeAudioDecoder m_audio_decoder;
    QEdgeVideoDecoder m_video_decoder;
    QEdgeDemuxer m_demuxer;

    bool m_audio_finished;
    bool m_video_finished;
};

#endif //QEDGEMEDIACONTROLLER_H
