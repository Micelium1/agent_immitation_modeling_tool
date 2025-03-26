#include "subject.h"

Subject::Subject(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Subject::notify); // Таймер вызывает сигнал notify
    timer->start(100); // Таймер срабатывает каждые 100 мс
}
