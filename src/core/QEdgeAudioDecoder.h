#ifndef QEDGEAUDIODECODER_H
#define QEDGEAUDIODECODER_H

#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <core/QEdgeDecoderBase.h>

class AVCodecContext;

class QEdgeAudioDecoder : public QEdgeDecoderBase
{
public:
    QEdgeAudioDecoder();
    virtual void Decode( AVPacket* packet ) override;
};

#endif // QEDGEAUDIODECODER_H
