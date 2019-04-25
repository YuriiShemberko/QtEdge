#ifndef QEdgeButton_H
#define QEdgeButton_H

#include <QPushButton>
#include <QEnterEvent>

class QEdgeButton : public QPushButton
{
    Q_OBJECT
public:
    QEdgeButton( QWidget* parent = nullptr );

    void SetIconSize( const QSize& size );
    void SetNormalIcon( const QIcon& icon );
    void SetDisabledIcon( const QIcon& icon );
    void SetHoverIcon( const QIcon& icon );
    void SetCheckedIcon( const QIcon& icon );
    void SetCheckedHoverIcon( const QIcon& icon );
    void SetCheckedDisabledIcon( const QIcon& icon );
    void SetClickedIcon( const QIcon& icon );
    void SetClickedCheckedIcon( const QIcon& icon );

protected:
    virtual bool event( QEvent* ev );

private slots:
    void OnCheckedChagned( bool checked );

private:
    QSize m_icon_size;
    QIcon m_normal_icon;
    QIcon m_hover_icon;
    QIcon m_checked_icon;
    QIcon m_checked_hover_icon;
    QIcon m_clicked_icon;
    QIcon m_clicked_checked_icon;

    bool m_show_first_time;
};

#endif // QEdgeButton_H
