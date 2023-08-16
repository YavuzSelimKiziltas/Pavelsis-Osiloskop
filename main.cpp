#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setStyleSheet("#Widget{background-color: #525252;}");
    w.setWindowTitle("Pavelsis");
    w.setFixedSize(1280,720);
    w.show();
    return a.exec();
}


