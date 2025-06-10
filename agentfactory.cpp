#include "agentfactory.h"
#include "agent.h"
#include <QGraphicsPathItem>
#include <QGraphicsScene>

AgentFactory::AgentFactory(const QPainterPath& shape,
                           const QString& pythonCodePath,
                           const QString& name,
                           const QString& shapeName)
    : m_shape(shape),
    m_pythonCodePath(pythonCodePath),
    m_name(name),
    m_shapeName(shapeName)
{

}

QSharedPointer<Agent> AgentFactory::createAgent(long id,QPointF pos) const
{

    return QSharedPointer<Agent>::create(id,m_shape,m_pythonCodePath,m_shapeName,m_name,pos);
}

const QJsonObject AgentFactory::toJson()
{
    QJsonObject factoryObj;
    QJsonObject shapeName;
    shapeName["shape_name"] = m_shapeName;
    factoryObj[m_name] = shapeName;

    return factoryObj;
}
