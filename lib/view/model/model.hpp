#ifndef APP_MODEL_HPP
#define APP_MODEL_HPP
#include "beacons.hpp"
#include "espobject.hpp"


class Model {
public:
    explicit Model();

    Beacons beacons() const;

    Beacon *beacon(int index);

    void setPosEsp(const QPointF &pos);

    void moveEsp(const QPointF &pos);

    void clearPath();

    void addPointToPath(const QPointF &pos);

    QList<QPointF> path() const;

signals:
    void dataChanged();

private:
    Beacons m_beacons;
    EspObject m_esp;
    QList<QPointF> m_path;

};


#endif //APP_MODEL_HPP
