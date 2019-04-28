#ifndef QEDGEUTILS_H
#define QEDGEUTILS_H

#include <QImage>

#include <queue>
#include <mutex>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}


namespace utils {

template <class TypeName>
class QEdgeMultithreadQueue : protected std::queue<TypeName>
{
public:
    void Push( TypeName element )
    {
        std::lock_guard<std::mutex> lock( m_mtx );
        this->push( element );
        Q_UNUSED( m_mtx );
    }

    TypeName Pop()
    {
        std::lock_guard<std::mutex> lock( m_mtx );
        Q_UNUSED( m_mtx );

        if( this->empty() )
        {
            return NULL;
        }

        TypeName value = this->front();
        this->pop();
        return value;
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> lock( m_mtx );
        Q_UNUSED( m_mtx );

        return this->size();
    }

    bool IsEmpty()
    {
        std::lock_guard<std::mutex> lock( m_mtx );
        Q_UNUSED( m_mtx );

        return this->empty();
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock( m_mtx );
        this->clear();
        Q_UNUSED( m_mtx );
    }

    static const size_t s_max_size = 200;

private:
    std::mutex m_mtx;

};

QImage* AVFrameToQImage( const AVFrame *frame, const QSize &target_size );

void QEdgeSleep( int msecs );

int64_t MsecsToTimebaseUnits( AVRational time_base, int64_t msecs );
int64_t TimebaseUnitsToMsecs( AVRational time_base, int64_t timebase_units );

}

#endif //QEDGEUTILS_H
