#include "ui_QEdgeMainWindow.h"
#include <ui/QEdgeMainWindow.h>

QEdgeMainWindow::QEdgeMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QEdgeMainWindow),
    m_provider( CNullFrameProvider::Instance() )
{
    ui->setupUi(this);

    connect( ui->btn_play_stop, SIGNAL(clicked(bool)), this, SLOT(OnPlayStopClicked()));
}

void QEdgeMainWindow::Init( IFrameProvider *provider )
{
    provider->BindClient( this );
    m_provider = provider;
}

void QEdgeMainWindow::OnNewFrame( const QImage &image )
{
    ui->frame_area->UpdateImage( image );
}

QSize QEdgeMainWindow::TargetSize()
{
    return ui->frame_area->size();
}

void QEdgeMainWindow::OnPlayStopClicked()
{
    SFrameProviderConfig config;
    config.file_name = QString( "C:/Users/Shemberko/Desktop/test4.mp4" );
    m_provider->Start( config );
}

QEdgeMainWindow::~QEdgeMainWindow()
{
    delete ui;
}
