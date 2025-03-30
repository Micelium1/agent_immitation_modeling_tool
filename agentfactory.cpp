#include "agentfactory.h"
#include "agent.h"
#include <QGraphicsPathItem>
#include <QGraphicsScene>

AgentFactory::AgentFactory(const QPainterPath& shape,
                           const QString& pythonCode,
                           const QString& name)
    : m_shape(shape),
    m_pythonCode(pythonCode),
    m_name(name)
{}

QSharedPointer<Agent> AgentFactory::createAgent(long id) const
{

    return QSharedPointer<Agent>::create(id,m_shape,m_pythonCode,m_name);
}
