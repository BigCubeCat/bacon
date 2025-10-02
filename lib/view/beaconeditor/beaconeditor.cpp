#include "beaconeditor.hpp"

#include "ui_beaconeditor.h"

BeaconEditor::BeaconEditor(Model *m, QWidget *parent) : QWidget(parent), m_ui(new Ui::BeaconEditor), m_model(m) {
    m_ui->setupUi(this);
    m_ui->plainTextEdit->setPlainText("Name;X;Y\n"
        "beacon_1;3.0;-2.4\n"
        "beacon_2;-2.4;-0.6\n"
        "beacon_3;1.8;9\n"
        "beacon_4;4.8;18.6\n"
        "beacon_5;-1.8;26.4\n"
        "beacon_6;-1.8;34.2\n"
        "beacon_7;7.8;34.2\n"
        "beacon_8;-1.8;40.8\n");

    connect(m_ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &BeaconEditor::updateText);
}

BeaconEditor::~BeaconEditor() {
    delete m_ui;
}

void BeaconEditor::parseBeacons(const QString &text) {
    QList<Beacon> beacons;
    std::istringstream iss(text.toStdString());
    std::string line;

    static auto parseOne = [](const std::string &line)-> std::optional<std::pair<QString, QPointF> > {
        std::istringstream lineStream(line);
        std::string token;

        // name
        if (!std::getline(lineStream, token, ';')) {
            return std::nullopt;
        }
        auto name = token;

        // X
        if (!std::getline(lineStream, token, ';')) {
            return std::nullopt;
        }
        float x, y;
        try {
            x = std::stof(token);
        } catch (const std::invalid_argument &e) {
            return std::nullopt;
        }

        // Y
        if (!std::getline(lineStream, token, ';')) {
            return std::nullopt;
        }
        try {
            y = std::stof(token);
        } catch (const std::invalid_argument &e) {
            return std::nullopt;
        }
        std::pair<QString, QPointF> result = {QString::fromStdString(token), {x, y}};
        return result;
    };

    // Пропускаем заголовок
    if (!std::getline(iss, line)) {
        m_ui->acceptButton->setEnabled(false);
    }

    while (std::getline(iss, line)) {
        if (line.empty()) continue;

        const auto one = parseOne(line);
        if (!one.has_value()) {
            m_ui->acceptButton->setEnabled(false);
            return;
        }
        const auto value = one.value();
        beacons.emplace_back(value.first, value.second, "");
    }
    m_beacons = beacons;
    m_ui->acceptButton->setEnabled(true);
}

void BeaconEditor::updateText() {
    const auto text = m_ui->plainTextEdit->toPlainText();
    parseBeacons(text);
    update();
}

void BeaconEditor::updateBeacons() {
    const auto beacons = m_model->beacons();
    QString res = "Name;X;Y\n";
    for (int i = 0; i < beacons.size(); ++i) {
        const auto beacon = beacons.at(i);
        const auto pos = beacon.pos();
        res += beacon.name() + ";" + QString::fromStdString(std::to_string(pos.x())) + ";" +
                QString::fromStdString(std::to_string(pos.y())) + "\n";
    }
    m_ui->plainTextEdit->setPlainText(res);
}

void BeaconEditor::setText(const QString &text) {
    parseBeacons(text);
    m_ui->plainTextEdit->setPlainText(text);
    update();
}
