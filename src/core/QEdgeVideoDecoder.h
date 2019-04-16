#ifndef QEDGEVIDEODECODER_H
#define QEDGEVIDEODECODER_H

#include <core/QEdgeDecoderBase.h>

class AVCodecContext;

class QEdgeVideoDecoder : public QEdgeDecoderBase
{
public:
    QEdgeVideoDecoder();
    virtual void Decode( AVPacket *packet ) override;
};

#endif // QEDGEVIDEODECODER_H
