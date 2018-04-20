#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.get_app_path(a.applicationDirPath());
    w.show();

    return a.exec();
}
