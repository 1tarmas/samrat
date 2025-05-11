// mainwindow.h
#pragma once
#include <QMainWindow>
#include "volume_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCreateVolume();
    void onMountVolume();
    void onDismountVolume();

private:
    Ui::MainWindow *ui;
    VolumeManager volumeManager;
    std::string currentVolumePath;
};