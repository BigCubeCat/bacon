#ifndef APP_MODEL_HPP
#define APP_MODEL_HPP
#include <QList>
#include <QObject>

#include "beacon.hpp"
#include "espobject.hpp"

class Model : public QObject {
    Q_OBJECT

   public:
    explicit Model();

    [[nodiscard]] QList<Beacon> beacons() const;

    [[nodiscard]] Beacon beacon(int index) const;

    [[nodiscard]] EspObject esp() const;

    void setPosEsp(const QPointF& pos);

    void moveEsp(const QPointF& pos);

    void clearPath();

    void addPointToPath(const QPointF& pos);

    [[nodiscard]] QList<QPointF> path() const;

    void updateBeacon(int index, const Beacon& beacon);

    void addBeacon(const Beacon& beacon);

   signals:
    void dataChanged();

    void pathChanged();

    void oneBeaconChanged(int index);

    void signalBeaconsChanged();

    void freqChanged(float f);

    void urlChanged(const QString& url);

   private:
    QList<Beacon> m_beacons;
    EspObject m_esp;
    QList<QPointF> m_path;
    QString m_url;
    float m_freq;

    bool m_running = true;

   public slots:
    void beaconChanged(const QList<Beacon>& beacons);
    void pointAdded(const QPointF& point);
    void onChangeFreq(float freq);
    void onUrlChanged(const QString& url);

    void onStopped();
    void onStarted();
};

#endif  //APP_MODEL_HPP
