#include "mqtt_connector/mqtt_client.h"

#include <mqtt/async_client.h>
#include <mqtt/message.h>
#include <algorithm>
#include <sstream>

namespace mqtt_connector {

MqttClient::MqttClient() 
    : connection_manager_(std::make_unique<ConnectionManager>())
    , message_handler_(std::make_unique<MessageHandler>())
    , initialized_(false) {
}

MqttClient::~MqttClient() {
    shutdown();
}

bool MqttClient::initialize(const ConnectionConfig& config) {
    if (initialized_) {
        shutdown();
    }
    
    current_config_ = config;
    
    connection_manager_->setConnectionCallback([this](ConnectionState state) {
        if (state == ConnectionState::CONNECTED) {
            restoreSubscriptions();
        }
    });
    
    if (!connection_manager_->connect(config)) {
        return false;
    }
    
    initialized_ = true;
    return true;
}

void MqttClient::shutdown() {
    if (!initialized_) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(subscriptions_mutex_);
        for (const auto& topic : subscriptions_) {
            try {
                if (connection_manager_->isConnected()) {
                    auto client = connection_manager_->getClient();
                    if (client) {
                        auto token = client->unsubscribe(topic);
                        token->wait_for(std::chrono::seconds(5));
                    }
                }
            } catch (const std::exception&) {
                // Игнорируем ошибки при отключении
            }
        }
        subscriptions_.clear();
    }
    
    connection_manager_->disconnect();
    message_handler_->clearHandlers();
    
    initialized_ = false;
}

bool MqttClient::subscribe(const std::string& topic, int qos, MessageCallback callback) {
    if (!initialized_ || !connection_manager_->isConnected()) {
        return false;
    }
    
    try {
        auto client = connection_manager_->getClient();
        if (!client) {
            return false;
        }
        
        if (callback) {
            message_handler_->registerHandler(topic, callback);
        }
        
        auto token = client->subscribe(topic, qos);
        token->wait_for(std::chrono::seconds(10));
        
        if (token->get_return_code() == mqtt::ReasonCode::SUCCESS) {
            std::lock_guard<std::mutex> lock(subscriptions_mutex_);
            if (std::find(subscriptions_.begin(), subscriptions_.end(), topic) == subscriptions_.end()) {
                subscriptions_.push_back(topic);
            }
            return true;
        }
        
        return false;
        
    } catch (const std::exception& e) {
        connection_manager_->setErrorCallback([e](const std::string&) {
            // Обработка ошибки подписки
        });
        return false;
    }
}

bool MqttClient::unsubscribe(const std::string& topic) {
    if (!initialized_ || !connection_manager_->isConnected()) {
        return false;
    }
    
    try {
        auto client = connection_manager_->getClient();
        if (!client) {
            return false;
        }
        
        auto token = client->unsubscribe(topic);
        token->wait_for(std::chrono::seconds(10));
        
        if (token->get_return_code() == mqtt::ReasonCode::SUCCESS) {
            message_handler_->unregisterHandler(topic);
            
            std::lock_guard<std::mutex> lock(subscriptions_mutex_);
            subscriptions_.erase(
                std::remove(subscriptions_.begin(), subscriptions_.end(), topic),
                subscriptions_.end()
            );
            
            return true;
        }
        
        return false;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool MqttClient::publish(const Message& message) {
    if (!initialized_ || !connection_manager_->isConnected()) {
        return false;
    }
    
    try {
        auto client = connection_manager_->getClient();
        if (!client) {
            return false;
        }
        
        auto mqtt_msg = mqtt::make_message(message.topic, message.payload);
        mqtt_msg->set_qos(message.qos);
        mqtt_msg->set_retained(message.retained);
        
        auto token = client->publish(mqtt_msg);
        token->wait_for(std::chrono::seconds(10));
        
        return token->get_return_code() == mqtt::ReasonCode::SUCCESS;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool MqttClient::publish(const std::string& topic, const std::string& payload, int qos, bool retained) {
    return publish(Message(topic, payload, qos, retained));
}

bool MqttClient::isConnected() const {
    return initialized_ && connection_manager_->isConnected();
}

ConnectionState MqttClient::getConnectionState() const {
    if (!initialized_) {
        return ConnectionState::DISCONNECTED;
    }
    return connection_manager_->getConnectionState();
}

void MqttClient::setDefaultMessageHandler(MessageCallback callback) {
    message_handler_->setDefaultHandler(callback);
}

void MqttClient::setConnectionStateHandler(ConnectionCallback callback) {
    if (connection_manager_) {
        connection_manager_->setConnectionCallback(callback);
    }
}

void MqttClient::setErrorHandler(ErrorCallback callback) {
    if (connection_manager_) {
        connection_manager_->setErrorCallback(callback);
    }
}

std::vector<std::string> MqttClient::getActiveSubscriptions() const {
    std::lock_guard<std::mutex> lock(subscriptions_mutex_);
    return subscriptions_;
}

void MqttClient::setAutoReconnect(bool enable, int retry_interval) {
    if (connection_manager_) {
        connection_manager_->setAutoReconnect(enable, retry_interval);
    }
}

std::string MqttClient::getStatus() const {
    std::ostringstream status;
    
    status << "MQTT Client Status:\n";
    status << "  Initialized: " << (initialized_ ? "Yes" : "No") << "\n";
    status << "  Connected: " << (isConnected() ? "Yes" : "No") << "\n";
    status << "  State: ";
    
    switch (getConnectionState()) {
        case ConnectionState::DISCONNECTED: status << "Disconnected"; break;
        case ConnectionState::CONNECTING: status << "Connecting"; break;
        case ConnectionState::CONNECTED: status << "Connected"; break;
        case ConnectionState::RECONNECTING: status << "Reconnecting"; break;
        case ConnectionState::FAILED: status << "Failed"; break;
    }
    
    status << "\n";
    status << "  Broker: " << current_config_.broker_host << ":" << current_config_.broker_port << "\n";
    status << "  Client ID: " << current_config_.client_id << "\n";
    
    {
        std::lock_guard<std::mutex> lock(subscriptions_mutex_);
        status << "  Active subscriptions: " << subscriptions_.size() << "\n";
        for (const auto& topic : subscriptions_) {
            status << "    - " << topic << "\n";
        }
    }
    
    if (connection_manager_) {
        auto lastError = connection_manager_->getLastError();
        if (!lastError.empty()) {
            status << "  Last error: " << lastError << "\n";
        }
    }
    
    return status.str();
}

void MqttClient::onMessageReceived(const Message& message) {
    message_handler_->handleMessage(message);
}

void MqttClient::restoreSubscriptions() {
    std::lock_guard<std::mutex> lock(subscriptions_mutex_);
    
    for (const auto& topic : subscriptions_) {
        try {
            auto client = connection_manager_->getClient();
            if (client) {
                client->subscribe(topic, 0);
            }
        } catch (const std::exception&) {
            // Игнорируем ошибки восстановления подписок
        }
    }
}

} // namespace mqtt_connector
