#ifndef APP_ESPITEM_HPP
#define APP_ESPITEM_HPP

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QVector>

class EspItem : public QGraphicsEllipseItem {
   public:
    EspItem(qreal radius = 8.0)
        : QGraphicsEllipseItem(-radius, -radius, radius * 2, radius * 2) {
        setBrush(QColor(200, 32, 200));
        setZValue(10);
    }

    void moveTo(const QPointF& p) { setPos(p); }

   private:
};

#endif  //APP_ESPITEM_HPP
