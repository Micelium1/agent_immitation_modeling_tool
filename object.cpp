#include "object.h"

Object::Object(long id, const QString& name, const QPainterPath& shape, const QPointF pos)
    : m_name(name)
    , m_id(id)

{
    m_shape = shape;
    m_simplifiedPath = createSimplifiedPath(shape);
    m_graphicsItem = new QGraphicsPathItem(m_simplifiedPath);
    m_graphicsItem->setParentItem(nullptr);


    m_graphicsItem->setData(0, QVariant::fromValue<Object*>(this));
    m_graphicsItem->setPos(pos);
    m_graphicsItem->setFlag(QGraphicsItem::ItemIsMovable);
    m_graphicsItem->setTransformOriginPoint(m_graphicsItem->boundingRect().center());
}

long Object::id() const
{
    return m_id;
}


QJsonObject Object::toJson() const
{
    QRectF rect = shape().boundingRect();
    return {
        {"id", QJsonValue(static_cast<qint64>(m_id))},
        {"shapeName", m_name},
        {"position", QJsonObject{
                         {"x", graphicsItem()->pos().x()},
                         {"y", graphicsItem()->pos().y()}
                     }},
        {"shape", QJsonObject{
                      {"bounding_rect", QJsonObject{
                                            {"left", rect.left()},
                                            {"top", rect.top()},
                                            {"right", rect.right()},
                                            {"bottom", rect.bottom()}
                                        }}
                  }}
    };
}

QPainterPath Object::createSimplifiedPath(QPainterPath shape)
{
    QPainterPath simplifiedPath;

    qreal tolerance = 2.0;
    if (shape.elementCount() > 0) {
        simplifiedPath.moveTo(shape.elementAt(0).x, shape.elementAt(0).y);
    }

    for (int i = 1; i < shape.elementCount(); ++i) {
        QPainterPath::Element elem = shape.elementAt(i);
        QPointF lastPoint(simplifiedPath.currentPosition());
        QPointF currentPoint(elem.x, elem.y);

        qreal dx = currentPoint.x() - lastPoint.x();
        qreal dy = currentPoint.y() - lastPoint.y();
        qreal distanceSquared = dx * dx + dy * dy;

        if (distanceSquared > tolerance * tolerance) {
            simplifiedPath.lineTo(currentPoint);
        }
    }
    return simplifiedPath;
}
