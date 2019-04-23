#ifndef QEDGEMEDIACONTROLLER_H
#define QEDGEMEDIACONTROLLER_H

#include <QElapsedTimer>

#include <memory>

#include <core/IMediaController.h>
#include <core/QEdgeAudioDecoder.h>
#include <core/QEdgeVideoDecoder.h>
#include <core/QEdgeDemuxer.h>
#include <core/QEdgeSynchronizer.h>

class QEdgeMediaController : public IMediaController,
                             public IDecoder::IDecoderSubscriber,
                             public IDemuxer::IDemuxerSubscriber
{
public:
    QEdgeMediaController();

    //IMediaController overrides
    virtual void ConnectToController( IMediaControllerSubscriber *subscriber ) override;
    virtual void Start( QString file_name ) override;
    virtual void VideoFrameProcessed( AVFrame* frame ) override;
    virtual void AudioFrameProcessed( AVFrame* frame ) override;
    virtual void VideoPresented() override;
    virtual void AudioPresented( long long audio_data_remains ) override;
    virtual void Stop() override;
    virtual void Seek( int64_t msec ) override;

    //IDecoderSubscriber overrides
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

    private:
    QEdgeSynchronizer m_synchronizer;

    IMediaControllerSubscriber* m_subscriber;
    QEdgeAudioDecoder m_audio_decoder;
    QEdgeVideoDecoder m_video_decoder;
    QEdgeDemuxer m_demuxer;

    std::unique_ptr<AVCodecContext> m_video_ctx;
    std::unique_ptr<AVCodecContext> m_audio_ctx;

    bool m_audio_finished;
    bool m_video_finished;

    bool m_presentation_started;
};

#endif //QEDGEMEDIACONTROLLER_H
