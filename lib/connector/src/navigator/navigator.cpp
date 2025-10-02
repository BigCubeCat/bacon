#include "navigator/navigator.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <Eigen/Dense>

using namespace message_objects;

namespace navigator {

    Navigator::Navigator(const std::vector<BLEBeacon> &knownBeacons, double alpha)
        : knownBeacons_(knownBeacons), alpha_(alpha) {}

    double Navigator::rssiToDistance(int rssi, int txPower) const {
        constexpr double n = 3.0; // коэффициент затухания
        return std::pow(10.0, (txPower - rssi) / (10.0 * n));
    }

    std::pair<double, double> Navigator::calculatePosition(
        const std::vector<std::pair<std::string, std::vector<BLEBeaconState>>> &beaconMeasurements)
    {
        std::vector<std::pair<BLEBeacon, double>> distances;

        for (const auto &[beaconName, measurements] : beaconMeasurements) {
            auto it = std::find_if(knownBeacons_.begin(), knownBeacons_.end(),
                                   [&beaconName](const BLEBeacon &b) {
                                       return b.name_ == beaconName;
                                   });

            if (it != knownBeacons_.end()) {
                std::vector<double> measuredDistances;
                for (const auto &measurement : measurements) {
                    double d = rssiToDistance(measurement.rssi_, measurement.txPower_);
                    measuredDistances.push_back(d);
                }

                double medianDistance = calculateMedian(measuredDistances);
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
            return values[n];
        } else {
            double median1 = values[n];
            std::nth_element(values.begin(), values.begin() + n - 1, values.end());
            double median2 = values[n - 1];
            return (median1 + median2) / 2.0;
        }
    }

    double Navigator::updateMovingAverage(const std::string &beaconName, double newValue) {
        auto it = emaMap_.find(beaconName);

        if (it == emaMap_.end()) {
            emaMap_[beaconName] = newValue;
            return newValue;
        } else {
            double &currentAverage = it->second;
            currentAverage = alpha_ * newValue + (1 - alpha_) * currentAverage;
            return currentAverage;
        }
    }

    std::pair<double, double> Navigator::trilateration(
        const std::vector<std::pair<BLEBeacon, double>> &distances) const
    {
        const auto &[b0, r0] = distances[0];

        Eigen::MatrixXd A(distances.size() - 1, 3);
        Eigen::VectorXd b(distances.size() - 1);

        for (size_t i = 1; i < distances.size(); ++i) {
            const auto &[bi, ri] = distances[i];

            A(i - 1, 0) = 2 * (bi.x_ - b0.x_);
            A(i - 1, 1) = 2 * (bi.y_ - b0.y_);
            A(i - 1, 2) = 0; // все маяки на плоскости z=0

            b(i - 1) = r0*r0 - ri*ri - b0.x_*b0.x_ + bi.x_*bi.x_
                     - b0.y_*b0.y_ + bi.y_*bi.y_;
        }

        Eigen::JacobiSVD<Eigen::MatrixXd> svd(A);
        if (svd.singularValues().minCoeff() < 1e-8) {
            throw std::runtime_error("Маяки почти коллинеарны, триангуляция нестабильна.");
        }

        Eigen::Vector3d position = (A.transpose() * A).ldlt().solve(A.transpose() * b);

        return {position(0), position(1)};
    }

} // namespace navigator
