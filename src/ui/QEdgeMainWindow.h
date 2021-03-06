#ifndef QEDGEMAINWINDOW_H
#define QEDGEMAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <core/IPlayer.h>
#include <core/QEdgeAudioReproductor.h>
#include <ui/QEdgeButton.h>
#include <ui/QDropDownVolumeSlider.h>
#include <QTimer>

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
    virtual void closeEvent( QCloseEvent* ev ) override;
    virtual bool eventFilter( QObject* watched, QEvent* event ) override;
    virtual bool event( QEvent* event ) override;

private slots:
    void OnPlayStopClicked();
    void OnSliderPressed();
    void OnSliderReleased();
    void DelayNextClick();
    void EnableClickedBtn();
    void OnFrameShown();
    void OnFrameProcessed( AVFrame* frame );
    void OnBtnVolumeClicked( bool checked );
    void OnFullScreenRequested( bool fs );
    void OnHideTimerTimeout();
    void OnOpenFile();

signals:
    void PlayStopClicked( bool play );
    void setVolume( int value );

private:
    void RequestSeek( int seek_value );

    Ui::QEdgeMainWindow *ui;
    bool m_started;
    std::unique_ptr<IPlayer> m_player;
    int m_video_duration;
    bool m_paused;
    volatile bool m_seeking;
    QString m_file_name;

    QTimer m_controls_hide_timer;

    QDropDownVolumeSlider* m_volume_slider;
    QPushButton* m_clicked_btn;
};

#endif // QEDGEMAINWINDOW_H
