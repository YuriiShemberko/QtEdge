#include <core/QEdgeUtils.h>
#include <QDebug>

namespace utils {

QImage* AVFrameToQImage( const AVFrame *frame, const QSize &target_size )
{
    int width = target_size.width() - target_size.width() % 4;
    int height = target_size.height();

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

    av_frame_free( &converted );
    av_free( buffer );
    sws_freeContext( img_convert_context );

    return img;
}


}
