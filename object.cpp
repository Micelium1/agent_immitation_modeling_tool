#include "object.h"

Object::Object(const QString& name,const QPainterPath& shape)
    : m_name(name)
{
    m_shape = shape;
    m_graphicsItem = new QGraphicsPathItem(shape);
    m_graphicsItem->setFlag(QGraphicsItem::ItemIsMovable);
}
QJsonObject Object::toJson() const
{
    QRectF rect = shape().boundingRect();
    return {
        {"type", m_name},
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
