#pragma once

#include <vector>
#include <string>
#include <utility>

#include "message_objects/BLE.h"

namespace navigator {
class Navigator {
public:
    Navigator(const std::vector<message_objects::BLEBeacon> &knownBeacons);

    std::pair<double, double> calculatePosition(
        const std::vector<message_objects::BLEBeaconState> &beaconStates) const;

private:
    std::vector<message_objects::BLEBeacon> &knownBeacons_;

    double rssiToDistance(int rssi, int txPower) const;

    std::pair<double, double> trilateration(
        const std::vector<std::pair<message_objects::BLEBeacon, double>> &distances) const;
};
} // namespace navigator
