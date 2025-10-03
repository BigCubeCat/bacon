#include "navigator/navigator.h"
#include <Eigen/Dense>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <iostream>
#include <unordered_map>

using namespace message_objects;

namespace navigator {

void Navigator::setKnownBeacons(
    std::vector<message_objects::BLEBeacon> newBeacons) {
    knownBeacons_ = newBeacons;
}

// --- конструктор ---
Navigator::Navigator(const std::vector<BLEBeacon>& knownBeacons, double alpha,
                     double positionAlpha)
    : knownBeacons_(knownBeacons),
      alpha_(alpha),
      positionAlpha_(positionAlpha) {}

// --- calculatePosition ---
std::pair<double, double> Navigator::calculatePosition(
    std::vector<std::pair<std::string, std::vector<BLEBeaconState>>>&
        beaconMeasurements) {
    std::vector<std::pair<BLEBeacon, double>> distances;

    for (const auto& [beaconName, measurements] : beaconMeasurements) {
        auto it = std::find_if(knownBeacons_.begin(), knownBeacons_.end(),
                               [&beaconName](const BLEBeacon& b) {
                                   return b.name_ == beaconName;
                               });
        if (it == knownBeacons_.end())
            continue;

        std::vector<double> measuredDistances;
        for (const auto& measurement : measurements) {
            double d = rssiToDistance(measurement.rssi_, measurement.txPower_);
            std::cout << "name:" << beaconName << " " << "dist: " << d << std::endl;
            if (d <= 30.0)
                measuredDistances.push_back(d);  // отбрасываем шум
        }
        if (measuredDistances.empty())
            continue;

        double filteredDistance = calculateMedian(measuredDistances);
        double smoothedDistance =
            updateMovingAverage(beaconName, filteredDistance);

        // ограничение скачка
        constexpr double maxJump = 5.0;
        auto prevIt = emaMap_.find(beaconName);
        if (prevIt != emaMap_.end() &&
            std::abs(smoothedDistance - prevIt->second) > maxJump) {
            smoothedDistance = prevIt->second;
        }

        distances.emplace_back(*it, smoothedDistance);
    }

    if (distances.size() < 3)
        throw std::runtime_error("Недостаточно маяков для триангуляции.");

    auto rawPos = trilateration(distances);

    return applyPositionEMA(rawPos);
}

// --- calculateMedian с IQR ---
double Navigator::calculateMedian(std::vector<double>& values) const {
    if (values.empty())
        throw std::runtime_error("Empty vector for median");

    std::sort(values.begin(), values.end());
    double q1 = values[values.size() / 4];
    double q3 = values[3 * values.size() / 4];
    double iqr = q3 - q1;

    std::vector<double> filtered;
    for (double v : values) {
        if (v >= q1 - 1.5 * iqr && v <= q3 + 1.5 * iqr)
            filtered.push_back(v);
    }
    if (filtered.empty())
        filtered = values;

    size_t n = filtered.size() / 2;
    if (filtered.size() % 2 == 1)
        return filtered[n];
    else
        return (filtered[n - 1] + filtered[n]) / 2.0;
}

// --- updateMovingAverage ---
double Navigator::updateMovingAverage(const std::string& beaconName,
                                      double newValue) {
    auto it = emaMap_.find(beaconName);
    if (it == emaMap_.end()) {
        emaMap_[beaconName] = newValue;
        return newValue;
    } else {
        double& current = it->second;
        current = alpha_ * newValue + (1 - alpha_) * current;
        return current;
    }
}

// --- RSSI → расстояние (переписано) ---
double Navigator::rssiToDistance(int rssi, int txPower) const {
    constexpr double n = 3.3;  // indoor, меньше чем 3
    double distance = std::pow(10.0, (-52 - rssi) / (10.0 * n));
    return std::max(distance, 0.5);  // минимальное расстояние 0.5 м
}

// --- EMA координат ---
std::pair<double, double> Navigator::applyPositionEMA(
    const std::pair<double, double>& newPos) const {
    if (!lastPositionInitialized_) {
        lastPosition_ = newPos;
        lastPositionInitialized_ = true;
        return newPos;
    }
    double alpha = 0.65;  // сильнее учитываем новое измерение
    lastPosition_.first =
        alpha * newPos.first + (1 - alpha) * lastPosition_.first;
    lastPosition_.second =
        alpha * newPos.second + (1 - alpha) * lastPosition_.second;
    return lastPosition_;
}

// --- Триангуляция с равными весами ---
std::pair<double, double> Navigator::trilateration(
    std::vector<std::pair<BLEBeacon, double>>& distances) const {
    if (distances.size() < 3)
        throw std::runtime_error(
            "Недостаточно маяков для триангуляции (нужно минимум 3).");

    // Старт: центр масс маяков
    double x = 0, y = 0;
    for (auto& d : distances) {
        x += d.first.x_;
        y += d.first.y_;
    }
    x /= distances.size();
    y /= distances.size();

    // Градиентный спуск
    int maxIter = 200;
    double lr = 0.2;
    double tol = 1e-4;

    for (int iter = 0; iter < maxIter; ++iter) {
        double gx = 0, gy = 0;

        for (auto& d : distances) {
            double dx = x - d.first.x_;
            double dy = y - d.first.y_;
            double dist = std::sqrt(dx * dx + dy * dy) + 1e-9;
            double err = dist - d.second;

            // Равные веса
            gx += err * dx / dist;
            gy += err * dy / dist;
        }

        gx /= distances.size();
        gy /= distances.size();

        x -= lr * gx;
        y -= lr * gy;

        if (std::sqrt((lr * gx) * (lr * gx) + (lr * gy) * (lr * gy)) < tol)
            break;
    }

    return {x, y};
}

}  // namespace navigator
