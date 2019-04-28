#ifndef QEDGESYNCHRONIZER_H
#define QEDGESYNCHRONIZER_H

#include <QEvent>
#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QCoreApplication>

#include <memory>
#include <mutex>
#include <condition_variable>

#include <core/Global.h>
#include <core/ISynchronizer.h>
#include <core/QEdgeUtils.h>

class QEdgeSynchronizer : public QObject, public ISynchronizer
{
    Q_OBJECT
public:
    QEdgeSynchronizer( QObject* parent = nullptr );

    //ISynchronizer overrides
    virtual void SessionStarted( const AVStream *video_stream, const AVStream *audio_stream ) override;
    virtual void BindMediaReceiver( IMediaController::IMediaControllerSubscriber* receiver ) override;
    virtual void ProcessAudio( AVFrame* frame ) override;
    virtual void ProcessVideo( AVFrame* frame ) override;
    virtual void OnAudioPresented( long long buffer_remains ) override;
    virtual void OnVideoPresented() override;

private:
    void UpdateVideoRate( AVFrame* frame );
    void UpdateAudioRate( AVFrame* frame );
    int GetMsDelay( double delay );
    void PushNextFrame( AVFrame* frame );
    double GetAudioRate();
    void SyncVideo();

    struct SStreamParams
    {
        AVRational time_base;
        int sample_rate;
        int format;
        int channels; //for audio

    } m_audio_params, m_video_params;

    double m_video_rate;
    double m_audio_rate;
    double m_frame_timer;
    double m_current_frame_pts;
    double m_frame_last_pts;
    double m_frame_last_delay;
    long long m_audio_remains;
    double m_next_delay;
    bool m_video_started;
    bool m_audio_buffer_full;

    QElapsedTimer m_sync_timer;

    volatile bool m_audio_started;
    std::condition_variable m_audio_started_condition;
    std::mutex m_audio_wait_mtx;

    std::unique_ptr<IMediaController::IMediaControllerSubscriber> m_receiver;
};

#endif // QEDGESYNCHRONIZER_H
