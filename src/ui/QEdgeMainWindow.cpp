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
    m_seeking( false )
{
    ui->setupUi(this);

    connect( ui->btn_play_stop, SIGNAL( clicked(bool) ), this, SLOT( OnPlayStopClicked() ) );
    connect( ui->frame_area, SIGNAL( frameShown() ), this, SLOT( OnFrameShown() ) );
    connect( ui->frame_area, SIGNAL( frameProcessed(AVFrame*) ), this, SLOT( OnFrameProcessed(AVFrame*) ) );
    connect( ui->btn_forward, SIGNAL( clicked(bool) ), this, SLOT( OnSeekForward()) );
    connect( ui->btn_back, SIGNAL( clicked(bool)), this, SLOT(OnSeekBackward()) );
    connect( ui->time_slider, SIGNAL( sliderReleased()), this, SLOT( OnSliderReleased() ) );
    connect( ui->time_slider, SIGNAL( sliderPressed()), this, SLOT( OnSliderPressed() ) );
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
    if( !m_started )
    {
        m_player->Start( QString("C:/Users/Shemberko/Desktop/testb.mp4") );
    }
    else
    {
        m_player->Stop();
    }

    m_started = !m_started;

    ui->btn_play_stop->setEnabled( false );
    QTimer::singleShot( 300, Qt::PreciseTimer, this, SLOT( EnablePlay() ) );
}

void QEdgeMainWindow::EnablePlay()
{
    ui->btn_play_stop->setEnabled( true );
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
    delete ui;
}
