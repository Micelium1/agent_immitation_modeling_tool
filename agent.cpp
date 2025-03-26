#include "agent.h"
#include <QDebug>

Agent::Agent(QGraphicsItem *item, QObject *parent) : QObject(parent), item(item) {}

QGraphicsItem *Agent::getItem() const {
    return item;
}

void Agent::update() {
    // Логика агента при каждом тике таймера
    qDebug() << "Агент обновлён!";
    // Пример: двигаем агента вправо
    item->setPos(item->x() + 1, item->y());
}
