#include <QImage>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

namespace utils {

QImage* AVFrameToQImage( const AVFrame *frame, const QSize &target_size );

}