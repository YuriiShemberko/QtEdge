#include <ui/QDropDownVolumeSlider.h>
#include "ui_QDropDownVolumeSlider.h"
#include <QDebug>

QDropDownVolumeSlider::QDropDownVolumeSlider( QWidget* dock_widget, int min, int max ) :
    QWidget( dock_widget->topLevelWidget() ),
    ui( new Ui::QDropDownVolumeSlider ),
    m_dock_widget( dock_widget )
{
    ui->setupUi(this);

    ui->slider->setMinimum( min );
    ui->slider->setMaximum( max );
    ui->slider->setValue( 0 );
    ui->label->setText( QString::number(0) );

    connect( ui->slider, SIGNAL( sliderMoved( int ) ), this, SLOT( OnValueChanged(int) ) );

    dock_widget->installEventFilter( this );

    this->hide();
}

void QDropDownVolumeSlider::SetValue( int value )
{
    ui->slider->setValue( value );
    ui->label->setText( QString::number( value ) );
}

int QDropDownVolumeSlider::GetValue()
{
    return ui->slider->value();
}

QDropDownVolumeSlider::~QDropDownVolumeSlider()
{
    delete ui;
}

void QDropDownVolumeSlider::OnValueChanged( int value )
{
    ui->label->setText( QString::number( value ) );
    emit valueChanged( value );
}

bool QDropDownVolumeSlider::eventFilter( QObject* watched, QEvent* event )
{
    if( watched == m_dock_widget )
    {
        if( event->type() == QEvent::Enter )
        {
            QPoint show_position = m_dock_widget->mapTo( topLevelWidget(), QPoint( 0, 0 ) );
            setGeometry( show_position.x(), show_position.y() - this->height(), this->minimumWidth(), this->minimumHeight() );
            show();
        }

        else if( event->type() == QEvent::Leave )
        {
            OnParentLeave();
        }
    }

    return QWidget::eventFilter( watched, event );
}

bool QDropDownVolumeSlider::event( QEvent *event )
{
    if( event->type() == QEvent::Leave )
    {
        this->hide();
    }

    return QWidget::event( event );
}

void QDropDownVolumeSlider::OnParentLeave()
{
    QPoint leave_pos = m_dock_widget->mapFromGlobal(QCursor::pos());

    QPoint top_left = m_dock_widget->rect().topLeft();
    QPoint top_right = m_dock_widget->rect().topRight();

    //mouse leave at the top of parent widget
    bool to_slider = leave_pos.x() >= top_left.x()
                && leave_pos.x() <= top_right.x()
                && leave_pos.y() <= top_left.y();

    if( !to_slider )
    {
        hide();
    }
}
