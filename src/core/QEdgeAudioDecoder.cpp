#include <core/QEdgeAudioDecoder.h>

QEdgeAudioDecoder::QEdgeAudioDecoder() :
    m_subscriber( CNullDecoderSubscriber::Instance() ),
    m_running( false )
{

}

void QEdgeAudioDecoder::Init( AVStream *stream, IDecoderSubscriber* subscriber )
{
    if( !m_running )
    {
        Q_ASSERT( stream );
        m_stream = stream;
        m_subscriber.reset( subscriber );

        AVCodecParameters* codec_params = m_stream->codecpar;
        AVCodec* codec = avcodec_find_decoder( codec_params->codec_id );

        m_codec_context.reset( avcodec_alloc_context3( codec ) );
        avcodec_parameters_to_context( m_codec_context.get(), codec_params );

        int error_code = avcodec_open2( m_codec_context.get(), codec, 0 );

        if( error_code < 0 || !m_codec_context.get() )
        {
            m_subscriber->OnDecoderFailed( this, QString( "Unable to start audio decoder, error code: " ).arg( error_code ) );
            return;
        }

        m_running = true;
        m_decode_thread.reset( new std::thread( QEdgeAudioDecoder::DecodeThread, this ) );
    }
}

void QEdgeAudioDecoder::StopDecode()
{
    m_running = false;

    if( m_decode_thread.get() )
    {
        m_decode_thread->join();
    }

    m_decode_thread.release();
    m_codec_context.release();
    FreeQueue();
}

void QEdgeAudioDecoder::FreeQueue()
{
    std::lock_guard<std::mutex> queue_guard( m_queue_mutex );

    while( !m_packet_queue.empty() )
    {
        AVPacket* packet = m_packet_queue.front();

        if( packet )
        {
            av_packet_unref( packet );
        }

        m_packet_queue.pop();
    }

    Q_UNUSED( queue_guard );
}

void QEdgeAudioDecoder::OnNewAudioPacket( AVPacket *packet )
{
    if( !m_running )
    {
        Q_ASSERT( 0 );
        m_subscriber->OnDecoderFailed( this, QString( "Audio decoder not started") );
        av_packet_unref( packet );
        return;
    }

    else
    {
        PushPacket( packet );
    }
}

void QEdgeAudioDecoder::PushPacket( AVPacket* packet )
{
    std::lock_guard<std::mutex> queue_guard( m_queue_mutex );
    m_packet_queue.push( packet );

    Q_UNUSED( queue_guard ); //to avoid compiler warning
}

void QEdgeAudioDecoder::OnNewFrame( AVFrame *frame )
{
    m_subscriber->OnNewFrame( this, frame );
}

void QEdgeAudioDecoder::OnFailed( QString err_text )
{
    m_subscriber->OnDecoderFailed( this, err_text );
    Q_ASSERT( 0 );
}

AVPacket *QEdgeAudioDecoder::PopPacket()
{
    std::lock_guard<std::mutex> queue_guard( m_queue_mutex );

    if( m_packet_queue.empty() )
    {
        return nullptr;
    }

    else
    {
        AVPacket* packet = m_packet_queue.front();
        m_packet_queue.pop();
        return packet;
    }

    Q_UNUSED( queue_guard ); //avoiding compiler warning
}

void QEdgeAudioDecoder::DecodeThread( void *ctx )
{
    QEdgeAudioDecoder* host = static_cast<QEdgeAudioDecoder*>( ctx );

    while ( host->m_running )
    {
        AVPacket* packet = host->PopPacket();

        if( !packet )
        {
            continue; //wait for next packet
        }

        int result = -1;
        result = avcodec_send_packet( host->m_codec_context.get(), packet );

        if( result < 0 )
        {
            host->OnFailed( QString( "Audio Decoder: avcodec_send_packet error: %1" ).arg( result ) );
            return;
        }

        while( result >= 0 )
        {
            AVFrame* frame = av_frame_alloc();
            result = avcodec_receive_frame( host->m_codec_context.get(), frame );

            if( result == AVERROR( EAGAIN ) || result == AVERROR_EOF )
            {
                break;
            }

            else if( result < 0 )
            {
                QString err_text = "Audio Decoder ffmpeg: error while decoding";
                host->OnFailed( err_text );
                return;
            }

            host->OnNewFrame( frame );
        }

        av_packet_unref( packet );
    }
}
