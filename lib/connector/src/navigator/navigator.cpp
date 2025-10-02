#include "Navigator.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <Eigen/Dense>

Navigator::Navigator(const std::vector<BLEBeacon> &knownBeacons)
    : knownBeacons_(const_cast<std::vector<BLEBeacon>&>(knownBeacons)) {}

double Navigator::rssiToDistance(int rssi, int txPower) const {
    constexpr double n = 3.0; // коэффициент затухания
    return std::pow(10.0, (txPower - rssi) / (10.0 * n));
}

std::pair<double, double> Navigator::calculatePosition(
    const std::vector<BLEBeaconState> &beaconStates) const
{
    // Собираем расстояния до известных маяков
    std::vector<std::pair<BLEBeacon, double>> distances;
    for (const auto &state : beaconStates) {
        auto it = std::find_if(knownBeacons_.begin(), knownBeacons_.end(),
                               [&state](const BLEBeacon &b) { return b.name_ == state.name; });
        if (it != knownBeacons_.end()) {
            double d = rssiToDistance(state.rssi_, state.txPower_);
            distances.emplace_back(*it, d);
        }
    }

    if (distances.size() < 3) {
        throw std::runtime_error("Недостаточно маяков для триангуляции (нужно минимум 3).");
    }

    return trilateration(distances);
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
