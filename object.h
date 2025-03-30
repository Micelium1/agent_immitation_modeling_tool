#ifndef OBJECT_H
#define OBJECT_H

#include <QGraphicsItem>
#include <QJsonObject>

class Object : public QObject {
protected:
    QPainterPath m_shape; // Логическая форма
    QGraphicsPathItem* m_graphicsItem = nullptr; // Визуальное представление

public:
    Object(const QString& name,const QPainterPath& shape);

    // Для симуляции:
    const QPainterPath& shape() const { return m_shape; }

    // Для отображения:
    QGraphicsPathItem* graphicsItem() const { return m_graphicsItem; }

    virtual QJsonObject toJson() const;

    virtual ~Object() {
        if (m_graphicsItem) delete m_graphicsItem;
    }
private:
    QString m_name;
};

#endif // OBJECT_H
