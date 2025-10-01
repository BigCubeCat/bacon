#include "beaconeditor.hpp"

#include "ui_beaconeditor.h"

BeaconEditor::BeaconEditor(QWidget *parent) : QWidget(parent), m_ui(new Ui::BeaconEditor) {
    m_ui->setupUi(this);
}

BeaconEditor::~BeaconEditor() {
    delete m_ui;
}
