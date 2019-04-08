#ifndef QEDGEVIDEODECODER_H
#define QEDGEVIDEODECODER_H

#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <core/IDecoder.h>

class AVCodecContext;

class QEdgeVideoDecoder : public IVideoDecoder
{
public:
    QEdgeVideoDecoder();

    virtual void Init( AVStream *stream, IDecoderSubscriber* subscriber ) override;
    virtual void StopDecode() override;
    virtual void OnNewVideoPacket( AVPacket* packet );

private:
    static void DecodeThread( void* ctx );
    AVPacket* PopPacket();
    void PushPacket( AVPacket* packet );
    void FreeQueue();

    void OnNewFrame( AVFrame* frame );
    void OnFailed( QString err_text );

    std::unique_ptr<AVStream> m_stream;
    std::unique_ptr<IDecoderSubscriber> m_subscriber;
    std::unique_ptr<AVCodecContext> m_codec_context;

    std::mutex m_queue_mutex;
    std::queue<AVPacket*> m_packet_queue;
    std::unique_ptr<std::thread> m_decode_thread;

    bool m_running;

};

#endif // QEDGEVIDEODECODER_H
