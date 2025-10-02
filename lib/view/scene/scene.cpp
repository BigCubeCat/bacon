#include "scene.hpp"
#include <QTimer>
#include <cmath>
#include <QGraphicsView>
#include <QVBoxLayout>

#include "beaconitem.hpp"
#include "espitem.hpp"
#include "griditem.hpp"

Scene::Scene(Model *model,
             QWidget *parent
)
    : QWidget(parent),
      m_model(model),
      m_scene(new QGraphicsScene(this)),
      m_view(new QGraphicsView(m_scene, this)),
      m_esp() {
    // Размещение QGraphicsView во всём окне
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_view);
    setLayout(m_layout);

    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setSceneRect(-500, -500, 1000, 1000);

    setupBasicScene();
}

Scene::~Scene() {
    delete m_view;
    delete m_esp;
    delete m_layout;
}

void Scene::setupBasicScene() {
    clearScene();
    m_scene->addItem(new GridItem(60)); // сетка
    m_esp = new EspItem();
    m_scene->addItem(m_esp->pathItem());
    m_scene->addItem(m_esp);
    m_esp->setPos(0, 0);
}

void Scene::clearScene() {
    m_scene->clear();
}

void Scene::beaconChanged() {
    setupBasicScene();
    const auto beacons = m_model->beacons();
    for (const auto &beacon: beacons) {
        const auto pos = beacon.pos();
        m_scene->addItem(new BeaconItem(beacon.name(), pos.x(), pos.y()));
    }
    update();
}

void Scene::espChanged() {
    if (!m_esp) {
        m_esp = new EspItem();
    }
    const auto eo = m_model->esp();
    const auto pos = eo.pos();
    m_esp->setPos(pos.x(), pos.y());
}
