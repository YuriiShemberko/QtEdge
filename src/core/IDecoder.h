#ifndef IDECODER_H
#define IDECODER_H

class AVPacket;
class AVStream;
class AVFrame;

#include <QString>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

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
    virtual void StopDecode() = 0;
};

class CNullDecoder : public IDecoder
{
public:
    virtual void Init( AVStream* stream, IDecoderSubscriber* subscriber ) override
    {
        Q_UNUSED( subscriber );
        Q_UNUSED( stream );
    }

    virtual void StopDecode() override {}

    static CNullDecoder* Instance()
    {
        static CNullDecoder null_decoder;
        return &null_decoder;
    }

private:
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

class IAudioDecoder : public IDecoder
{
public:
    virtual void OnNewAudioPacket( AVPacket* audio_packet ) = 0;
};

class IVideoDecoder : public IDecoder
{
public:
    virtual void OnNewVideoPacket( AVPacket* video_packet ) = 0;
};

class CNullAudioDecoder : public IAudioDecoder
{
public:
    virtual void Init( AVStream* stream, IDecoderSubscriber* subscriber ) override
    {
        Q_UNUSED( subscriber );
        Q_UNUSED( stream );
    }

    virtual void StopDecode() override {}

    virtual void OnNewAudioPacket( AVPacket* audio_packet ) override
    {
        Q_UNUSED( audio_packet );
    }

    static CNullAudioDecoder* Instance()
    {
        static CNullAudioDecoder null_audio_decoder;
        return &null_audio_decoder;
    }

private:
    CNullAudioDecoder() {}
};

class CNullVideoDecoder : public IVideoDecoder
{
public:
    virtual void Init( AVStream* stream, IDecoderSubscriber* subscriber ) override
    {
        Q_UNUSED( subscriber );
        Q_UNUSED( stream );
    }

    virtual void StopDecode() override {}

    virtual void OnNewVideoPacket( AVPacket* video_packet ) override
    {
        Q_UNUSED( video_packet );
    }

    static CNullVideoDecoder* Instance()
    {
        static CNullVideoDecoder null_video_decoder;
        return &null_video_decoder;
    }

private:
    CNullVideoDecoder() {}
};


#endif // IDECODER_H
