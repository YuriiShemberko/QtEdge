#include <QApplication>
#include <core/QEdgePlayer.h>
#include <ui/QEdgeMainWindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QEdgePlayer player;
    QEdgeMainWindow w;

    w.Init( &player );
    w.show();

    return a.exec();
}
