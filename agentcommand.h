#ifndef AGENTCOMMAND_H
#define AGENTCOMMAND_H

#include "qcolor.h"
#include "qpainterpath.h"
#include <QPointF>
#include <QJsonObject>

struct AgentCommand {
    enum Type {
        MOVE,
        SET_POSITION,
        SET_COLOR,
        SET_SHAPE,
        SPAWN,
        STOP,
        MESSAGE,
        ROTATE,
        DELETE
    };

    Type type;
    QVariantMap params;
    QColor color;
    QPainterPath path;
    long id;

    static AgentCommand fromJson(const QJsonObject& json);
    QJsonObject toJson() const;
};

#endif // AGENTCOMMAND_H
