#ifndef IDECODER_H
#define IDECODER_H

#include <QString>

#include <core/GLobal.h>

class IDecoder
{
public:
    class IDecoderSubscriber
    {
    public:
        virtual void OnDecoderFailed( IDecoder* sender, QString err_text ) = 0;
        virtual void OnNewFrame( IDecoder* sender, AVFrame* frame ) = 0;
        virtual void OnDecoderFinished( IDecoder* sender ) = 0;
    };

    virtual void Init( AVStream* stream, IDecoderSubscriber* subscriber ) = 0;
    virtual void StopDecode( bool interrupt ) = 0;
    virtual void OnNewPacket( AVPacket* packet ) = 0;
};

class CNullDecoder : public IDecoder
{
public:
    virtual void Init( AVStream* stream, IDecoderSubscriber* subscriber ) override
    {
        Q_UNUSED( subscriber );
        Q_UNUSED( stream );
    }

    virtual void StopDecode( bool interrupt ) override
    {
        Q_UNUSED( interrupt );
    }

    virtual void OnNewPacket( AVPacket* packet )
    {
        Q_UNUSED( packet );
    }

    static CNullDecoder* Instance()
    {
        static CNullDecoder null_decoder;
        return &null_decoder;
    }


    CNullDecoder() {}
};

class CNullDecoderSubscriber: public IDecoder::IDecoderSubscriber
{
public:
    virtual void OnDecoderFailed( IDecoder* sender, QString err_text ) override
    {
        Q_UNUSED( sender );
        Q_UNUSED( err_text );
    }

    virtual void OnDecoderFinished( IDecoder* sender ) override
    {
        Q_UNUSED( sender );
    }

    virtual void OnNewFrame( IDecoder* sender, AVFrame* frame ) override
    {
        Q_UNUSED( sender );
        Q_UNUSED( frame );
    }

    static CNullDecoderSubscriber* Instance()
    {
        static CNullDecoderSubscriber null_decoder_subscriber;
        return &null_decoder_subscriber;
    }

private:
    CNullDecoderSubscriber() {}
};



#endif // IDECODER_H
