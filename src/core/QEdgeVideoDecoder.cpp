#include <core/QEdgeVideoDecoder.h>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

QEdgeVideoDecoder::QEdgeVideoDecoder() {}

void QEdgeVideoDecoder::Decode( AVPacket *packet )
{
    int result = -1;
    result = avcodec_send_packet( m_codec_context.get(), packet );

    if( result < 0 )
    {
        OnFailed( QString( "Video Decoder: avcodec_send_packet error: %1" ).arg( result ) );
        return;
    }

    while( result >= 0 )
    {
        AVFrame* frame = av_frame_alloc();
        result = avcodec_receive_frame( m_codec_context.get(), frame );

        if( result == AVERROR( EAGAIN ) || result == AVERROR_EOF )
        {
            av_frame_unref( frame );
            av_frame_free( &frame );
            break;
        }

        else if( result < 0 )
        {
            av_frame_unref( frame );
            av_frame_free( &frame );
            QString err_text = "Video Decoder: ffmpeg: error while decoding";
            OnFailed( err_text );
            return;
        }

        OnNewFrame( frame );
    }
}
