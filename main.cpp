#include <QApplication>
#include <QPushButton>

#include "mainwindow.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow window;
    window.resize(1200, 800);
    window.show();
    return QApplication::exec();
}
