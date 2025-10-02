#include "navigator/navigator.h"

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <Eigen/Dense>

using namespace message_objects;

namespace navigator {

    Navigator(const std::vector<BLEBeacon> &knownBeacons, double alpha = 0.2):
        knownBeacons_(const_cast<std::vector<BLEBeacon>&>(knownBeacons)), alpha_(alpha) {}

    double Navigator::rssiToDistance(int rssi, int txPower) const {
        constexpr double n = 3.0; // коэффициент затухания
        return std::pow(10.0, (txPower - rssi) / (10.0 * n));
    }

    std::pair<double, double> Navigator::calculatePosition(
        const std::vector<std::pair<std::string, std::vector<BLEBeaconState>>> &beaconMeasurements)
    {
        // Собираем медианные расстояния до известных маяков
        std::vector<std::pair<BLEBeacon, double>> distances;

        for (const auto &[beaconName, measurements] : beaconMeasurements) {
            // Находим соответствующий известный маяк
            auto it = std::find_if(knownBeacons_.begin(), knownBeacons_.end(),
                                    [&beaconName](const BLEBeacon &b) {
                                        return b.name_ == beaconName;
                                    });

            if (it != knownBeacons_.end()) {
                // Рассчитываем дистанции для всех измерений
                std::vector<double> measuredDistances;
                for (const auto &measurement : measurements) {
                    double d = rssiToDistance(measurement.rssi_, measurement.txPower_);
                    measuredDistances.push_back(d);
                }

                // Вычисляем медиану дистанций
                double medianDistance = calculateMedian(measuredDistances);

                // Обновляем скользящее среднее и получаем сглаженное значение
                double smoothedDistance = updateMovingAverage(beaconName, medianDistance);

                distances.emplace_back(*it, smoothedDistance);
            }
        }

        if (distances.size() < 3) {
            throw std::runtime_error("Недостаточно маяков для триангуляции (нужно минимум 3).");
        }

        return trilateration(distances);
    }

    double Navigator::calculateMedian(std::vector<double> &values) const {
        if (values.empty()) {
            throw std::runtime_error("Cannot calculate median of empty vector");
        }

        size_t n = values.size() / 2;
        std::nth_element(values.begin(), values.begin() + n, values.end());

        if (values.size() % 2 == 1) {
            // Нечетное количество элементов
            return values[n];
        } else {
            // Четное количество элементов - берем среднее двух центральных
            double median1 = values[n];
            std::nth_element(values.begin(), values.begin() + n - 1, values.end());
            double median2 = values[n - 1];
            return (median1 + median2) / 2.0;
        }
    }

    double Navigator::updateMovingAverage(const std::string &beaconName, double newValue) {
        // Ищем существующую запись
        auto it = emaMap_.find(beaconName);

        if (it == emaMap_.end()) {
            // Первое значение для этого маяка
            emaMap_[beaconName] = newValue;
            return newValue;
        } else {
            // Обновляем скользящее среднее
            double &currentAverage = it->second;
            currentAverage = alpha_ * newValue + (1 - alpha_) * currentAverage;
            return currentAverage;
        }
    }

    std::pair<double, double> Navigator::trilateration(
        const std::vector<std::pair<BLEBeacon, double>> &distances) const
    {
        const auto &[b0, r0] = distances[0];

        // Теперь система в 3D -> матрица A имеет 3 столбца (x, y, z)
        Eigen::MatrixXd A(distances.size() - 1, 3);
        Eigen::VectorXd b(distances.size() - 1);

        for (size_t i = 1; i < distances.size(); ++i) {
            const auto &[bi, ri] = distances[i];

            A(i - 1, 0) = 2 * (bi.x_ - b0.x_);
            A(i - 1, 1) = 2 * (bi.y_ - b0.y_);
            A(i - 1, 2) = 0; // так как z_i = z0 = 0 у всех маяков

            b(i - 1) = r0*r0 - ri*ri - b0.x_*b0.x_ + bi.x_*bi.x_
                        - b0.y_*b0.y_ + bi.y_*bi.y_;
        }

        // Проверка вырожденности (условное число)
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(A);
        if (svd.singularValues().minCoeff() < 1e-8) {
            throw std::runtime_error("Маяки почти коллинеарны, триангуляция нестабильна.");
        }

        // Решение методом наименьших квадратов
        Eigen::Vector3d position = (A.transpose() * A).ldlt().solve(A.transpose() * b);

        // Возвращаем только x, y
        return {position(0), position(1)};
    }

    // Метод для установки коэффициента сглаживания
    void Navigator::setSmoothingFactor(double alpha) {
        if (alpha < 0.0 || alpha > 1.0) {
            throw std::invalid_argument("Smoothing factor must be between 0 and 1");
        }
        alpha_ = alpha;
    }

    // Метод для сброса скользящих средних
    void Navigator::resetMovingAverages() {
        movingAverages_.clear();
    }

} // namespace navigator
