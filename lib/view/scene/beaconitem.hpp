#ifndef APP_BEACONITEM_HPP
#define APP_BEACONITEM_HPP

#include <QGraphicsEllipseItem>
#include <QBrush>
#include <QGraphicsSceneMouseEvent>

class BeaconItem : public QGraphicsEllipseItem {
public:
    BeaconItem(qreal x, qreal y, qreal radius = 10.0)
        : QGraphicsEllipseItem(-radius, -radius, radius * 2, radius * 2) {
        setPos(x, y);
        setBrush(QBrush(Qt::blue));
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
        setToolTip(QString("Beacon at (%1, %2)").arg(x).arg(y));
    }
};

#endif //APP_BEACONITEM_HPP