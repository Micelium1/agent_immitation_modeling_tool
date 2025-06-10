#ifndef SUBJECT_H
#define SUBJECT_H

#include <QObject>
#include <QTimer>

class Subject : public QObject {
    Q_OBJECT

public:
    Subject(QObject *parent = nullptr);
    void start(int interval);
    void stop();
    ~Subject();
    bool isRunning() const;
    void setInterval(int msec);
    int interval() const;
    void printListeners();
signals:
    void notify();

private:
    QTimer* m_timer;
};

#endif // SUBJECT_H
