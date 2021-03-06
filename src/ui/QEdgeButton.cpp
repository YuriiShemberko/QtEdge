#include <ui/QEdgeButton.h>

QEdgeButton::QEdgeButton( QWidget* parent ) : QPushButton( parent ), m_show_first_time( true )
{
    connect( this, SIGNAL( toggled( bool ) ), this, SLOT( OnCheckedChanged( bool ) ) );
}

void QEdgeButton::SetIconSize( const QSize& size )
{
    this->setIconSize( size );
    m_icon_size = size;
}

void QEdgeButton::SetDisabledIcon( const QIcon& icon )
{
    m_normal_icon.addPixmap( icon.pixmap(width(), height() ), QIcon::Disabled );
}

void QEdgeButton::OnCheckedChanged( bool checked )
{
    if( isCheckable() )
    {
        QPoint mouse_global = QCursor::pos();
        QPoint mouse_pos = parentWidget()->mapFromGlobal( mouse_global );

        bool over_btn = mouse_pos.x() >= geometry().left() && mouse_pos.x() <= geometry().right()
                && mouse_pos.y() >= geometry().top() && mouse_pos.y() <= geometry().bottom();

        setIcon( checked ? ( over_btn ? m_checked_hover_icon : m_checked_icon )
                         : ( over_btn ? m_hover_icon : m_normal_icon ) );
    }
}

void QEdgeButton::SetNormalIcon( const QIcon& icon )
{
    m_normal_icon = icon;
}

void QEdgeButton::SetHoverIcon( const QIcon &icon )
{
    m_hover_icon = icon;
}

void QEdgeButton::SetCheckedIcon( const QIcon& icon )
{
    m_checked_icon = icon;
}

void QEdgeButton::SetCheckedHoverIcon( const QIcon& icon )
{
    m_checked_hover_icon = icon;
}

void QEdgeButton::SetCheckedDisabledIcon( const QIcon &icon )
{
    m_checked_icon.addPixmap( icon.pixmap(width(), height() ), QIcon::Disabled );
}

void QEdgeButton::SetClickedIcon( const QIcon &icon )
{
    m_clicked_icon = icon;
}

void QEdgeButton::SetClickedCheckedIcon( const QIcon &icon )
{
    m_clicked_checked_icon = icon;
}

bool QEdgeButton::event( QEvent *ev )
{
    if( ( ev->type() == QEvent::Show && m_show_first_time ) || ev->type() == QEvent::EnabledChange )
    {
        setIcon( isCheckable() && isChecked() ? m_checked_icon : m_normal_icon );
    }

    else if( ev->type() == QEvent::Enter )
    {
        if( isEnabled() )
        {
            setIcon( isCheckable() && isChecked() ? m_checked_hover_icon : m_hover_icon );
        }
    }

    else if( ev->type() == QEvent::Leave )
    {
        if( isEnabled() )
        {
            setIcon( isCheckable() && isChecked() ? m_checked_icon : m_normal_icon );
        }
    }

    else if( ev->type() == QEvent::MouseButtonPress )
    {
        if( isEnabled() )
        {
            setIcon( isCheckable() && isChecked() ? m_clicked_checked_icon : m_clicked_icon );
        }
    }

    else if( ev->type() == QEvent::MouseButtonRelease )
    {
        if( isEnabled() )
        {
            setIcon( isCheckable() && isChecked() ? m_checked_icon : m_normal_icon );
        }
    }

    return QPushButton::event( ev );
}
