#include <core/QEdgeFrameProvider.h>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

QEdgeFrameProvider::QEdgeFrameProvider() :
    m_client( CNullFrameProviderClient::Instance() ),
    m_busy( false )
{

}

QEdgeFrameProvider::~QEdgeFrameProvider()
{
    DisconnectClient();
}

void QEdgeFrameProvider::DisconnectClient()
{
    m_client->Disconnected();
    m_client = CNullFrameProviderClient::Instance();
}

void QEdgeFrameProvider::BindClient( IFrameProviderClient *client )
{
    if( client )
    {
        m_client = client;
    }

    else
    {
        Q_ASSERT( 0 );
        qDebug() << "QEdgeFrameProvider::BindClient: invalid argument - client";
    }
}

bool QEdgeFrameProvider::Start( SFrameProviderConfig config )
{
    if( m_busy )
    {
        qDebug() << "QEdgeFrameProvider::Start: provider is busy. ";
        Q_ASSERT( 0 );
        return false;
    }

    if( m_client )
    {
        m_config = config;
        m_thread.reset( new std::thread( QEdgeFrameProvider::StartReadFramesInThread, this ) );
        m_client->OnStarted();
        m_busy = true;

        return true;
    }

    else
    {
        qDebug() << "QEdgeFrameProvider::Start: invalid client";
        Q_ASSERT( 0 );
        return false;
    }
}

void QEdgeFrameProvider::StartReadFramesInThread( void *ctx )
{
    QEdgeFrameProvider* host = static_cast<QEdgeFrameProvider*>( ctx );
    std::string target_file = host->m_config.file_name.toStdString();
    const char* filename = target_file.c_str();

    AVFormatContext* format_context = avformat_alloc_context();
    int error_code = avformat_open_input( &format_context, filename, 0, 0 );

    if( error_code < 0 )
    {
        QString err_text = "ffmpeg: unable to open input file: " + QString( filename ) + ". Error code: " + QString::number( error_code );
        host->OnFailed( err_text );
        return;
    }

    error_code = avformat_find_stream_info( format_context, 0 );

    if( error_code < 0 )
    {
        QString err_text = "ffmpeg: unable to find stream info, input file: " + QString( filename ) + ". Error code: " + QString::number( error_code );
        host->OnFailed( err_text );
        return;
    }

    uint video_stream;

    for( video_stream = 0; video_stream < format_context->nb_streams; ++video_stream )
    {
        if( format_context->streams[video_stream]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO )
        {
            break;
        }
    }

    if( video_stream == format_context->nb_streams )
    {
        QString err_text = "ffmpeg: unable to find video stream";
        host->OnFailed( err_text );
        return;
    }

    AVCodecParameters* codec_params = format_context->streams[video_stream]->codecpar;
    AVCodec* codec = avcodec_find_decoder( codec_params->codec_id );
    AVCodecContext* codec_context = avcodec_alloc_context3( codec );
    avcodec_parameters_to_context( codec_context, codec_params );
    error_code = avcodec_open2( codec_context, codec, 0 );

    if( error_code < 0 )
    {
        QString err_text = "ffmpeg: unable to open codec";
        host->OnFailed( err_text );
        return;
    }

    AVPacket packet;
    SwsContext* img_convert_context = nullptr;
    AVFrame* frame = av_frame_alloc();

    while( av_read_frame( format_context, &packet) >= 0 )
    {
        if( packet.stream_index == (int)video_stream )
        {
            int result = -1;
            result = avcodec_send_packet( codec_context, &packet );

            if( result < 0 )
            {
                qDebug() << "avcodec_send_packet error: ret = " << result;
                break;
            }

            while( result >= 0 )
            {
                result = avcodec_receive_frame( codec_context, frame );

                if( result == AVERROR( EAGAIN ) || result == AVERROR_EOF )
                {
                    break;
                }

                else if( result < 0 )
                {
                    QString err_text = "ffmpeg: error while decoding";
                    host->OnFailed( err_text );
                    return;
                }

                QSize target_size = host->m_client->TargetSize();
                int width = target_size.width();
                int height = target_size.height();

                AVFrame* converted = av_frame_alloc();
                int buf_size = av_image_get_buffer_size( AV_PIX_FMT_RGB32, width, height, 32 );
                uint8_t* buffer = (uint8_t*)av_malloc( buf_size );
                av_image_fill_arrays( converted->data, converted->linesize, buffer, AV_PIX_FMT_RGB32, width, height, 32 );

                img_convert_context = sws_getContext(   codec_context->width,
                                                        codec_context->height,
                                                        codec_context->pix_fmt,
                                                        width, height, AV_PIX_FMT_RGB32, 0, 0, 0, 0 );

                if( img_convert_context == NULL )
                {
                    QString err_text = "ffmpeg: cannot initialize image convert context";
                    host->OnFailed( err_text );
                    return;
                }

                sws_scale( img_convert_context, frame->data, frame->linesize, 0, codec_context->height, converted->data, converted->linesize );
                QImage* img = new QImage( width, height, QImage::Format_RGB32 );

                if( img->isNull() )
                {
                    Q_ASSERT(0);
                }

                memcpy( img->scanLine(0), converted->data[0], converted->linesize[0] * height );

                host->m_client->OnNewFrame( *img );
                av_frame_free( &converted );
                av_free( buffer );
                sws_freeContext( img_convert_context );

                delete img;
            }

            av_packet_unref( &packet );
        }

        //std::this_thread::sleep_for( std::chrono::milliseconds(5) );
    }

    av_frame_free( &frame );
    avcodec_free_context( &codec_context );
    avformat_close_input( &format_context );

    host->OnFinished();
}

void QEdgeFrameProvider::OnFailed( QString error_text )
{
    m_client->OnFailed();
    qDebug() << "QEdgeFrameProvider failed: " << error_text;
    m_busy = false;
    DisconnectClient();
    Q_ASSERT( 0 );
}

void QEdgeFrameProvider::OnFinished()
{
    m_thread.release();
    m_busy = false;
    m_client->OnFinished();
}
