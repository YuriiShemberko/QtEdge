#include "ui_QEdgeMainWindow.h"
#include <ui/QEdgeMainWindow.h>

#include <QDebug>
#include <QTimer>

QEdgeMainWindow::QEdgeMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QEdgeMainWindow),
    m_started( false ),
    m_player( CNullPlayer::Instance() )
{
    ui->setupUi(this);

    connect( ui->btn_play_stop, SIGNAL( clicked(bool) ), this, SLOT( OnPlayStopClicked() ) );
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

void QEdgeMainWindow::OnPlayStopClicked()
{
    if( !m_started )
    {
        m_player->Start( QString("C:/Users/Shemberko/Desktop/test.mp4") );
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

QEdgeMainWindow::~QEdgeMainWindow()
{
    delete ui;
}
