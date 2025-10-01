#ifndef APP_BEACONEDITOR_HPP
#define APP_BEACONEDITOR_HPP

#include <QWidget>


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
};


#endif //APP_BEACONEDITOR_HPP
