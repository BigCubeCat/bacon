#ifndef APP_ESPITEM_HPP
#define APP_ESPITEM_HPP

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QVector>

class EspItem : public QGraphicsEllipseItem {
public:
    EspItem(qreal radius = 8.0)
        : QGraphicsEllipseItem(-radius, -radius, radius * 2, radius * 2) {
        setBrush(Qt::red);
        setZValue(10);
    }

    void moveTo(const QPointF &p) {
        setPos(p);
        m_path << p;
        updatePath();
    }

    QGraphicsPathItem* pathItem() { return &m_pathItem; }

private:
    void updatePath() {
        QPainterPath path;
        if (m_path.isEmpty()) return;
        path.moveTo(m_path[0]);
        for (int i = 1; i < m_path.size(); ++i)
            path.lineTo(m_path[i]);

        m_pathItem.setPath(path);
        m_pathItem.setPen(QPen(Qt::darkRed, 2));
        m_pathItem.setZValue(5);
    }

    QVector<QPointF> m_path;
    QGraphicsPathItem m_pathItem;
};

#endif //APP_ESPITEM_HPP