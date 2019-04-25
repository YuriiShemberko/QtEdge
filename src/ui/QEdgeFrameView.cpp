#include "ui_QEdgeFrameView.h"
#include <ui/QEdgeFrameView.h>
#include <QPainter>
#include <core/QEdgeUtils.h>

QEdgeFrameView::QEdgeFrameView( QWidget *parent ) :
    QWidget( parent ),
    m_frame_shown( false ),
    ui( new Ui::QEdgeFrameView ),
    m_current_frame( nullptr ),
    m_image( new QImage( ":img/resources/curtain.png" ) )
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
        std::lock_guard<std::mutex> frame_guard( m_frame_mtx );
        if( m_current_frame )
        {
            emit frameProcessed( m_current_frame );
        }
        m_current_frame = frame;
        m_frame_shown = false;

        Q_UNUSED( frame_guard );
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
    QPainter painter( this );
    {
        if( m_current_frame )
        {
            std::lock_guard<std::mutex> frame_guard( m_frame_mtx );
            ResetImage( utils::AVFrameToQImage( m_current_frame, this->size() ) );
            Q_UNUSED( frame_guard );
        }
    }

    int w_diff = this->width() - m_image->width();
    int h_diff = this->height() - m_image->height();

    if( w_diff > h_diff  )
    {
        painter.fillRect( QRectF( 0, 0, w_diff / 2, this->height() ), QColor( 192, 195, 193 ) );
        painter.drawImage( QPoint( w_diff / 2, 0 ), *m_image );
        painter.fillRect( QRectF( w_diff / 2 + m_image->width(), 0, w_diff / 2, this->height() ), QColor( 192, 195, 193 ) );
    }

    else if( h_diff > w_diff )
    {
        painter.fillRect( QRectF( 0, 0, this->width(), h_diff / 2 ), QColor( 192, 195, 193 ) );
        painter.drawImage( 0, h_diff / 2, *m_image );
        painter.fillRect( QRectF( 0, m_image->height() + h_diff / 2, this->width(), h_diff / 2 ), QColor( 192, 195, 193 ) );
    }

    else
    {
        painter.drawImage( QPoint( 0, 0 ), ( *m_image ) );
    }

    painter.end();

    if( !m_frame_shown )
    {
        m_frame_shown = true;
        emit frameShown();
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
