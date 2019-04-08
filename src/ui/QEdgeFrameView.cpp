#include "ui_QEdgeFrameView.h"
#include <ui/QEdgeFrameView.h>
#include <QPainter>
#include <core/QEdgeUtils.h>

QEdgeFrameView::QEdgeFrameView( QWidget *parent ) :
    QWidget( parent ),
    ui( new Ui::QEdgeFrameView ),
    m_current_frame( nullptr ),
    m_image( nullptr )
{
    ui->setupUi( this );
}

QEdgeFrameView::~QEdgeFrameView()
{
    delete ui;
}

void QEdgeFrameView::OnNewFrame( AVFrame *frame )
{
    {
        std::lock_guard<std::mutex> m_frame_guard( m_frame_mtx );
        av_frame_free( &m_current_frame );
        m_current_frame = frame;
        Q_UNUSED( m_frame_guard );
    }
    QCoreApplication::removePostedEvents( this, QUpdatePreviewEvent::EventType );
    QCoreApplication::postEvent( this, new QUpdatePreviewEvent() );
}

void QEdgeFrameView::ResetImage( QImage *image )
{
    if( m_image )
    {
        delete m_image;
    }

    m_image = image;
}

void QEdgeFrameView::paintEvent( QPaintEvent *ev )
{
    if( m_current_frame )
    {
        QPainter painter( this );
        {
            std::lock_guard<std::mutex> frame_guard( m_frame_mtx );
            ResetImage( utils::AVFrameToQImage( m_current_frame, this->size() ) );
            Q_UNUSED( frame_guard );
        }
        painter.drawImage( QPoint( 0, 0 ), ( *m_image ) );
        painter.end();
    }

    else
    {
        QWidget::paintEvent( ev );
    }
}

bool QEdgeFrameView::event( QEvent *event )
{
    if( event->type() == QUpdatePreviewEvent::EventType )
    {
        update();
    }

    return QWidget::event( event );
}
