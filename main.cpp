#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<Agent*>("Agent*");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
