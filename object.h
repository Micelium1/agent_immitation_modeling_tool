#ifndef OBJECT_H
#define OBJECT_H

#include <QGraphicsItem>
#include <QJsonObject>

class Object : public QObject {
protected:
    QPainterPath m_shape; // Логическая форма
    QPainterPath m_simplifiedPath;
    QGraphicsPathItem* m_graphicsItem = nullptr; // Визуальное представление
public:
    Object(long id, const QString& name, const QPainterPath& shape, const QPointF pos = QPointF(0,0));
    QPainterPath shape() const { return m_shape; }

    QGraphicsPathItem* graphicsItem() const { return m_graphicsItem; }

    QPainterPath simple_shape() const {return m_simplifiedPath;};

    long id() const;
    QString name() const {return m_name;};
    virtual QJsonObject toJson() const;

    virtual ~Object() {
        if (m_graphicsItem) delete m_graphicsItem;
    }
private:
    QPainterPath createSimplifiedPath(QPainterPath shape);
    QString m_name;
    long m_id;
};

#endif // OBJECT_H
