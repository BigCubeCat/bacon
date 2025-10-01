#ifndef APP_MAINWINDOW_HPP
#define APP_MAINWINDOW_HPP

#include <QMainWindow>

#include "beaconeditor.hpp"
#include "scene.hpp"

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    Ui::MainWindow *m_ui;

    BeaconEditor *m_beaconEditor;
    Scene *m_scene;
};

#endif //APP_MAINWINDOW_HPP
