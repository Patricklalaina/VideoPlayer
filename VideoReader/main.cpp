#include "videoreader.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VideoReader w;
    w.show();
    return a.exec();
}
