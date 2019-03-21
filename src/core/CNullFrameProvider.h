#ifndef CNULLFRAMEPROVIDER_H
#define CNULLFRAMEPROVIDER_H

#include <core/IFrameProvider.h>

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

    virtual void DisconnectClient(){}
};

#endif // CNULLFRAMEPROVIDER_H
