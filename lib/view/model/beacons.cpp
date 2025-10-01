#include "beacons.hpp"

Beacons::Beacons(QList<Beacon *> beacons) : m_beacons(std::move(beacons)) {
}

Beacon * Beacons::beacon(int index) {
    return m_beacons.at(index);
}

Beacons::~Beacons() {
    for (int i = 0; i < m_beacons.size(); ++i) {
        delete m_beacons[i];
    }
}
