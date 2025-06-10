#ifndef AGENT_H
#define AGENT_H

#include "object.h"
#include <QProcess>
#include <QJsonArray>
class Agent : public Object
{
    Q_OBJECT;
public:
    explicit Agent(const long id,const QPainterPath& shape, const QString& pythonCodePath, const QString& shapeName, const QString& name = "agent",QPointF pos = QPointF(0,0), QGraphicsItem* parent = nullptr);

    ~Agent();

    QJsonObject toJson() const override;

    void startExecution();
    void stopExecution();



    void sendMessage(QJsonObject message) {mailbox.append(message);};



signals:
    void commandReady(const QJsonObject& command);

public slots:
    void handleSimulationTick(QJsonObject state);

private:
    QJsonArray collectMail();
    QJsonArray getCollisionList();
    void initPythonProcess();
    QString m_name;
    long m_id;
    QProcess* m_process;
    QString m_pythonCodePath;
    QList<QJsonObject> mailbox;
};

#endif // AGENT_H
