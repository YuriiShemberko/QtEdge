#include <QApplication>
#include <core/QEdgeFrameProvider.h>
#include <ui/QEdgeMainWindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QEdgeMainWindow w;
    QEdgeFrameProvider provider;

    w.Init( &provider );
    w.show();

    return a.exec();
}
