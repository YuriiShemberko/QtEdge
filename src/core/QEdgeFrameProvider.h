#ifndef QEDGEFRAMEPROVIDER_H
#define QEDGEFRAMEPROVIDER_H

#include <memory>
#include <thread>
#include <QDebug>
#include <core/CNullFrameProvider.h>

class QEdgeFrameProvider : public CNullFrameProvider
{
public:
    QEdgeFrameProvider();
    ~QEdgeFrameProvider();
    virtual void BindClient( IFrameProviderClient* client );
    virtual void DisconnectClient();
    virtual bool Start( SFrameProviderConfig config );

private:
    static void StartReadFramesInThread( void* ctx );
    void OnFailed( QString error_text );
    void OnFinished();

    SFrameProviderConfig m_config;
    IFrameProviderClient* m_client;
    std::unique_ptr<std::thread> m_thread;
    bool m_busy;
};

#endif // QEDGEFRAMEPROVIDER_H
