#include "ui_QEdgeFrameView.h"
#include <ui/QEdgeFrameView.h>
#include <QPainter>

QEdgeFrameView::QEdgeFrameView( QWidget *parent ) :
    QWidget( parent ),
    ui( new Ui::QEdgeFrameView )
{
    ui->setupUi( this );
}

QEdgeFrameView::~QEdgeFrameView()
{
    delete ui;
}

void QEdgeFrameView::UpdateImage( const QImage &image )
{
    m_image = image;
    QCoreApplication::postEvent( this, new QUpdatePreviewEvent() );
}

void QEdgeFrameView::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    painter.drawImage( QPoint( 0, 0 ), m_image );
    painter.end();
}

bool QEdgeFrameView::event( QEvent *event )
{
    if( event->type() == QUpdatePreviewEvent::EventType )
    {
        update();
    }

    return QWidget::event( event );
}
