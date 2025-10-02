#pragma once

#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include "message_objects/BLE.h"

namespace navigator {
    class Navigator {
    public:
        // Конструктор принимает список известных маяков и коэффициент сглаживания
        Navigator(const std::vector<message_objects::BLEBeacon> &knownBeacons,
                double alpha = 0.2);

        // Теперь сюда передаётся вектор {имя маяка, список состояний}
        std::pair<double, double> calculatePosition(
            const std::vector<std::pair<std::string, std::vector<message_objects::BLEBeaconState>>> &beaconMeasurements);

    private:
        // Список известных маяков
        std::vector<message_objects::BLEBeacon> knownBeacons_;

        // Параметр сглаживания для EMA
        double alpha_;

        // Карта "имя маяка → сглаженное значение расстояния"
        mutable std::unordered_map<std::string, double> emaMap_;

        // Конвертация RSSI → расстояние
        double rssiToDistance(int rssi, int txPower) const;

        double calculateMedian(std::vector<double> &values) const;

        double updateMovingAverage(const std::string &beaconName, double newValue);

        // Триангуляция по сглаженным расстояниям
        std::pair<double, double> trilateration(
            const std::vector<std::pair<message_objects::BLEBeacon, double>> &distances) const;
    };
}