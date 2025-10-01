#ifndef APP_SCENE_HPP
#define APP_SCENE_HPP

#include <QMainWindow>
#include <QGraphicsView>
#include <QVBoxLayout>

#include "espitem.hpp"

class Scene : public QWidget {
    Q_OBJECT

public:
    explicit Scene(QWidget *parent = nullptr);
    ~Scene();

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    EspItem *m_esp;

    QTimer *m_timer;

    QVBoxLayout *m_layout;
};

#endif //APP_SCENE_HPP
