#include <core/QEdgeMediaController.h>
#include <QDebug>

QEdgeMediaController::QEdgeMediaController() : m_subscriber( CNullMediaControllerSubscriber::Instance() ) {}

void QEdgeMediaController::ConnectToController( IMediaController::IMediaControllerSubscriber *subscriber )
{
    m_subscriber = subscriber;
}

void QEdgeMediaController::Start( QString file_name )
{
    m_demuxer.Init( this );

    if( !m_demuxer.Start( file_name ) )
    {
        OnDemuxerFailed( "cannot start demuxer" );
    }
}

void QEdgeMediaController::Stop()
{
    m_demuxer.Interrupt();
    m_audio_decoder.StopDecode();
    m_video_decoder.StopDecode();
}

void QEdgeMediaController::Seek( int msec )
{
    Q_UNUSED( msec );
    //TODO
}

void QEdgeMediaController::OnDecoderFinished( IDecoder *sender )
{
    Q_UNUSED( sender );
    //TODO
}

void QEdgeMediaController::OnDecoderFailed( IDecoder *sender, QString err_text )
{
    if( sender == &m_video_decoder )
    {
        qDebug() << "[QEdgeMediaController] Video decoder failed: " << err_text;
    }
    else if( sender == &m_audio_decoder )
    {
        qDebug() << "[QEdgeMediaController] Audio decoder failed: " << err_text;
    }
    else
    {
        Q_ASSERT( 0 );
    }
}

void QEdgeMediaController::OnNewFrame( IDecoder *sender, AVFrame *frame )
{
    if( sender == &m_video_decoder )
    {
        m_subscriber->OnNewVideoFrame( frame );
        //new video
    }
    else if( sender == &m_audio_decoder )
    {
        m_subscriber->OnNewAudioFrame( frame );
        //new audio
    }
}

void QEdgeMediaController::OnDemuxerFailed( QString err_text )
{
    qDebug() << "[QEdgeMediaController] Demuxer failed: " << err_text;
}

void QEdgeMediaController::OnDemuxerFinished()
{

}

void QEdgeMediaController::InitStream( AVStream *video_stream, AVStream *audio_stream )
{
    m_audio_decoder.Init( audio_stream, this );
    m_video_decoder.Init( video_stream, this );
}

void QEdgeMediaController::OnAudioPacket( AVPacket *packet )
{
    m_audio_decoder.OnNewAudioPacket( packet );
}

void QEdgeMediaController::OnVideoPacket( AVPacket *packet )
{
    m_video_decoder.OnNewVideoPacket( packet );
}
