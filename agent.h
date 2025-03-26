#ifndef AGENT_H
#define AGENT_H

#include <QObject>
#include <QGraphicsItem>

class Agent : public QObject {
    Q_OBJECT

public:
    Agent(QGraphicsItem *item, QObject *parent = nullptr);
    QGraphicsItem *getItem() const;

public slots:
    void update(); // Слот для реакции на уведомление

private:
    QGraphicsItem *item;
};

#endif // AGENT_H
