#ifndef AGENTFACTORY_H
#define AGENTFACTORY_H

#include <QPainterPath>
#include <QString>
#include <QSharedPointer>
class Agent;
class QGraphicsScene;
class QGraphicsPathItem;

class AgentFactory
{
public:
    AgentFactory(const QPainterPath& shape,
                 const QString& pythonCodePath,
                 const QString& agentTypeName,
                 const QString& shapeName);

    QSharedPointer<Agent> createAgent(long id, QPointF pos = QPointF(0,0)) const;

    const QPainterPath& shape() const { return m_shape; }
    const QString& pythonCodePath() const { return m_pythonCodePath; }
    const QString& agentTypeName() const { return m_name; }
    const QJsonObject toJson();

private:
    QPainterPath m_shape;
    QString m_pythonCodePath;
    QString m_name;
    QString m_shapeName;
};

#endif // AGENTFACTORY_H
