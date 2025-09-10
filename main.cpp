#include <QApplication>
#include <QSurfaceFormat>
#include "welcome.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Welcome welcome;
    if (welcome.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();
        return app.exec();
    }

    return 0;
}
