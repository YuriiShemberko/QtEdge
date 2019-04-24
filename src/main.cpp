#include <QApplication>
#include <core/QEdgePlayer.h>
#include <core/QEdgeAudioReproductor.h>
#include <ui/QEdgeMainWindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QEdgePlayer player;
    QEdgeMainWindow main_window;
    QEdgeAudioReproductor audio_reproductor;

    player.InitPlayer( &main_window, &audio_reproductor );
    audio_reproductor.Init( &player );
    main_window.Init( &player );

    QObject::connect( &main_window, SIGNAL(setVolume(int) ), &audio_reproductor, SLOT(OnSetVolume(int) ) );

    main_window.show();

    return a.exec();
}
