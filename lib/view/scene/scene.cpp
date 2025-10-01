#include "scene.hpp"
 #include <QTimer>
#include <cmath>
#include <QGraphicsView>
#include <QVBoxLayout>

#include "beaconitem.hpp"
#include "espitem.hpp"
#include "griditem.hpp"

Scene::Scene(QWidget *parent)
    : QWidget(parent),
      m_scene(new QGraphicsScene(this)),
      m_view(new QGraphicsView(m_scene, this)),
      m_esp(new EspItem) {
    // Размещение QGraphicsView во всём окне
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_view);
    setLayout(m_layout);

    // Настройки view
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setSceneRect(-500, -500, 1000, 1000);
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);

    // Добавляем элементы
    m_scene->addItem(new GridItem(50));                  // сетка
    m_scene->addItem(new BeaconItem(-200, -100));        // маяки
    m_scene->addItem(new BeaconItem(150,  150));
    m_scene->addItem(new BeaconItem(300, -200));

    // Приёмник и путь
    m_scene->addItem(m_esp->pathItem());
    m_scene->addItem(m_esp);
    m_esp->setPos(0, 0);

    // Тестовое движение приёмника
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [this]() {
        static double angle = 0;
        angle += 5;
        double r = 200.0;
        QPointF p(r * std::cos(qDegreesToRadians(angle)),
                  r * std::sin(qDegreesToRadians(angle)));
        m_esp->moveTo(p);
    });
    m_timer->start(200);
}

Scene::~Scene() {
    delete m_scene;
    delete m_view;
    delete m_timer;
    delete m_esp;
    delete m_layout;
}
