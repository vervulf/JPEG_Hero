#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    QCoreApplication::setLibraryPaths(paths);
    w.get_app_path(a.applicationDirPath());
    w.show();

    return a.exec();
}
