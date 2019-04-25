#include <ui/QEdgeButton.h>

QEdgeButton::QEdgeButton( QWidget* parent ) : QPushButton( parent ), m_show_first_time( true )
{
    connect( this, SIGNAL( toggled( bool ) ), this, SLOT( OnCheckedChagned( bool ) ) );
}

void QEdgeButton::SetIconSize( const QSize& size )
{
    this->setIconSize( size );
    m_icon_size = size;
}

void QEdgeButton::SetDisabledIcon( const QIcon& icon )
{
    m_disabled_icon = icon;
}

void QEdgeButton::OnCheckedChagned( bool checked )
{
    if( isCheckable() )
    {
        setIcon( checked ? m_checked_hover_icon : m_hover_icon );
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
    m_checked_disabled_icon = icon;
}

bool QEdgeButton::event( QEvent *ev )
{
    if( ev->type() == QEvent::Show && m_show_first_time )
    {
        if( isEnabled() )
        {
            setIcon( isCheckable() && isChecked() ? m_checked_icon : m_normal_icon );
        }
        else
        {
            setIcon( isCheckable() && isChecked() ? m_checked_disabled_icon : m_disabled_icon );
        }
    }

    else if( ev->type() == QEvent::EnabledChange )
    {
        if( isEnabled() )
        {
            setIcon( isCheckable() && isChecked() ? m_checked_icon : m_normal_icon );
        }
        else
        {
            setIcon( isCheckable() && isChecked() ? m_checked_disabled_icon : m_disabled_icon );
        }
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

    return QPushButton::event( ev );
}
