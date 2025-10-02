#ifndef APP_BEACONITEM_HPP
#define APP_BEACONITEM_HPP

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>
#include <QString>

class BeaconItem : public QGraphicsEllipseItem {
public:
    BeaconItem(const QString &name, qreal x, qreal y, qreal radius = 10.0)
        : QGraphicsEllipseItem(-radius, -radius, radius * 2, radius * 2),
          m_name(name), m_radius(radius) {
        setPos(x, y);
        setPen(Qt::NoPen);
        setBrush(QBrush(kSecondaryColor[0]));
        updateToolTip();
        setZValue(7);

        const auto msec = 70;
        const auto countWaves = 3;
        const auto r = radius * 7;
        for (int i = 0; i < countWaves; i++) {
            auto *waveItem = new WaveItem(
                radius,
                r,
                kSecondaryColor[0],
                kSecondaryColor[1],
                this,
                msec,
                r * i * msec / countWaves
            );
            waveItem->setZValue(8);
            m_waves.push_back(waveItem);
        }
    }

    void setName(const QString &name) {
        m_name = name;
        updateToolTip();
    }

    QString name() const { return m_name; }

private:
    QString m_name;
    QList<WaveItem *> m_waves;
    qreal m_radius;

    void updateToolTip() {
        setToolTip(QString("%1 at (%2, %3)")
            .arg(m_name)
            .arg(pos().x())
            .arg(pos().y()));
    }
};


#endif //APP_BEACONITEM_HPP
