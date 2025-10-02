#include "mainwindow.hpp"

#include "ui_mainwindow.h"


MainWindow::MainWindow(Model *model, QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_model(model),
      m_beaconEditor(new BeaconEditor(model)),
      m_scene(new Scene(m_model)) {
    m_ui->setupUi(this);

    m_ui->tabWidget->removeTab(0);
    m_ui->tabWidget->removeTab(0);
    m_ui->tabWidget->addTab(m_beaconEditor, "Beacon Editor");
    m_ui->scrollArea->setWidget(m_scene);

    connect(m_beaconEditor, &BeaconEditor::accepted, m_model, &Model::beaconChanged);
    connect(m_model, &Model::signalBeaconsChanged, m_beaconEditor, &BeaconEditor::updateBeacons);
}

MainWindow::~MainWindow() {
    delete m_ui;
    delete m_beaconEditor;
    delete m_scene;
}
