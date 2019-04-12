#ifndef QEDGEAUDIOREPRODUCTOR_H
#define QEDGEAUDIOREPRODUCTOR_H

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

private:
    qint64 m_pos;
    QByteArray m_buffer;
};

class QEdgeAudioReproductor : public QObject
{
    Q_OBJECT

public:
    QEdgeAudioReproductor();
    ~QEdgeAudioReproductor();
    void Open();
    void Start();
    void Stop();
    void PlayAudio( AVFrame* audio_frame );

    static int GetSampleSize( AVSampleFormat fmt );

protected:
    virtual bool event( QEvent* ev ) override;

private:

    class QStartEvent : public QEvent
    {
    public:
        static const QEvent::Type s_start_event_type = (QEvent::Type)(QEvent::User + 1);
        QStartEvent() : QEvent(s_start_event_type) {}
    };

    std::unique_ptr<QAudioOutput> m_audio_output;
    QIODevice* m_audio_device;
    QEdgeBufferizedContainer m_audio_buffer;
    QAudioFormat m_format;

    bool m_audio_output_initialized;
};

#endif // QEDGEAUDIOREPRODUCTOR_H
