#pragma once

#include "types.h"
#include "message_handler.h"
#include "connection_manager.h"
#include <memory>
#include <vector>
#include <mutex>

#include <QObject>

#include <mqtt/callback.h>

class callback : public virtual mqtt::callback {
    void message_arrived(mqtt::const_message_ptr msg) override {
        std::cout << "Message received: " << msg->get_payload_str() << std::endl;
    }
};

namespace mqtt_connector {

/**
 * @brief Основной класс MQTT клиента для приема сообщений
 */
class MqttClient : public QObject {
    Q_OBJECT
    
public:
    MqttClient();
    ~MqttClient();

    /**
     * @brief Инициализация и подключение к MQTT брокеру
     * @param config Конфигурация подключения
     * @return true если подключение успешно
     */
    bool initialize(const ConnectionConfig& config);

    /**
     * @brief Отключение от брокера и очистка ресурсов
     */
    void shutdown();

    /**
     * @brief Подписка на топик
     * @param topic Топик для подписки (поддерживает wildcards: +, #)
     * @param qos Quality of Service уровень (0, 1, 2)
     * @param callback Функция обработки сообщений для этого топика
     * @return true если подписка успешна
     */
    bool subscribe(const std::string& topic, int qos = 0, MessageCallback callback = nullptr);

    /**
     * @brief Отписка от топика
     * @param topic Топик для отписки
     * @return true если отписка успешна
     */
    bool unsubscribe(const std::string& topic);

    /**
     * @brief Публикация сообщения
     * @param message Сообщение для публикации
     * @return true если публикация успешна
     */
    bool publish(const Message& message);

    /**
     * @brief Публикация сообщения (расширенное)
     * @param topic Топик
     * @param payload Содержимое сообщения
     * @param qos Quality of Service уровень
     * @param retained Флаг retained
     * @return true если публикация успешна
     */
    bool publish(const std::string& topic, const std::string& payload, int qos = 0, bool retained = false);

    /**
     * @brief Проверка состояния подключения
     * @return true если подключен к брокеру
     */
    bool isConnected() const;

    /**
     * @brief Получение текущего состояния подключения
     * @return Состояние подключения
     */
    ConnectionState getConnectionState() const;

    /**
     * @brief Регистрация обработчика сообщений по умолчанию
     * @param callback Функция обработки всех сообщений
     */
    void setDefaultMessageHandler(MessageCallback callback);

    /**
     * @brief Установка обработчика изменений состояния подключения
     * @param callback Функция обработки изменений состояния
     */
    void setConnectionStateHandler(ConnectionCallback callback);

    /**
     * @brief Установка обработчика ошибок
     * @param callback Функция обработки ошибок
     */
    void setErrorHandler(ErrorCallback callback);

    /**
     * @brief Получение списка активных подписок
     * @return Вектор топиков, на которые подписан клиент
     */
    std::vector<std::string> getActiveSubscriptions() const;

    /**
     * @brief Включение/отключение автоматического переподключения
     * @param enable true для включения
     * @param retry_interval Интервал между попытками в секундах
     */
    void setAutoReconnect(bool enable, int retry_interval = 5);

    /**
     * @brief Получение статистики клиента
     * @return Строка с информацией о состоянии
     */
    std::string getStatus() const;

    Q_SIGNALS:
    void addPathPoint(const QPointF &pos);

public slots:
    void initOnChange(const QString &url);
    void setFreqOnChange(float freq);    

private:
    std::unique_ptr<ConnectionManager> connection_manager_;
    std::unique_ptr<MessageHandler> message_handler_;
    
    std::vector<std::string> subscriptions_;
    mutable std::mutex subscriptions_mutex_;
    
    ConnectionConfig current_config_;
    bool initialized_;

    /**
     * @brief Внутренний обработчик входящих сообщений
     * @param message Полученное сообщение
     */
    void onMessageReceived(const Message& message);

    /**
     * @brief Восстановление подписок после переподключения
     */
    void restoreSubscriptions();

    float m_freq = 1.0f;
};

} // namespace mqtt_connector