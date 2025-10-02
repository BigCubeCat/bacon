#ifndef APP_BEACONEDITOR_HPP
#define APP_BEACONEDITOR_HPP

#include <QWidget>

#include "beacon.hpp"


QT_BEGIN_NAMESPACE

namespace Ui {
    class BeaconEditor;
}

QT_END_NAMESPACE


class BeaconEditor : public QWidget {
    Q_OBJECT

public:
    explicit BeaconEditor(QWidget *parent = nullptr);

    ~BeaconEditor() override;

private:
    Ui::BeaconEditor *m_ui;

    QList<Beacon *> m_beacons;
    void parseBeacons(const QString &text);

public slots:
    void setText(const QString &text);
    void updateText();

signals:
    void accepted(const QList<Beacon*> &beacons);
};


#endif //APP_BEACONEDITOR_HPP
