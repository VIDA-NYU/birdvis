#pragma once
#include <QApplication>
#include <QDebug>
#include <exception>

class BirdVisApplication : public QApplication {
public:
    using QApplication::QApplication;
    bool notify(QObject *receiver, QEvent *event) override {
        try { return QApplication::notify(receiver,event); }
        catch(const std::exception &error) {
            qCritical() << "BirdVis could not complete the operation:" << error.what();
            exit(1);
            return false;
        }
    }
};
