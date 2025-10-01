#include <QApplication>

#include "mainwindow.hpp"
#include "model.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    std::shared_ptr<Model> model = std::make_shared<Model>();
    MainWindow window(model);

    window.resize(1200, 800);
    window.show();

    return QApplication::exec();
}
