#ifndef IFRAMEPROVIDER_H
#define IFRAMEPROVIDER_H

#include <QImage>
#include <core/IDecoder.h>

class IFrameConverter
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

class CNullFrameProviderClient : public IFrameProviderClient
{
public:

    static CNullFrameProviderClient* Instance()
    {
        static CNullFrameProviderClient instance;
        return &instance;
    }

    virtual QSize TargetSize()
    {
        return QSize();
    }

    virtual void OnFailed() {}
    virtual void OnStarted() {}
    virtual void OnFinished() {}
    virtual void OnNewFrame( const QImage& img )
    {
        Q_UNUSED( img );
    }

    virtual void Disconnected() {}

private:
    CNullFrameProviderClient() {}
};

class CNullFrameProvider : public IFrameProvider
{
public:
    static CNullFrameProvider* Instance()
    {
        static CNullFrameProvider instance;
        return &instance;
    }

    virtual void BindClient( IFrameProviderClient* client )
    {
        Q_UNUSED( client );
    }

    virtual bool Start( SFrameProviderConfig config )
    {
        Q_UNUSED( config );
        return false;
    }

    virtual void DisconnectClient() {}

private:
    CNullFrameProvider() {}
};

#endif // IFRAMEPROVIDER_H
