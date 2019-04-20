#ifndef QEDGEDECODERBASE_H
#define QEDGEDECODERBASE_H

#include <memory>
#include <thread>
#include <queue>
#include <mutex>

#include <QEvent>
#include <QCoreApplication>

#include <core/IDecoder.h>
#include <core/QEdgeUtils.h>

class QEdgeDecoderBase : public QObject, public IDecoder
{
    Q_OBJECT
public:
    QEdgeDecoderBase();

    virtual void Init( AVStream *stream, IDecoder::IDecoderSubscriber* subscriber ) override;
    virtual void StopDecode( bool interrupt ) override;
    virtual void OnNewPacket( AVPacket* packet ) override;

protected:
    static void DecodeThread( void* ctx );
    AVPacket* PopPacket();
    void PushPacket( AVPacket* packet );

    bool IsQueueEmpty() { return m_packet_queue.IsEmpty(); }
    void FreeQueue();
    void OnNewFrame( AVFrame* frame );
    void OnFailed( QString err_text );

    void OnThreadFinished();

    virtual void Decode( AVPacket* packet ) = 0;

    class QCleanupEvent : public QEvent
    {
    public:
        static const QEvent::Type s_cleanup_event_type = (QEvent::Type)(QEvent::User + 1);
        QCleanupEvent() : QEvent(s_cleanup_event_type) {}
    };

    virtual bool event( QEvent* ev ) override;

    void FreeCodecContext();

    AVStream* m_stream;
    std::unique_ptr<IDecoderSubscriber> m_subscriber;
    std::unique_ptr<AVCodecContext> m_codec_context;

    utils::QEdgeMultithreadQueue<AVPacket*> m_packet_queue;
    std::unique_ptr<std::thread> m_decode_thread;

    volatile bool m_running;
    bool m_demuxer_finished;
};

#endif // QEDGEDECODERBASE_H
