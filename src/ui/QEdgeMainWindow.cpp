#include "ui_QEdgeMainWindow.h"
#include <ui/QEdgeMainWindow.h>

#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>

QEdgeMainWindow::QEdgeMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QEdgeMainWindow),
    m_started( false ),
    m_player( CNullPlayer::Instance() ),
    m_video_duration( 0 ),
    m_paused( false ),
    m_seeking( false )
{
    ui->setupUi(this);

    m_volume_slider = new QDropDownVolumeSlider( ui->btn_volume, 0, 100 );
    m_volume_slider->SetValue( 100 );

    connect( ui->btn_play_stop, SIGNAL( clicked(bool) ), this, SLOT( OnPlayStopClicked() ) );
    connect( ui->btn_play_stop, SIGNAL( clicked(bool) ), this, SLOT( DelayNextClick() ) );

    connect( ui->frame_area, SIGNAL( frameShown() ), this, SLOT( OnFrameShown() ) );
    connect( ui->frame_area, SIGNAL( frameProcessed(AVFrame*) ), this, SLOT( OnFrameProcessed(AVFrame*) ) );
    connect( ui->time_slider, SIGNAL( sliderReleased()), this, SLOT( OnSliderReleased() ) );
    connect( ui->time_slider, SIGNAL( sliderPressed()), this, SLOT( OnSliderPressed() ) );
    connect( m_volume_slider, SIGNAL( valueChanged( int ) ), this, SIGNAL( setVolume( int ) ) );
    connect( ui->btn_volume, SIGNAL( clicked( bool ) ), this, SLOT( OnBtnVolumeClicked( bool ) ) );
    connect( ui->btn_fullscreen, SIGNAL( clicked( bool ) ), this, SLOT( OnFullScreenRequested( bool ) ) );
    connect( &m_controls_hide_timer, SIGNAL( timeout() ), this, SLOT( OnHideTimerTimeout() ) );
    connect( ui->action_open, SIGNAL( triggered() ), this, SLOT( OnOpenFile() ) );
    connect( ui->action_quit, SIGNAL( triggered() ), this, SLOT( close() ) );

    //---------------------------------------------------------------------------------------

    ui->btn_play_stop->SetIconSize( QSize( 32, 32 ) );
    ui->btn_fullscreen->SetIconSize( QSize( 32, 24 ) );
    ui->btn_volume->SetIconSize( QSize( 32, 24 ) );

    ui->btn_play_stop->SetNormalIcon( QIcon( ":img/resources/play.png" ) );
    ui->btn_play_stop->SetHoverIcon( QIcon( ":/img/resources/play_hover.png" ) );
    ui->btn_play_stop->SetDisabledIcon( QIcon( ":img/resources/play_disabled.png" ) );
    ui->btn_play_stop->SetClickedIcon( QIcon( ":img/resources/play_clicked.png" ) );

    ui->btn_play_stop->SetCheckedIcon( QIcon( ":img/resources/pause.png" ) );
    ui->btn_play_stop->SetCheckedHoverIcon( QIcon( ":/img/resources/pause_hover.png" ) );
    ui->btn_play_stop->SetCheckedDisabledIcon( QIcon( ":img/resources/pause_disabled.png" ) );
    ui->btn_play_stop->SetClickedCheckedIcon( QIcon( ":img/resources/pause_clicked.png" ) );

    //---------------------------------------------------------------------------------------

    ui->btn_volume->SetNormalIcon( QIcon( ":img/resources/vol.png" ) );
    ui->btn_volume->SetHoverIcon( QIcon( ":/img/resources/vol_hover.png" ) );
    ui->btn_volume->SetDisabledIcon( QIcon( ":img/resources/vol_disabled.png" ) );
    ui->btn_volume->SetClickedIcon( QIcon( ":img/resources/vol_clicked.png" ) );

    ui->btn_volume->SetCheckedIcon( QIcon( ":img/resources/muted.png" ) );
    ui->btn_volume->SetCheckedHoverIcon( QIcon( ":/img/resources/muted_hover.png" ) );
    ui->btn_volume->SetCheckedDisabledIcon( QIcon( ":img/resources/muted_disabled.png" ) );
    ui->btn_volume->SetClickedCheckedIcon( QIcon( ":img/resources/muted_clicked.png" ) );

    //---------------------------------------------------------------------------------------

    ui->btn_fullscreen->SetNormalIcon( QIcon( ":img/resources/fullscreen.png" ) );
    ui->btn_fullscreen->SetHoverIcon( QIcon( ":/img/resources/fullscreen_hover.png" ) );
    ui->btn_fullscreen->SetDisabledIcon( QIcon( ":img/resources/fullscreen_disabled.png" ) );
    ui->btn_fullscreen->SetClickedIcon( QIcon( ":img/resources/fullscreen_clicked.png" ) );

    ui->btn_fullscreen->SetCheckedIcon( QIcon( ":img/resources/win.png" ) );
    ui->btn_fullscreen->SetCheckedHoverIcon( QIcon( ":/img/resources/win_hover.png" ) );
    ui->btn_fullscreen->SetCheckedDisabledIcon( QIcon( ":img/resources/win_disabled.png" ) );
    ui->btn_fullscreen->SetClickedCheckedIcon( QIcon( ":img/resources/win_clicked.png" ) );

    ui->btn_play_stop->setEnabled( false );
    ui->time_slider->setEnabled( false );
    ui->frame_area->installEventFilter( this );
    ui->widget_btns->installEventFilter( this );
}

void QEdgeMainWindow::Init( IPlayer* player )
{
    m_player.reset( player );
}

void QEdgeMainWindow::OnVideo( AVFrame *frame )
{
    ui->frame_area->OnNewFrame( frame );
}

void QEdgeMainWindow::OnAudio( AVFrame *frame )
{
    Q_UNUSED( frame );
}

void QEdgeMainWindow::OnFailed( QString err_text )
{
    qDebug() << "[QEdgeMainWindow] " << err_text ;
}

void QEdgeMainWindow::OnFinished()
{
    m_started = false;
    m_paused = false;
    ui->btn_play_stop->setChecked( false );
    ui->label_time->setText( "00:00" );
    ui->time_slider->setValue( 0 );
}

void QEdgeMainWindow::PlayerStarted()
{
    ui->time_slider->setEnabled( true );
    emit setVolume( m_volume_slider->GetValue() );
}

void QEdgeMainWindow::PlayerStopped()
{
    ui->btn_play_stop->setChecked( false );
}

void QEdgeMainWindow::OnSliderPressed()
{
    m_seeking = true;
}

void QEdgeMainWindow::OnSliderReleased()
{
    RequestSeek( ui->time_slider->value() );
    m_seeking = false;
}

void QEdgeMainWindow::DelayNextClick()
{
    if( sender() )
    {
        QPushButton* btn = qobject_cast<QPushButton*>( sender() );

        if( btn )
        {
            m_clicked_btn = btn;
            m_clicked_btn->setEnabled( false );
            QTimer::singleShot( 600, Qt::PreciseTimer, this, SLOT( EnableClickedBtn() ) );
        }
    }
}

void QEdgeMainWindow::DurationSpecified( int64_t msecs )
{
    ui->label_time_total->setText( QTime( 0, 0, 0, 0 ).addMSecs( msecs ).toString("hh:mm:ss") );
    ui->time_slider->setMinimum( 0 );
    ui->time_slider->setMaximum( msecs );
    m_video_duration = msecs;
}

void QEdgeMainWindow::CurrentTimestampChanged( int64_t msecs )
{
    ui->label_time->setText( QTime( 0, 0, 0, 0 ).addMSecs( msecs ).toString("hh:mm:ss") );

    if( !m_seeking )
    {
        ui->time_slider->setValue( msecs );
    }
}

void QEdgeMainWindow::closeEvent( QCloseEvent *ev )
{
    m_player->Stop();

    QMainWindow::closeEvent( ev );
}

bool QEdgeMainWindow::eventFilter( QObject* watched, QEvent *event )
{
    if( watched == ui->frame_area )
    {
        if( isFullScreen() )
        {
            if( event->type() == QEvent::MouseMove )
            {
                if( isFullScreen() )
                {
                    ui->widget_btns->show();
                    m_controls_hide_timer.stop();
                    m_controls_hide_timer.start( 2000 );
                    ui->frame_area->update();
                }
            }
        }
    }

    else if( watched == ui->widget_btns )
    {
        if( isFullScreen() )
        {
            if( event->type() == QEvent::Leave )
            {
                m_controls_hide_timer.stop();
                m_controls_hide_timer.start( 2000 );
            }
        }
    }

    return false;
}

bool QEdgeMainWindow::event( QEvent *event )
{
    if( event->type() == QEvent::MouseButtonDblClick )
    {
        ui->btn_fullscreen->click();
    }

    else if( event->type() == QEvent::KeyPress )
    {
        QKeyEvent* key_event = static_cast<QKeyEvent*>( event );

        if( key_event->key() == Qt::Key_Escape )
        {
            if( isFullScreen() )
            {
                ui->btn_fullscreen->click();
            }
        }
    }

    return QWidget::event( event );
}

void QEdgeMainWindow::RequestSeek( int seek_value )
{
    m_player->Seek( seek_value );
}

void QEdgeMainWindow::OnPlayStopClicked()
{
    if( !m_started )
    {
        m_started = true;
        m_player->Start( m_file_name );
        return;
    }
    if( !m_paused )
    {
        m_player->Pause();
    }
    else
    {
        m_player->ContinuePlay();
    }

    m_paused = !m_paused;

    ui->time_slider->setEnabled( !m_paused );
}

void QEdgeMainWindow::EnableClickedBtn()
{
    m_clicked_btn->setEnabled( true );
}

void QEdgeMainWindow::OnFrameShown()
{
    m_player->VideoPresented();
}

void QEdgeMainWindow::OnFrameProcessed( AVFrame *frame )
{
    m_player->VideoProcessed( frame );
}

void QEdgeMainWindow::OnBtnVolumeClicked( bool checked )
{
    int value = checked ? 0 : 100;
    m_volume_slider->SetValue( value );
    emit setVolume( value );
}

void QEdgeMainWindow::OnFullScreenRequested( bool fs )
{
    if( fs )
    {
        m_controls_hide_timer.start( 2000 );
        showFullScreen();
    }

    else
    {
        m_controls_hide_timer.stop();
        ui->widget_btns->show();
        ui->menu_bar->show();
        showNormal();
    }
}

void QEdgeMainWindow::OnHideTimerTimeout()
{
    if( isFullScreen() )
    {
        QPoint mouse_pos = QCursor::pos();
        QRect geometry = ui->widget_btns->geometry();

        if( mouse_pos.x() >= geometry.left() && mouse_pos.x() <= geometry.right()
                && mouse_pos.y() >= geometry.top() && mouse_pos.y() <= geometry.bottom() )
        {
            return;
        }
    }

    ui->widget_btns->hide();
    ui->menu_bar->hide();
}

void QEdgeMainWindow::OnOpenFile()
{
    QString file_name = QFileDialog::getOpenFileName( this, tr( "Open Video" ), "",
        tr( "Video Files (*.mp4 *.3gp *.avi);" ) );

    if( !file_name.isEmpty() )
    {
        m_file_name = file_name;
        ui->btn_play_stop->setEnabled( true );
        ui->btn_play_stop->setChecked( true );
        m_player->Start( m_file_name );
        m_started = true;
    }
}

QEdgeMainWindow::~QEdgeMainWindow()
{
    m_player.release();
    delete ui;
}
