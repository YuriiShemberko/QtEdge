#include <core/QEdgeAudioReproductor.h>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QDebug>
#include <thread>
#include <QThread>
#include <QCoreApplication>

QEdgeBufferizedContainer::QEdgeBufferizedContainer() : QIODevice(), m_pos(0) {}
QEdgeBufferizedContainer::~QEdgeBufferizedContainer() {}

void QEdgeBufferizedContainer::Start()
{
    QIODevice::open( QIODevice::ReadWrite );
}

void QEdgeBufferizedContainer::Stop()
{
    close();
    m_pos = 0;
}

qint64 QEdgeBufferizedContainer::readData( char *data, qint64 len )
{
    qint64 total = 0;
    if (!m_buffer.isEmpty())
    {
        while (len - total > 0)
        {
            const qint64 chunk = qMin( (m_buffer.size() - m_pos), len - total );
            memcpy( data + total, m_buffer.constData() + m_pos, chunk );
            m_pos = ( m_pos + chunk ) % m_buffer.size();
            total += chunk;
        }
    }
    return total;
}

qint64 QEdgeBufferizedContainer::writeData( const char *data, qint64 len )
{
    int pos = m_buffer.size();

    m_buffer.resize( m_buffer.size() + len );

    memcpy( m_buffer.data() + pos, data, len );

    return len;
}

qint64 QEdgeBufferizedContainer::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}

QEdgeAudioReproductor::QEdgeAudioReproductor() :
    m_audio_output_initialized( false )
{
}

QEdgeAudioReproductor::~QEdgeAudioReproductor()
{
    m_audio_output->stop();
    delete m_audio_output;
}

void QEdgeAudioReproductor::Start()
{
    m_audio_buffer.Start();
}

void QEdgeAudioReproductor::PlayAudio( AVFrame *audio_frame )
{
    int sample_size_bytes = av_get_bytes_per_sample( (AVSampleFormat)audio_frame->format );
    m_audio_buffer.writeData( (char*)audio_frame->data[0], sample_size_bytes * audio_frame->nb_samples );

    if( !m_audio_output_initialized )
    {
        m_format.setSampleRate( audio_frame->sample_rate / audio_frame->channels );
        m_format.setChannelCount( audio_frame->channels );
        m_format.setSampleSize( ( sample_size_bytes * 8 ) );
        m_format.setByteOrder( QAudioFormat::LittleEndian );
        m_format.setSampleType( QAudioFormat::Float );
        m_format.setCodec("audio/pcm");

        QCoreApplication::postEvent( this, new QStartEvent() );

        m_audio_output_initialized = true;
    }
}

bool QEdgeAudioReproductor::event( QEvent* ev )
{
    if( ev->type() == QStartEvent::s_start_event_type )
    {
        m_audio_output = new QAudioOutput( QAudioDeviceInfo::defaultOutputDevice(), m_format, this );
        m_audio_output->start( &m_audio_buffer );
        m_audio_output->setVolume(1);
        qDebug() << m_audio_output->error();
        qDebug() << m_audio_output->state();
        return true;
    }

    return QObject::event( ev );
}

