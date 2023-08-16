#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("Pavelsis");
    w.setFixedSize(1280,720);
    w.show();
    return a.exec();
}


