#include "mainwindow.hpp"

#include "ui_mainwindow.h"


MainWindow::MainWindow(std::shared_ptr<Model> model, QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_model(model),
      m_beaconEditor(new BeaconEditor()),
      m_scene(new Scene(m_model)) {
    m_ui->setupUi(this);

    m_ui->tabWidget->removeTab(0);
    m_ui->tabWidget->removeTab(0);
    // m_ui->area->addTab(m_scene, "Scene");
}

MainWindow::~MainWindow() {
    delete m_ui;
    delete m_beaconEditor;
    delete m_scene;
}
