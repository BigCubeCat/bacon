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

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Model *m_model;
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    EspItem *m_esp;

    QVBoxLayout *m_layout;

    void setupBasicScene();

    void clearScene();

    int m_zoomCounter = 0;

public slots:
    void beaconChanged();

    void espChanged();
};

#endif //APP_SCENE_HPP
