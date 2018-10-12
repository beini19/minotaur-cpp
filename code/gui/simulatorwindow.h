#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QDialog>
#include <memory>

class MainWindow;
class RenderScene;
class Simulator;

namespace Ui {
    class SimulatorWindow;
}

class SimulatorWindow : public QDialog {
Q_OBJECT

public:
    explicit SimulatorWindow(std::shared_ptr<Simulator> simulator, QWidget *parent = nullptr);

    void setVisible(bool visible) override;

    void keyPressEvent(QKeyEvent *) override;

    void keyReleaseEvent(QKeyEvent *) override;

    void reject() override;

    ~SimulatorWindow() override;

private:
    std::unique_ptr<Ui::SimulatorWindow> ui;
    // Graphics scene for simulator
    std::unique_ptr<RenderScene> m_simulator_scene;
};

#endif // SIMULATORWINDOW_H
