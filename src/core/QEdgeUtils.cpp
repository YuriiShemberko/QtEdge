#include <core/QEdgeUtils.h>
#include <QDebug>
#include <thread>

namespace utils {

QImage* AVFrameToQImage( const AVFrame *frame, const QSize &target_size )
{
    int width;
    int height;

    double width_coef = (double)target_size.width() / (double)frame->width;
    double height_coef = (double)target_size.height() / (double)frame->height;
    double min_coef = std::min( width_coef, height_coef );

    width = frame->width * min_coef;
    height = frame->height * min_coef;

    width -= width % 4;

    AVFrame* converted = av_frame_alloc();
    int buf_size = av_image_get_buffer_size( AV_PIX_FMT_RGB32, width, height, 4 );
    uint8_t* buffer = (uint8_t*)av_malloc( buf_size );
    int output_slice = av_image_fill_arrays( converted->data, converted->linesize, buffer, AV_PIX_FMT_RGB32, width, height, 4 );

    converted->width = width;
    converted->height = height;
    converted->format = AV_PIX_FMT_RGB32;

    SwsContext* img_convert_context = sws_getContext(   frame->width,
                                                        frame->height,
                                                        (AVPixelFormat)frame->format,
                                                        width, height, AV_PIX_FMT_RGB32, 0, 0, 0, 0 );

    if( img_convert_context == NULL )
    {
        Q_ASSERT( 0 );
        return new QImage();
    }

    int res = sws_scale( img_convert_context, frame->data, frame->linesize, 0, frame->height, converted->data, converted->linesize );
    QImage* img = new QImage( width, height, QImage::Format_RGB32 );

    Q_ASSERT( res > 0 );

    memcpy( img->scanLine(0), converted->data[0], output_slice );

    av_frame_unref( converted );
    av_frame_free( &converted );
    av_free( buffer );

    sws_freeContext( img_convert_context );

    return img;
}

void QEdgeSleep( int msec )
{
    std::this_thread::sleep_for( std::chrono::milliseconds( msec) );
}

int64_t MsecsToTimebaseUnits( AVRational time_base, int64_t msecs )
{
    int64_t frame_idx = av_rescale( msecs, time_base.den, time_base.num );
    return frame_idx / 1000;
}

int64_t TimebaseUnitsToMsecs( AVRational time_base, int64_t timebase_units )
{
    if( timebase_units == AV_NOPTS_VALUE )
    {
        return AV_NOPTS_VALUE;
    }

    double time_base_d =  av_q2d( time_base );
    double duration = (double)timebase_units * time_base_d * 1000.0;

    return duration + 0.5;
}

}
