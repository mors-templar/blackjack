#include "mainwindow.h"
#include "welcome.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Welcome wel;
    wel.show();
    w.show();
    return a.exec();
}
