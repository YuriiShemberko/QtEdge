#ifndef IDEMUXER_H
#define IDEMUXER_H

#include <QString>

#include <core/Global.h>

class IDemuxer
{
public:
    class IDemuxerSubscriber
    {
    public:
        virtual void InitStream( AVStream* video_stream, AVStream* audio_stream ) = 0;
        virtual void OnVideoPacket( AVPacket* packet ) = 0;
        virtual void OnAudioPacket( AVPacket* packet ) = 0;
        virtual void OnDemuxerFailed( QString err_text ) = 0;
        virtual void OnDemuxerFinished() = 0;
    };

    virtual void Init( IDemuxerSubscriber* subscriber ) = 0;
    virtual bool Start( QString url, int seek_pos = 0 ) = 0;
    virtual void Interrupt() = 0;
    virtual void Seek( int msec ) = 0;
};

class CNullDemuxerSubscriber : public IDemuxer::IDemuxerSubscriber
{
public:
    virtual void InitStream( AVStream* video_stream, AVStream* audio_stream )
    {
        Q_UNUSED( audio_stream );
        Q_UNUSED( video_stream );
    }

    virtual void OnDemuxerFailed( QString err_text ) override
    {
        Q_UNUSED( err_text );
    }

    virtual void OnVideoPacket( AVPacket* packet ) override
    {
        Q_UNUSED( packet );
    }

    virtual void OnAudioPacket( AVPacket* packet ) override
    {
        Q_UNUSED( packet );
    }

    virtual void OnDemuxerFinished() override {}

    static CNullDemuxerSubscriber* Instance()
    {
        static CNullDemuxerSubscriber null_demuxer_notifier;
        return &null_demuxer_notifier;
    }

private:
    CNullDemuxerSubscriber() {}
};

class CNullDemuxer: public IDemuxer
{
public:
    virtual void Init( IDemuxerSubscriber* subscriber ) override
    {
        Q_UNUSED( subscriber );
    }

    virtual bool Start( QString url, int seek_pos = 0 ) override
    {
        Q_UNUSED( url );
        Q_UNUSED( seek_pos );
        return false;
    }

    virtual void Interrupt() override { }

    virtual void Seek( int msec ) override
    {
        Q_UNUSED( msec );
    }

    static CNullDemuxer* Instance()
    {
        static CNullDemuxer demuxer;
        return &demuxer;
    }

private:
    CNullDemuxer() {}
};

#endif // IDEMUXER_H
