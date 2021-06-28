#include "widget.h"
#include <QTextCodec>
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    qRegisterMetaType<std::string>("std::string");
    Widget w;
    w.show();
    return a.exec();
}