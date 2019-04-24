#ifndef QEDGEAUDIOREPRODUCTOR_H
#define QEDGEAUDIOREPRODUCTOR_H

#include <core/IPlayer.h>
#include <core/IDecoder.h>
#include <QIODevice>
#include <QAudioOutput>
#include <QEvent>

#include <memory>

class QEdgeBufferizedContainer : public QIODevice
{
    Q_OBJECT

public:
    QEdgeBufferizedContainer();
    ~QEdgeBufferizedContainer();

    void Start();
    void Stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;

signals:
    void eof();
    void bufferRead( qint64 remains );

private:
    qint64 m_pos;
    QByteArray m_buffer;
    bool m_eof;
};

class QEdgeAudioReproductor : public QObject, public IPlayer::IPlayerClient
{
    Q_OBJECT

public:
    QEdgeAudioReproductor();
    ~QEdgeAudioReproductor();

    virtual void Init( IPlayer* player ) override;
    virtual void OnAudio( AVFrame* audio_frame ) override;
    virtual void OnVideo( AVFrame* frame ) override;
    virtual void OnFailed( QString err_text ) override;
    virtual void OnFinished() override;
    virtual void PlayerStarted() override;
    virtual void PlayerStopped() override;
    virtual void DurationSpecified( int64_t msecs ) override;
    virtual void CurrentTimestampChanged( int64_t msecs ) override;

public slots:
    void Start();
    void Stop();
    void OnSetVolume( int volume );

protected:
    virtual bool event( QEvent* ev ) override;

private slots:
    void OnBufferEof();
    void OnBufferRead(   );

private:

    class QAudioOutputEvent : public QEvent
    {
    public:
        static const QEvent::Type s_audio_output_event_type = (QEvent::Type)(QEvent::User + 1);
        QAudioOutputEvent( bool start ) : QEvent(s_audio_output_event_type), m_start( start ) {}
        bool IsStart() { return m_start; }

    private:
        bool m_start;
    };

    std::unique_ptr<QAudioOutput> m_audio_output;
    QEdgeBufferizedContainer m_audio_buffer;
    QAudioFormat m_format;
    std::unique_ptr<IPlayer> m_player;

    bool m_audio_output_initialized;
};

#endif // QEDGEAUDIOREPRODUCTOR_H
