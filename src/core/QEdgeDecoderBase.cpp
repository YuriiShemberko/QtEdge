#include <core/QEdgeDecoderBase.h>

QEdgeDecoderBase::QEdgeDecoderBase() :
    QObject(nullptr),
    m_subscriber( CNullDecoderSubscriber::Instance() ),
    m_codec_context( nullptr ),
    m_running( false ),
    m_demuxer_finished( false )
{

}

void QEdgeDecoderBase::Init( AVStream *stream, IDecoderSubscriber* subscriber )
{
    if( !m_running )
    {
        Q_ASSERT( stream );
        m_stream = stream;

        if( m_subscriber.get() )
        {
            m_subscriber.release();
        }

        m_subscriber.reset( subscriber );

        AVCodecParameters* codec_params = m_stream->codecpar;
        AVCodec* codec = avcodec_find_decoder( codec_params->codec_id );

        if( m_codec_context.get() )
        {
            FreeCodecContext();
        }

        m_codec_context.reset( avcodec_alloc_context3( codec ) );
        avcodec_parameters_to_context( m_codec_context.get(), codec_params );

        int error_code = avcodec_open2( m_codec_context.get(), codec, 0 );

        if( error_code < 0 || !m_codec_context.get() )
        {
            m_subscriber->OnDecoderFailed( this, QString( "Unable to start decoder, error code: " ).arg( error_code ) );
            return;
        }

        m_demuxer_finished = false;
        m_running = true;

        if( m_decode_thread.get() )
        {
            m_decode_thread.release();
        }

        m_decode_thread.reset( new std::thread( QEdgeDecoderBase::DecodeThread, this ) );
    }
}

void QEdgeDecoderBase::StopDecode( bool interrupt )
{
    if( interrupt )
    {
        m_running = false;

        if( m_decode_thread.get() )
        {
            m_decode_thread->join();
        }

        m_decode_thread.release();
        FreeCodecContext();
        FreeQueue();
    }

    else
    {
        m_demuxer_finished = true;
    }
}

void QEdgeDecoderBase::FreeQueue()
{
    while( !m_packet_queue.IsEmpty() )
    {
        AVPacket* packet = m_packet_queue.Pop();

        if( packet )
        {
            av_packet_unref( packet );
            av_packet_free( &packet );
        }
    }
}

void QEdgeDecoderBase::OnNewPacket( AVPacket *packet )
{
    if( !m_running )
    {
        Q_ASSERT( 0 );
        m_subscriber->OnDecoderFailed( this, QString( "Decoder not started") );
        av_packet_unref( packet );
        av_packet_free( &packet );
        return;
    }

    else
    {
        PushPacket( packet );
    }
}

void QEdgeDecoderBase::PushPacket( AVPacket* packet )
{
    //wait for queue's free space
    while( m_packet_queue.Size() >= QEdgePacketQueue::s_max_size )
    {
        utils::QEdgeSleep( 10 );
    }

    m_packet_queue.Push( packet );
}

void QEdgeDecoderBase::OnNewFrame( AVFrame *frame )
{
    m_subscriber->OnNewFrame( this, frame );
}

void QEdgeDecoderBase::OnFailed( QString err_text )
{
    m_subscriber->OnDecoderFailed( this, err_text );
    Q_ASSERT( 0 );
}

void QEdgeDecoderBase::OnThreadFinished()
{
    m_subscriber->OnDecoderFinished( this );
    QCoreApplication::postEvent( this, new QCleanupEvent() );
}

bool QEdgeDecoderBase::event( QEvent *ev )
{
    if( ev->type() == QCleanupEvent::s_cleanup_event_type )
    {
        StopDecode( true );
        return true;
    }

    return QObject::event( ev );
}

void QEdgeDecoderBase::FreeCodecContext()
{
    AVCodecContext* ctx = m_codec_context.get();
    avcodec_close( ctx );
    avcodec_free_context( &ctx );
    m_codec_context.release();
}

AVPacket *QEdgeDecoderBase::PopPacket()
{
    if( m_packet_queue.IsEmpty() )
    {
        return nullptr;
    }

    return m_packet_queue.Pop();
}

void QEdgeDecoderBase::DecodeThread( void *ctx )
{
    QEdgeDecoderBase* host = static_cast<QEdgeDecoderBase*>( ctx );

    while ( host->m_running )
    {
        AVPacket* packet = host->PopPacket();

        if( !packet )
        {
            utils::QEdgeSleep( 20 );
            continue; //wait for next packet
        }

        host->Decode( packet );

        av_packet_unref( packet );
        av_packet_free( &packet );

        if( host->IsQueueEmpty() && host->m_demuxer_finished )
        {
            host->OnThreadFinished();
            return;
        }
    }
}
