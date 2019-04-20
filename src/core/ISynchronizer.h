#ifndef ISYNCHRONIZER_H
#define ISYNCHRONIZER_H

#include <QtGlobal>

#include <core/IMediaController.h>
#include <core/Global.h>

class ISynchronizer
{
public:
    virtual void SessionStarted( const AVStream *video_stream, const AVStream *audio_stream ) = 0;
    virtual void BindMediaReceiver( IMediaController::IMediaControllerSubscriber* receiver ) = 0;
    virtual void ProcessAudio( AVFrame* frame ) = 0;
    virtual void ProcessVideo( AVFrame* frame ) = 0;
    virtual void OnAudioPresented( long long buffer_remains ) = 0;
    virtual void OnVideoPresented() = 0;
};

class CNullSynchronizer : public ISynchronizer
{
public:
    virtual void SessionStarted( const AVStream *video_stream, const AVStream *audio_stream ) override
    {
        Q_UNUSED( video_stream );
        Q_UNUSED( audio_stream );
    }

    virtual void BindMediaReceiver( IMediaController::IMediaControllerSubscriber* receiver ) override
    {
        Q_UNUSED( receiver );
    }

    virtual void ProcessAudio( AVFrame* frame ) override
    {
        Q_UNUSED( frame );
    }

    virtual void ProcessVideo( AVFrame* frame ) override
    {
        Q_UNUSED( frame );
    }

    virtual void OnAudioPresented( long long buffer_remains ) override
    {
        Q_UNUSED( buffer_remains );
    }

    virtual void OnVideoPresented() override { }
};

#endif // ISYNCHRONIZER_H
