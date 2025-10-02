#ifndef APP_ESPITEM_HPP
#define APP_ESPITEM_HPP

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QVector>

#include "const.hpp"
#include "waveitem.hpp"

class EspItem : public QGraphicsEllipseItem {
public:
    explicit EspItem(qreal radius = 8.0, qreal wave = 4, int countWaves = 3)
        : QGraphicsEllipseItem(-radius, -radius, radius * 2, radius * 2), m_radius(radius * wave) {
        setBrush(kPrimaryColor[0]);
        setPen(Qt::NoPen);
        setZValue(10);

        const auto msec = 100;
        for (int i = 0; i < countWaves; i++) {
            auto *waveItem = new WaveItem(
                radius,
                m_radius,
                kPrimaryColor[0],
                kPrimaryColor[2],
                this,
                msec,
                m_radius * i * msec / countWaves
            );
            waveItem->setZValue(9);
            m_waves.push_back(waveItem);
        }
    }

    ~EspItem() override {
        for (int i = 0; i < m_waves.size(); i++) {
            delete m_waves[i];
        }
    }

    QRectF boundingRect() const override {
        return QRectF(-m_radius, -m_radius,
                      m_radius * 2, m_radius * 2);
    }

    void moveTo(const QPointF &p) { setPos(p); }

private:
    QList<WaveItem *> m_waves;
    qreal m_radius;
};

#endif  //APP_ESPITEM_HPP
