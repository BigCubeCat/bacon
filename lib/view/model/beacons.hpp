#ifndef APP_BEACONS_HPP
#define APP_BEACONS_HPP
#include <QList>
#include "beacon.hpp"


class Beacons {
public:
    explicit Beacons() = default;

    explicit Beacons(QList<Beacon*> beacons);

    Beacon* beacon(int index);

    ~Beacons();

private:
    QList<Beacon*> m_beacons;
};


#endif //APP_BEACONS_HPP
