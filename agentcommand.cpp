#include "agentcommand.h"
#include <QJsonObject>
AgentCommand AgentCommand::fromJson(const QJsonObject& json) {
    AgentCommand cmd;
    QString command = json["command"].toString();

    if (command == "move") {
        cmd.type = MOVE;
        cmd.delta = QPointF(
            json["dx"].toDouble(),
            json["dy"].toDouble()
            );
    }
    else if (command == "set_position") {
        cmd.type = SET_POSITION;
        cmd.delta = QPointF(
            json["x"].toDouble(),
            json["y"].toDouble()
            );
    }
    // ... обработка других команд

    return cmd;
}
