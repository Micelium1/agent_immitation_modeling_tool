#include "agentcommand.h"
#include <QJsonObject>
AgentCommand AgentCommand::fromJson(const QJsonObject& json) {
    AgentCommand cmd;
    QString command = json["command"].toString();
    cmd.id = json["agent_id"].toInt();
    cmd.params = json["params"].toObject().toVariantMap();

    if (command == "move") {
        cmd.type = MOVE;
    }
    else if (command == "set_position") {
        cmd.type = SET_POSITION;
    }
    else if (command == "set_color") {
        cmd.type = SET_COLOR;
    }
    else if (command == "rotate") {
        cmd.type = ROTATE;
    }
    else if (command == "message") {
        cmd.type = MESSAGE;
    }
    else if (command == "spawn") {
        cmd.type = SPAWN;
    }
    else if (command == "stop") {
        cmd.type = STOP;
    }
    else if (command == "delete") {
        cmd.type = DELETE;
    }
    else
    {
        qDebug() << "Команда не распознана " << json;
    }

    return cmd;
}
