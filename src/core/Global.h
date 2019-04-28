#ifndef GLOBAL_H
#define GLOBAL_H

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#define AV_SYNC_THRESHOLD 0.01
#define AV_NOSYNC_THRESHOLD 10.0
#define AV_NOSYNC_DELAY 0.03
#define AV_MAX_AUDIO_BUF_SIZE 30 * 1024 * 1024; //30 Mbs

#endif // GLOBAL_H
