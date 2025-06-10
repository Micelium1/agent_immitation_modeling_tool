#include "subject.h"
#include <QDebug>

Subject::Subject(QObject *parent) : QObject(parent), m_timer(new QTimer(this))
{
    if (!m_timer) {
        qCritical() << "Failed to create QTimer";
        return;
    }


    m_timer->setInterval(100);
    m_timer->setTimerType(Qt::PreciseTimer);

    bool connected = connect(m_timer, &QTimer::timeout, this, &Subject::notify);
    if (!connected) {
        qWarning() << "Failed to connect timer timeout signal";
    }

}

Subject::~Subject()
{
    stop();
}

void Subject::start(int msec)
{
    if (!m_timer) {
        qWarning() << "Timer is not initialized";
        return;
    }

    if (msec > 0) {
        m_timer->setInterval(msec);
    }

    if (!m_timer->isActive()) {
        m_timer->start();
        qDebug() << "Subject timer started with interval:" << m_timer->interval() << "ms";
    }
}

void Subject::stop()
{
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        qDebug() << "Subject timer stopped";
    }
}

bool Subject::isRunning() const
{
    return m_timer ? m_timer->isActive() : false;
}

void Subject::setInterval(int msec)
{
    if (m_timer) {
        m_timer->setInterval(msec);
        qDebug() << "Subject timer interval changed to:" << msec << "ms";
    }
}

int Subject::interval() const
{
    return m_timer ? m_timer->interval() : 0;
}

void Subject::printListeners()
{
    qDebug() << "Recivers: " << this->receivers(SIGNAL(notify()));
}

