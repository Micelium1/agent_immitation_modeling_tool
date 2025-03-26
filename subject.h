#ifndef SUBJECT_H
#define SUBJECT_H

#include <QObject>
#include <QTimer>

class Subject : public QObject {
    Q_OBJECT

public:
    Subject(QObject *parent = nullptr);

signals:
    void notify(); // Сигнал для уведомления наблюдателей

private:
    QTimer *timer;
};

#endif // SUBJECT_H
