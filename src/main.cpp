// main.cpp
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // Initialize OpenSSL
    OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
    
    MainWindow w;
    w.show();
    
    int ret = a.exec();
    
    // Clean up OpenSSL
    OPENSSL_cleanup();
    return ret;
}