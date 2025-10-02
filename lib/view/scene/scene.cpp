#include "scene.hpp"
#include <qcolor.h>
#include <qevent.h>
#include <qpoint.h>
#include <QGraphicsView>
#include <QTimer>
#include <QVBoxLayout>
#include <cmath>

#include "beaconitem.hpp"
#include "espitem.hpp"
#include "griditem.hpp"

constexpr float CELL_SIZE = 30.0f;
constexpr float COUNT_CELLS = 100;
constexpr int MAX_ZOOM = 12;

Scene::Scene(Model* model, QWidget* parent)
    : QWidget(parent),
      m_model(model),
      m_scene(new QGraphicsScene(this)),
      m_view(new QGraphicsView(m_scene, this)) {
    // Размещение QGraphicsView во всём окне
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_view);
    setLayout(m_layout);

    m_view->setDragMode(QGraphicsView::ScrollHandDrag);

    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setSceneRect(
        -COUNT_CELLS / 2 * CELL_SIZE, -COUNT_CELLS / 2 * CELL_SIZE,
        COUNT_CELLS * 2 * CELL_SIZE, COUNT_CELLS * 2 * CELL_SIZE);

    setupBasicScene();
}

Scene::~Scene() {
    delete m_view;
    delete m_esp;
    delete m_layout;
}

void Scene::keyPressEvent(QKeyEvent* event) {
    QWidget::keyPressEvent(event);
    if (event->key() == Qt::Key_Plus) {
        if (m_zoomCounter < MAX_ZOOM) {
            m_view->scale(1.1, 1.1);
            m_zoomCounter++;
        }
    } else if (event->key() == Qt::Key_Minus) {
        if (-m_zoomCounter < MAX_ZOOM) {
            m_view->scale(0.9, 0.9);
            m_zoomCounter--;
        }
    }
}

void Scene::setupBasicScene() {
    clearScene();
    m_scene->addItem(new GridItem(CELL_SIZE));  // сетка
    m_esp = new EspItem(10);
    m_scene->addItem(m_esp);
    m_esp->setPos(0, 0);
    m_pathItems = new QGraphicsPathItem();
    m_scene->addItem(m_pathItems);
    m_pathItems->setPen(QPen(QColor(200, 32, 200), 2));
}

void Scene::clearScene() {
    m_scene->clear();
}

void Scene::beaconChanged() {
    setupBasicScene();
    const auto beacons = m_model->beacons();
    for (const auto& beacon : beacons) {
        const auto pos = beacon.pos();
        m_scene->addItem(new BeaconItem(beacon.name(), pos.x() * CELL_SIZE,
                                        -pos.y() * CELL_SIZE));
    }
    update();
}

void Scene::espChanged() {
    const auto eo = m_model->esp();
    const auto pos = eo.pos();
    std::cout << "espCahnged: " << pos.x() << " " << pos.y() << "\n";
    m_esp->setPos(pos.x() * CELL_SIZE, pos.y() * CELL_SIZE);
    const auto path = m_model->path();
    auto p = m_pathItems->path();
    if (path.isEmpty())
        p.moveTo(QPointF(pos.x() * CELL_SIZE, pos.y() * CELL_SIZE));
    else
        p.lineTo(QPointF(pos.x() * CELL_SIZE, pos.y() * CELL_SIZE));
    m_pathItems->setPath(p);
    update();
}
