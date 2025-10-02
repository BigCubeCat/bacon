#include <QApplication>
#include <QObject>

#include "mainwindow.hpp"
#include "model.hpp"

#include <QObject>

#include <memory.h>
#include <qobject.h>
#include <qtimer.h>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    std::shared_ptr<mqtt_connector::MqttClient> conn =
        std::make_shared<mqtt_connector::MqttClient>();
    std::shared_ptr<Model> model = std::make_shared<Model>(conn.get());

    MainWindow window(model.get());
    window.resize(1200, 800);
    window.show();

    QObject::connect(conn.get(), &mqtt_connector::MqttClient::addPathPoint, model.get(), &Model::addPointToPath);
    QObject::connect(model.get(), &Model::urlChanged, conn.get(), &mqtt_connector::MqttClient::initOnChange);
    QObject::connect(model.get(), &Model::freqChanged, conn.get(), &mqtt_connector::MqttClient::setFreqOnChange);
    QObject::connect(model.get(), &Model::signalBeaconsChanged, conn.get(), &mqtt_connector::MqttClient::setBeacons);

    // Создаем таймер
    QTimer timer;

    int i = 0;

    // Подключаем сигнал timeout к лямбда-функции
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        std::cout << i << std::endl;
        model->addPointToPath(QPointF(0, ++i));
    });

    // Таймер срабатывает каждые 1000 мс (1 секунда)
    timer.start(1000);

    return QApplication::exec();
}
