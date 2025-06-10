#include "mainwindow.h"
#include "projectmanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<Agent*>("Agent*");
    QApplication a(argc, argv);
    ProjectManager manager;
    manager.exec();
    if (manager.projectPath().isEmpty()) {
        return 0;
    }

    MainWindow w(manager.projectPath());
    w.show();
    return a.exec();
}
