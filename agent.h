#ifndef AGENT_H
#define AGENT_H

#include "object.h"
#include <QProcess>

class Agent : public Object
{
    Q_OBJECT;
public:
    explicit Agent(const long id,const QPainterPath& shape, const QString& pythonCode, const QString& name = "agent", QGraphicsItem* parent = nullptr);

    ~Agent();

    // Сериализация с доп. данными агента
    QJsonObject toJson() const override;

    void startExecution();
    void stopExecution();

signals:
    void commandReady(const QJsonObject& command);

public slots:
    void handleSimulationTick(const QJsonObject& state);

private:
    void initPythonProcess();
    QString wrapCode(const QString& code) const;
    QString m_name;
    long m_id;
    QProcess* m_process;
    QString m_pythonCode;
};

#endif // AGENT_H
