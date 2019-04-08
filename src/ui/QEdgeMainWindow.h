#ifndef QEDGEMAINWINDOW_H
#define QEDGEMAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <core/IPlayer.h>

namespace Ui {
class QEdgeMainWindow;
}

class QEdgeMainWindow : public QMainWindow, public IPlayer::IPlayerClient
{
    Q_OBJECT

public:
    explicit QEdgeMainWindow(QWidget *parent = 0);
    ~QEdgeMainWindow();

    virtual void Init( IPlayer* player ) override;
    virtual void OnVideo( AVFrame* frame ) override;
    virtual void OnAudio( AVFrame *frame ) override;
    virtual void OnFailed( QString err_text ) override;
    virtual void OnFinished() override;

private slots:
    void OnPlayStopClicked();

private:
    Ui::QEdgeMainWindow *ui;
    bool m_started;
    IPlayer* m_player;
};

#endif // QEDGEMAINWINDOW_H
