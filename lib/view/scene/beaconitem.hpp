#ifndef APP_BEACONITEM_HPP
#define APP_BEACONITEM_HPP

#include <QGraphicsEllipseItem>
#include <QBrush>
#include <QGraphicsSceneMouseEvent>
#include <qguiapplication.h>

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

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        // При нажатии мыши — стандартное поведение
        QGraphicsEllipseItem::mousePressEvent(event);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        // Разрешаем двигать только если зажат Ctrl
        if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier) {
            QGraphicsEllipseItem::mouseMoveEvent(event);
        } else {
            // Игнорируем перемещение без Ctrl
            event->ignore();
        }
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override {
        QGraphicsEllipseItem::mouseReleaseEvent(event);

        // Если Ctrl не нажат — округляем координаты
        if (!(QGuiApplication::keyboardModifiers() & Qt::ControlModifier)) {
            const auto p = pos();
            setPos(QPointF(std::round(p.x()), std::round(p.y())));
        }
    }
};

#endif //APP_BEACONITEM_HPP
