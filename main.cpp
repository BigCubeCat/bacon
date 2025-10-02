#include <QApplication>

#include "mainwindow.hpp"
#include "model.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    std::shared_ptr<mqtt_connector::MqttClient> conn = std::make_shared<mqtt_connector::MqttClient>();
    std::shared_ptr<Model> model = std::make_shared<Model>(conn.get());
    MainWindow window(model.get());

    window.resize(1200, 800);
    window.show();

    return QApplication::exec();
}
