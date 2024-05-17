#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setDesktopFileName("org.kde.system-installer");
    MainWindow w;
    w.show();
    return a.exec();
}
