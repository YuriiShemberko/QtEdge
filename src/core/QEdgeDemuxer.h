#ifndef QEDGEDEMUXER_H
#define QEDGEDEMUXER_H

#include <memory>
#include <thread>

#include <core/IDemuxer.h>

class AVStream;

class QEdgeDemuxer : public IDemuxer
{
public:
    QEdgeDemuxer();

    //IDemuxer overrides
    virtual void Init( IDemuxerSubscriber* subscriber ) override;
    virtual bool Start( QString url ) override;
    virtual void Interrupt() override;

private:
    static void DemuxInThread( void *ctx );
    void OnFailed( QString err_text );
    void OnFinished();

    QString m_url;

    IDemuxerSubscriber* m_subscriber;

    bool m_running;

    std::unique_ptr<std::thread> m_thread;
};

#endif // QEDGEDEMUXER_H
