#ifndef IMEDIACONTROLLER_H
#define IMEDIACONTROLLER_H

#include <QString>

class AVFrame;

class IMediaController
{
public:
    class IMediaControllerSubscriber
    {
    public:
        virtual void OnFailed( QString err_text ) = 0;
        virtual void OnNewVideoFrame( AVFrame* frame ) = 0;
        virtual void OnNewAudioFrame( AVFrame* frame ) = 0;
        virtual void OnPlayFinished() = 0;
        virtual void DurationSpecified( int64_t msecs ) = 0;
        virtual void CurrentTimestampChanged( int64_t msecs ) = 0;
    };

    virtual void ConnectToController( IMediaControllerSubscriber* subscriber ) = 0;
    virtual void Start( QString file_name ) = 0;
    virtual void AudioFrameProcessed( AVFrame* frame ) = 0;
    virtual void VideoFrameProcessed( AVFrame* frame ) = 0;
    virtual void AudioPresented( int64_t audio_data_remains ) = 0;
    virtual void VideoPresented() = 0;
    virtual void Stop() = 0;
    virtual void Seek( int64_t msec ) = 0;
};

class CNullMediaControllerSubscriber : public IMediaController::IMediaControllerSubscriber
{
public:
    virtual void OnFailed( QString err_text ) override
    {
        Q_UNUSED( err_text );
    }

    virtual void OnNewVideoFrame( AVFrame* frame ) override
    {
        Q_UNUSED( frame );
    }

    virtual void OnNewAudioFrame( AVFrame* frame ) override
    {
        Q_UNUSED( frame );
    }

    virtual void OnPlayFinished() override {}

    static CNullMediaControllerSubscriber* Instance()
    {
        static CNullMediaControllerSubscriber null_subscriber;
        return &null_subscriber;
    }

    virtual void DurationSpecified( int64_t msecs ) override
    {
        Q_UNUSED( msecs );
    }

    virtual void CurrentTimestampChanged( int64_t msecs ) override
    {
        Q_UNUSED( msecs );
    }

private:
    CNullMediaControllerSubscriber() {}
};

class CNullMediaController: public IMediaController
{
public:
    virtual void ConnectToController( IMediaControllerSubscriber* subscriber ) override
    {
        Q_UNUSED( subscriber );
    }

    virtual void Start( QString file_name ) override
    {
        Q_UNUSED( file_name );
    }
    virtual void Stop() override {}

    virtual void Seek( int64_t msec ) override
    {
        Q_UNUSED( msec );
    }

    virtual void AudioFrameProcessed( AVFrame* frame ) override
    {
        Q_UNUSED( frame );
    }

    virtual void VideoFrameProcessed( AVFrame* frame ) override
    {
        Q_UNUSED( frame );
    }

    virtual void VideoPresented() override {}

    virtual void AudioPresented( int64_t audio_data_remains ) override
    {
        Q_UNUSED( audio_data_remains );
    }

    static CNullMediaController* Instance()
    {
        static CNullMediaController null_media_controller;
        return &null_media_controller;
    }

private:
    CNullMediaController() {}
};

#endif // IMEDIACONTROLLER_H
