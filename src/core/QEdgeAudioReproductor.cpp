#include <core/QEdgeAudioReproductor.h>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QDebug>
#include <thread>
#include <QThread>
#include <QCoreApplication>
#include <core/QEdgeUtils.h>

QEdgeBufferizedContainer::QEdgeBufferizedContainer() : QIODevice(), m_buf_size(0), m_eof(false), m_buffer(nullptr) {}
QEdgeBufferizedContainer::~QEdgeBufferizedContainer() {}

void QEdgeBufferizedContainer::Start()
{
    QIODevice::open( QIODevice::ReadWrite );
}

void QEdgeBufferizedContainer::Stop()
{
    std::lock_guard<std::mutex> lock( m_rw_mtx );
    Q_UNUSED( lock );

    m_eof = false;
    FreeBuffer();
    close();
    m_buf_size = 0;
}

void QEdgeBufferizedContainer::FreeBuffer()
{
    if( m_buffer )
    {
        free( m_buffer );
        m_buffer = nullptr;
    }
}

qint64 QEdgeBufferizedContainer::readData( char *data, qint64 len )
{
    std::lock_guard<std::mutex> lock( m_rw_mtx );
    Q_UNUSED( lock );

    qint64 total = 0;

    if( m_eof )
    {
        emit eof();
        return 0;
    }

    if( len >= m_buf_size ) //end
    {
        total = m_buf_size;
        memcpy( data, m_buffer, size_t( total ) );
        m_eof = true;
    }

    else if( m_buf_size > 0 )
    {
        total = len;
        memcpy( data, m_buffer, size_t( len ) );
        m_buf_size -= len;
        uint8_t* buffer = (uint8_t*)malloc(m_buf_size);
        memcpy( buffer, m_buffer + len, m_buf_size );

        FreeBuffer();
        m_buffer = buffer;
    }

    return total;
}

qint64 QEdgeBufferizedContainer::writeData( const char *data, qint64 len )
{
    std::lock_guard<std::mutex> lock( m_rw_mtx );
    Q_UNUSED( lock );

    uint8_t* buffer = (uint8_t*)malloc( m_buf_size + len );
    memcpy( buffer, m_buffer, size_t( m_buf_size ) );
    memcpy( buffer + m_buf_size, data, size_t( len ) );

    FreeBuffer();

    m_buffer = buffer;

    m_buf_size += len;

    return len;
}

qint64 QEdgeBufferizedContainer::bytesAvailable() const
{
    return m_buf_size;
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
            m_audio_output->setNotifyInterval( 100 );
            m_audio_output->start( &m_audio_buffer );
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

