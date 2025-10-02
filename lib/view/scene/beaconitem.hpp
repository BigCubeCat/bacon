#ifndef APP_BEACONITEM_HPP
#define APP_BEACONITEM_HPP

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGuiApplication>
#include <QBrush>
#include <QString>

class BeaconItem : public QGraphicsEllipseItem {
public:
    BeaconItem(const QString &name, qreal x, qreal y, qreal radius = 10.0)
        : QGraphicsEllipseItem(-radius, -radius, radius * 2, radius * 2),
          name_(name)
    {
        setPos(x, y);
        setBrush(QBrush(Qt::blue));
        updateToolTip();
    }

    void setName(const QString &name) {
        name_ = name;
        updateToolTip();
    }

    QString name() const { return name_; }

private:
    QString name_;

    void updateToolTip() {
        setToolTip(QString("%1 at (%2, %3)")
                       .arg(name_)
                       .arg(pos().x())
                       .arg(pos().y()));
    }
};



#endif //APP_BEACONITEM_HPP
