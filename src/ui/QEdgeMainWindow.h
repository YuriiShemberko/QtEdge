#ifndef QEDGEMAINWINDOW_H
#define QEDGEMAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <core/IPlayer.h>
#include <core/QEdgeAudioReproductor.h>

namespace Ui {
class QEdgeMainWindow;
}

class QEdgeMainWindow : public QMainWindow, public IPlayer::IPlayerClient
{
    Q_OBJECT

public:
    explicit QEdgeMainWindow(QWidget *parent = nullptr);
    ~QEdgeMainWindow();

    virtual void Init( IPlayer* player ) override;
    virtual void OnVideo( AVFrame* frame ) override;
    virtual void OnAudio( AVFrame *frame ) override;
    virtual void OnFailed( QString err_text ) override;
    virtual void OnFinished() override;
    virtual void PlayerStarted() override;
    virtual void PlayerStopped() override;
    virtual void DurationSpecified( int64_t msecs ) override;
    virtual void CurrentTimestampChanged( int64_t msecs ) override;

protected:
    void closeEvent( QCloseEvent* ev ) override;

private slots:
    void OnPlayStopClicked();
    void OnSliderPressed();
    void OnSliderReleased();
    void EnablePlay();

    void OnSeekForward();
    void OnSeekBackward();

    void OnFrameShown();
    void OnFrameProcessed( AVFrame* frame );

signals:
    void PlayStopClicked( bool play );

private:
    void RequestSeek( int seek_value );

    Ui::QEdgeMainWindow *ui;
    bool m_started;
    std::unique_ptr<IPlayer> m_player;
    int m_video_duration;

    volatile bool m_seeking;
};

#endif // QEDGEMAINWINDOW_H
