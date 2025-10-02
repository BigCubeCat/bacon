#ifndef APP_SCENE_HPP
#define APP_SCENE_HPP

#include <QMainWindow>
#include <QGraphicsView>
#include <QVBoxLayout>

#include "espitem.hpp"
#include "model.hpp"

class Scene : public QWidget {
    Q_OBJECT

public:
    explicit Scene(Model *model, QWidget *parent = nullptr);

    ~Scene() override;

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    EspItem *m_esp;

    QTimer *m_timer;

    QVBoxLayout *m_layout;

public slots:
    void beaconChanged();
};

#endif //APP_SCENE_HPP
