#include <core/QEdgeAudioReproductor.h>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QDebug>
#include <thread>
#include <QThread>
#include <QCoreApplication>

QEdgeBufferizedContainer::QEdgeBufferizedContainer() : QIODevice(), m_pos(0), m_eof(false) {}
QEdgeBufferizedContainer::~QEdgeBufferizedContainer() {}

void QEdgeBufferizedContainer::Start()
{
    QIODevice::open( QIODevice::ReadWrite );
}

void QEdgeBufferizedContainer::Stop()
{
    m_eof = false;
    m_buffer.clear();
    close();
    m_pos = 0;
}

qint64 QEdgeBufferizedContainer::readData( char *data, qint64 len )
{
    qint64 total = 0;

    if( m_eof )
    {
        emit eof();
        return 0;
    }

    if( m_pos + len >= m_buffer.size() ) //end
    {
        total = m_buffer.size() - m_pos;
        memcpy( data, m_buffer.constData() + m_pos, total );
        m_eof = true;
        emit bufferRead( 0 );
        return total;
    }

    if (!m_buffer.isEmpty())
    {
        while (len - total > 0)
        {
            const qint64 chunk = qMin( ( m_buffer.size() - m_pos ), len - total );
            memcpy( data + total, m_buffer.constData() + m_pos, chunk );
            m_pos = ( m_pos + chunk ) % m_buffer.size();
            total += chunk;
        }
    }

    emit bufferRead( m_buffer.size() - m_pos );
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
    return m_buffer.size() - m_pos;
}

QEdgeAudioReproductor::QEdgeAudioReproductor() :
    m_player( CNullPlayer::Instance() ),
    m_audio_output_initialized( false )
{
    connect( &m_audio_buffer, SIGNAL( eof() ), this, SLOT( OnBufferEof() ) );
}

QEdgeAudioReproductor::~QEdgeAudioReproductor()
{
    m_audio_output.release();
    m_player.release();
}

void QEdgeAudioReproductor::Init( IPlayer *player )
{
    m_player.reset( player );
}

void QEdgeAudioReproductor::OnVideo( AVFrame *frame )
{
    Q_UNUSED( frame );
}

void QEdgeAudioReproductor::OnFailed( QString err_text )
{
    Q_UNUSED( err_text );
}

void QEdgeAudioReproductor::OnFinished()
{
}

void QEdgeAudioReproductor::PlayerStarted()
{
    Start();
}

void QEdgeAudioReproductor::PlayerStopped()
{
    Stop();
}

void QEdgeAudioReproductor::DurationSpecified( int64_t msecs )
{
    Q_UNUSED( msecs );
}

void QEdgeAudioReproductor::CurrentTimestampChanged( int64_t msecs )
{
    Q_UNUSED( msecs );
}

void QEdgeAudioReproductor::Start()
{
    m_audio_buffer.Start();
}

void QEdgeAudioReproductor::Stop()
{
    if( m_audio_output.get() )
    {
        m_audio_output->stop();
    }

    m_audio_output.release();

    m_audio_buffer.Stop();

    m_audio_output_initialized = false;
}

void QEdgeAudioReproductor::OnSetVolume( int volume )
{
    if( m_audio_output.get() )
    {
        m_audio_output->setVolume( volume / 100.0 );
    }
}

void QEdgeAudioReproductor::OnAudio( AVFrame *audio_frame )
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

        QCoreApplication::postEvent( this, new QAudioOutputEvent( true ) );

        m_audio_output_initialized = true;
    }

    m_player->AudioProcessed( audio_frame );
}

bool QEdgeAudioReproductor::event( QEvent* ev )
{
    if( ev->type() == QAudioOutputEvent::s_audio_output_event_type )
    {
        QAudioOutputEvent* audio_ev = static_cast<QAudioOutputEvent*>( ev );

        if( audio_ev->IsStart() )
        {
            m_audio_output.release();
            m_audio_output.reset( new QAudioOutput( QAudioDeviceInfo::defaultOutputDevice(), m_format, this ) );
            m_audio_output->start( &m_audio_buffer );
            m_audio_output->setVolume(1);
            m_audio_output->setNotifyInterval( 100 );
            connect( m_audio_output.get(), SIGNAL(notify()), this, SLOT(OnBufferRead()));
        }

        else
        {
            Stop();
        }

        return true;
    }

    return QObject::event( ev );
}

void QEdgeAudioReproductor::OnBufferEof()
{
    QCoreApplication::postEvent( this, new QAudioOutputEvent( false ) );
}

void QEdgeAudioReproductor::OnBufferRead()
{
    m_player->AudioPresented( m_audio_buffer.bytesAvailable() );
}

