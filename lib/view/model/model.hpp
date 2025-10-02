#ifndef APP_MODEL_HPP
#define APP_MODEL_HPP
#include <QList>
#include <QObject>

#include "beacon.hpp"
#include "espobject.hpp"


using Beacons = QList<Beacon>;

class Model : public QObject {
    Q_OBJECT

public:
    explicit Model();

    [[nodiscard]] Beacons beacons() const;

    [[nodiscard]] Beacon beacon(int index) const;

    void setPosEsp(const QPointF &pos);

    void moveEsp(const QPointF &pos);

    void clearPath();

    void addPointToPath(const QPointF &pos);

    [[nodiscard]] QList<QPointF> path() const;

    void updateBeacon(int index, Beacon beacon);

    void addBeacon(const Beacon &beacon);

signals:
    void dataChanged();

    void pathChanged();

    void pathAdded();

    void oneBeaconChanged(int index);

    void signalBeaconsChanged();

private:
    Beacons m_beacons;
    EspObject m_esp;
    QList<QPointF> m_path;

public slots:
    void beaconChanged(const QList<Beacon> &beacons);
};


#endif //APP_MODEL_HPP
