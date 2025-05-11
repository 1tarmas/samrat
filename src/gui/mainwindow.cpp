// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    
    connect(ui->createButton, &QPushButton::clicked, this, &MainWindow::onCreateVolume);
    connect(ui->mountButton, &QPushButton::clicked, this, &MainWindow::onMountVolume);
    connect(ui->dismountButton, &QPushButton::clicked, this, &MainWindow::onDismountVolume);
}

void MainWindow::onCreateVolume() {
    QString path = QFileDialog::getSaveFileName(this, "Create Volume");
    if (path.isEmpty()) return;
    
    bool ok;
    QString password = QInputDialog::getText(this, "Password", 
        "Enter password:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;
    
    uint64_t size = 1024 * 1024 * 100; // 100MB default
    if (volumeManager.createVolume(path.toStdString(), size, 
                                VolumeManager::AES256, password.toStdString())) {
        QMessageBox::information(this, "Success", "Volume created successfully");
    } else {
        QMessageBox::warning(this, "Error", "Failed to create volume");
    }
}

void MainWindow::onMountVolume() {
    QString path = QFileDialog::getOpenFileName(this, "Open Volume");
    if (path.isEmpty()) return;
    
    bool ok;
    QString password = QInputDialog::getText(this, "Password", 
        "Enter password:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;
    
    if (volumeManager.mountVolume(path.toStdString(), password.toStdString())) {
        currentVolumePath = path.toStdString();
        ui->statusLabel->setText("Mounted: " + path);
        QMessageBox::information(this, "Success", "Volume mounted successfully");
    } else {
        QMessageBox::warning(this, "Error", "Failed to mount volume");
    }
}

void MainWindow::onDismountVolume() {
    volumeManager.dismountVolume();
    ui->statusLabel->setText("No volume mounted");
    QMessageBox::information(this, "Info", "Volume dismounted");
}