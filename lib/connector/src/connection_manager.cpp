#include "mqtt_connector/connection_manager.h"
#include <mqtt/async_client.h>
#include <mqtt/callback.h>
#include <mqtt/ssl_options.h>
#include <chrono>

namespace mqtt_connector {

/**
 * @brief Внутренний callback класс для обработки событий Paho MQTT
 */
class MqttCallback : public mqtt::callback {
public:
    explicit MqttCallback(ConnectionManager* manager) : manager_(manager) {}

    void connection_lost(const std::string& cause) override {
        if (manager_) {
            manager_->setState(ConnectionState::DISCONNECTED);
            manager_->handleError("Connection lost: " + cause);
        }
    }

    void delivery_complete(mqtt::delivery_token_ptr tok) override {
        // Обработка подтверждения доставки (если нужно)
    }

private:
    ConnectionManager* manager_;
};

ConnectionManager::ConnectionManager() 
    : connection_state_(ConnectionState::DISCONNECTED)
    , auto_reconnect_(false)
    , retry_interval_(5)
    , should_stop_(false) {
}

ConnectionManager::~ConnectionManager() {
    disconnect();
    
    should_stop_ = true;
    if (reconnect_thread_.joinable()) {
        reconnect_thread_.join();
    }
}

bool ConnectionManager::connect(const ConnectionConfig& config) {
    try {
        config_ = config;
        
        std::string server_uri = (config.use_ssl ? "ssl://" : "tcp://") + 
                                config.broker_host + ":" + std::to_string(config.broker_port);
        
        client_ = std::make_unique<mqtt::async_client>(server_uri, config.client_id);
        callback_ = std::make_unique<MqttCallback>(this);
        client_->set_callback(*callback_);
        
        mqtt::connect_options connOpts;
        connOpts.set_keep_alive_interval(config.keep_alive_interval);
        connOpts.set_clean_session(config.clean_session);
        
        setState(ConnectionState::CONNECTING);
        
        auto token = client_->connect(connOpts);
        token->wait_for(std::chrono::seconds(config.connection_timeout));
        
        if (token->get_return_code() == mqtt::ReasonCode::SUCCESS) {
            setState(ConnectionState::CONNECTED);
            return true;
        } else {
            handleError("Failed to connect: return code " + std::to_string(static_cast<int>(token->get_return_code())));
            setState(ConnectionState::FAILED);
            return false;
        }
        
    } catch (const mqtt::exception& e) {
        handleError("MQTT exception: " + std::string(e.what()));
        setState(ConnectionState::FAILED);
        return false;
    } catch (const std::exception& e) {
        handleError("Exception: " + std::string(e.what()));
        setState(ConnectionState::FAILED);
        return false;
    }
}

void ConnectionManager::disconnect() {
    try {
        if (client_ && client_->is_connected()) {
            auto token = client_->disconnect();
            token->wait();
        }
        setState(ConnectionState::DISCONNECTED);
    } catch (const std::exception& e) {
        handleError("Error during disconnect: " + std::string(e.what()));
    }
    
    client_.reset();
    callback_.reset();
}

bool ConnectionManager::isConnected() const {
    return connection_state_ == ConnectionState::CONNECTED && 
           client_ && client_->is_connected();
}

ConnectionState ConnectionManager::getConnectionState() const {
    return connection_state_;
}

void ConnectionManager::setConnectionCallback(ConnectionCallback callback) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    connection_callback_ = std::move(callback);
}

void ConnectionManager::setErrorCallback(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    error_callback_ = std::move(callback);
}

mqtt::async_client* ConnectionManager::getClient() {
    return client_.get();
}

void ConnectionManager::setAutoReconnect(bool enable, int retry_interval) {
    auto_reconnect_ = enable;
    retry_interval_ = retry_interval;
    
    if (enable && !reconnect_thread_.joinable()) {
        should_stop_ = false;
        reconnect_thread_ = std::thread(&ConnectionManager::reconnectLoop, this);
    } else if (!enable && reconnect_thread_.joinable()) {
        should_stop_ = true;
        reconnect_thread_.join();
    }
}

std::string ConnectionManager::getLastError() const {
    std::lock_guard<std::mutex> lock(state_mutex_);
    return last_error_;
}

void ConnectionManager::setState(ConnectionState state) {
    connection_state_ = state;
    
    std::lock_guard<std::mutex> lock(state_mutex_);
    if (connection_callback_) {
        connection_callback_(state);
    }
}

void ConnectionManager::handleError(const std::string& error) {
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        last_error_ = error;
        
        if (error_callback_) {
            error_callback_(error);
        }
    }
}

void ConnectionManager::reconnectLoop() {
    while (!should_stop_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        if (should_stop_) break;
        
        if (auto_reconnect_ && 
            (connection_state_ == ConnectionState::DISCONNECTED || 
             connection_state_ == ConnectionState::FAILED)) {
            
            setState(ConnectionState::RECONNECTING);
            
            if (connect(config_)) {
                continue;
            }
            
            for (int i = 0; i < retry_interval_ && !should_stop_; ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
}

} // namespace mqtt_connector