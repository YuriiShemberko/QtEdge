#ifndef QEDGEAUDIODECODER_H
#define QEDGEAUDIODECODER_H

#include <core/QEdgeDecoderBase.h>

class QEdgeAudioDecoder : public QEdgeDecoderBase
{
public:
    QEdgeAudioDecoder();
    virtual void Decode( AVPacket* packet ) override;
};

#endif // QEDGEAUDIODECODER_H
