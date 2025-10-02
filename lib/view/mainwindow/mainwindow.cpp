#include "mainwindow.hpp"
#include <qpoint.h>
#include <qtimer.h>

#include "ui_mainwindow.h"

#include <QObject>

MainWindow::MainWindow(Model* model, QWidget* parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_model(model),
      m_beaconEditor(new BeaconEditor(model)),
      m_pathController(new PathController(model)),
      m_scene(new Scene(m_model)) {
    m_ui->setupUi(this);

    m_ui->tabWidget->removeTab(0);
    m_ui->tabWidget->removeTab(0);
    m_ui->tabWidget->addTab(m_pathController, "Controller");
    m_ui->tabWidget->addTab(m_beaconEditor, "Beacon Editor");
    m_ui->verticalLayout->addWidget(m_scene);

    connect(m_beaconEditor, &BeaconEditor::accepted, m_model,
            &Model::beaconChanged);
    connect(m_model, &Model::signalBeaconsChanged, m_beaconEditor,
            &BeaconEditor::updateBeacons);
    connect(m_model, &Model::signalBeaconsChanged, m_scene,
            &Scene::beaconChanged);

    connect(m_model, &Model::dataChanged, m_scene, &Scene::espChanged);
    connect(m_model, &Model::pointAddedSignal, m_pathController,
            &PathController::addPathPoint);

    setWindowIcon(QIcon(":/assets/assets/icon.png"));
    m_ui->actionOpen_beacon->setIcon(QIcon(":/assets/assets/open.png"));
    m_ui->actionSave_beacon->setIcon(QIcon(":/assets/assets/save.png"));
    m_ui->actionSave_Path->setIcon(QIcon(":/assets/assets/save.png"));
    m_ui->actionOpen_path->setIcon(QIcon(":/assets/assets/open.png"));
}

MainWindow::~MainWindow() {
    delete m_ui;
    delete m_beaconEditor;
    delete m_scene;
}
