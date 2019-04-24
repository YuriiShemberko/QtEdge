#include "ui_QEdgeMainWindow.h"
#include <ui/QEdgeMainWindow.h>

#include <QDebug>
#include <QTimer>
#include <QDateTime>

QEdgeMainWindow::QEdgeMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QEdgeMainWindow),
    m_started( false ),
    m_player( CNullPlayer::Instance() ),
    m_video_duration( 0 ),
    m_seeking( false ),
    m_paused( false )
{
    ui->setupUi(this);

    connect( ui->btn_play_stop, SIGNAL( clicked(bool) ), this, SLOT( OnPlayStopClicked() ) );
    connect( ui->btn_play_stop, SIGNAL( clicked(bool) ), this, SLOT( DelayNextClick() ) );

    connect( ui->btn_forward, SIGNAL( clicked(bool) ), this, SLOT( OnSeekForward() ) );
    connect( ui->btn_forward, SIGNAL( clicked(bool) ), this, SLOT( DelayNextClick() ) );

    connect( ui->btn_back, SIGNAL( clicked(bool) ), this, SLOT( OnSeekBackward() ) );
    connect( ui->btn_back, SIGNAL( clicked(bool) ), this, SLOT( DelayNextClick() ) );

    connect( ui->frame_area, SIGNAL( frameShown() ), this, SLOT( OnFrameShown() ) );
    connect( ui->frame_area, SIGNAL( frameProcessed(AVFrame*) ), this, SLOT( OnFrameProcessed(AVFrame*) ) );
    connect( ui->time_slider, SIGNAL( sliderReleased()), this, SLOT( OnSliderReleased() ) );
    connect( ui->time_slider, SIGNAL( sliderPressed()), this, SLOT( OnSliderPressed() ) );
    connect( ui->volume_slider, SIGNAL( sliderMoved(int) ), this, SIGNAL( setVolume(int) ) );
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
}

void QEdgeMainWindow::PlayerStarted()
{

}

void QEdgeMainWindow::PlayerStopped()
{

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

void QEdgeMainWindow::RequestSeek( int seek_value )
{
    m_player->Seek( seek_value );
}

void QEdgeMainWindow::OnSeekForward()
{
    int seek_value = std::min( m_video_duration, ui->time_slider->value() + 5000 );
    ui->time_slider->setValue( seek_value );
    RequestSeek( seek_value );
}

void QEdgeMainWindow::OnSeekBackward()
{
    int seek_value = std::max( 0, ui->time_slider->value() - 5000 );
    ui->time_slider->setValue( seek_value );
    RequestSeek( seek_value );
}

void QEdgeMainWindow::OnPlayStopClicked()
{
    //TMP!!!
    static bool f = false;
    if( !f )
    {
        f = true;
        m_file_name = QString("C:/Users/Shemberko/Desktop/testb.mp4");
        m_player->Start( m_file_name );
        m_started = true;
        return;
    }
    //------------------

    if( !m_paused )
    {
        m_player->Pause();
    }
    else
    {
        m_player->ContinuePlay();
    }

    m_paused = !m_paused;
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

QEdgeMainWindow::~QEdgeMainWindow()
{
    m_player.release();
    delete ui;
}
