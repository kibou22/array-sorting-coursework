#include <QApplication>

#include "mainwindow.h"

// Створює застосунок Qt та відкриває головне вікно.
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}