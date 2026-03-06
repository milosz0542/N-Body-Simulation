#include <QApplication>

#include "gui/MainWindow.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow window;
    window.resize(1024, 768);
    window.setWindowTitle("N-Body Simulation");
    window.show();

    return a.exec();
}