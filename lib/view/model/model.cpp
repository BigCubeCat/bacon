#include "model.hpp"

Model::Model() : m_beacons(), m_esp(QString("esp"), QPointF(10.0, 10.0)) {
}

Beacons Model::beacons() const {
    return m_beacons;
}

Beacon *Model::beacon(int index) const {
    return m_beacons.at(index);
}

void Model::setPosEsp(const QPointF &pos) {
    m_esp.setPos(pos);
}

void Model::moveEsp(const QPointF &pos) {
    m_esp.move(pos);
}

void Model::clearPath() {
    m_path.clear();
}

void Model::addPointToPath(const QPointF &pos) {
    m_path.append(pos);
}

QList<QPointF> Model::path() const {
    return m_path;
}

void Model::updateBeacon(int index, Beacon *beacon) {
    auto *old = m_beacons[index];
    m_beacons[index] = beacon;
    delete old;
}

void Model::addBeacon(Beacon *beacon) {
    m_beacons.append(beacon);
}
