#include <QApplication>
#include <core/QEdgePlayer.h>
#include <core/QEdgeAudioReproductor.h>
#include <ui/QEdgeMainWindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QEdgePlayer player;
    QEdgeMainWindow w;
    QEdgeAudioReproductor audio_reproductor;

    audio_reproductor.Init( &player );
    w.Init( &player );
    w.show();

    return a.exec();
}
