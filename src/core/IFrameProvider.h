#ifndef IFRAMEPROVIDER_H
#define IFRAMEPROVIDER_H

#include <QImage>

struct SFrameProviderConfig
{
    QString file_name;
};

class IFrameProviderClient
{
public:
    virtual void OnStarted() = 0;
    virtual void OnFinished() = 0;
    virtual void OnFailed() = 0;
    virtual void OnNewFrame( const QImage& img ) = 0;
    virtual void Disconnected() = 0;
    virtual QSize TargetSize() = 0;
};

class IFrameProvider
{
public:
    virtual void BindClient( IFrameProviderClient* client ) = 0;
    virtual bool Start( SFrameProviderConfig config ) = 0;
    virtual void DisconnectClient() = 0;
};

#endif // IFRAMEPROVIDER_H
