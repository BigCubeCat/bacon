#include <QApplication>

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

    QObject::connect(conn.get(), &mqtt_connector::MqttClient::addPathPoint,
                     model.get(), &Model::addPointToPath);

    // Создаем таймер
    QTimer timer;

    int i = 0;

    // Подключаем сигнал timeout к лямбда-функции
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        std::cout << i << std::endl;
        model->addPointToPath(QPointF(0, ++i));
    });

    // Таймер срабатывает каждые 1000 мс (1 секунда)
    timer.start(2000);

    return QApplication::exec();
}
