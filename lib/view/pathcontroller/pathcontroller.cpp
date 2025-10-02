#include "pathcontroller.hpp"

#include <qwidget.h>
#include "model.hpp"
#include "ui_pathcontroller.h"
#include "utils.hpp"

PathController::PathController(Model* model, QWidget* parent)
    : QWidget(parent),
      m_ui(new Ui::PathController),
      m_model(model),
      m_list(new QStandardItemModel(0, 2, this)) {
    m_ui->setupUi(this);
    m_list->setHeaderData(0, Qt::Horizontal, "X");
    m_list->setHeaderData(1, Qt::Horizontal, "Y");
    m_ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_ui->acceptUrlBtn, &QPushButton::clicked, this,
            &PathController::onUrlAccepted);
    connect(m_ui->freqBtn, &QPushButton::clicked, this,
            &PathController::onFreqAccepted);
    connect(m_ui->resetBtn, &QPushButton::clicked, this,
            &PathController::resetPath);

    connect(m_ui->startBtn, &QPushButton::clicked, m_model, &Model::onStarted);
    connect(m_ui->stopBtn, &QPushButton::clicked, m_model, &Model::onStopped);
}

PathController::~PathController() {
    delete m_ui;
}

void PathController::setPath(const QList<QPointF>& path) {

    m_list->setRowCount(path.size());
    for (int row = 0; row < path.size(); ++row) {
        const QPointF& p = path[row];
        m_list->setItem(row, 0, new QStandardItem(QString::number(p.x())));
        m_list->setItem(row, 1, new QStandardItem(QString::number(p.y())));
    }

    m_ui->tableView->setModel(m_list);

    m_ui->tableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    m_ui->tableView->verticalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    m_ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void PathController::resetPath() {
    m_list->removeRows(0, m_list->rowCount());
    emit pathReseted();
}

void PathController::addPathPoint(const QPointF& pnt) {
    const auto row = m_list->rowCount();
    m_list->setRowCount(row + 1);
    m_list->setItem(row, 0, new QStandardItem(QString::number(pnt.x())));
    m_list->setItem(row, 1, new QStandardItem(QString::number(pnt.y())));
}

void PathController::onUrlAccepted() {
    auto url = m_ui->urlEdit->text();
    if (!isValidIPv4WithPort(url)) {
        m_ui->urlEdit->setText("127.0.0.1:1883");
        url = m_ui->urlEdit->text();
    };
    emit urlChanged(url);
}

void PathController::onFreqAccepted() {
    const float freq = m_ui->freqSpinBox->value();
    emit freqChanged(freq);
}
