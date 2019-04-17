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

private slots:
    void OnPlayStopClicked();
    void EnablePlay();

signals:
    void PlayStopClicked( bool play );

private:

    Ui::QEdgeMainWindow *ui;
    bool m_started;
    std::unique_ptr<IPlayer> m_player;
};

#endif // QEDGEMAINWINDOW_H
