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
    // Конструктор с параметрами шаблона
    AgentFactory(const QPainterPath& shape,
                 const QString& pythonCode,
                 const QString& agentTypeName);

    // Создание агента на основе шаблона
    QSharedPointer<Agent> createAgent(long id) const;

    // Геттеры для параметров шаблона
    const QPainterPath& shape() const { return m_shape; }
    const QString& pythonCode() const { return m_pythonCode; }
    const QString& agentTypeName() const { return m_name; }

private:
    QPainterPath m_shape;
    QString m_pythonCode;
    QString m_name;
};

#endif // AGENTFACTORY_H
