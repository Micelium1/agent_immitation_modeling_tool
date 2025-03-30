#include "agent.h"
#include "qjsonobject.h"
#include <QJsonDocument>

Agent::Agent(const long id,const QPainterPath& shape, const QString& pythonCode, const QString& name, QGraphicsItem* parent)
    : Object(name,shape)
    , m_name(name)
    , m_id(id)
    , m_process(new QProcess())
    , m_pythonCode(pythonCode)

{
    initPythonProcess();
}

Agent::~Agent() {
    stopExecution();
}

void Agent::initPythonProcess() {
    connect(m_process, &QProcess::readyReadStandardOutput, [this]() {
        while (m_process->canReadLine()) {
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(
                m_process->readLine().trimmed(), &error);
            if (error.error == QJsonParseError::NoError) {
                emit commandReady(doc.object());
            }
        }
    });
}

QJsonObject Agent::toJson() const {
    QJsonObject obj = Object::toJson();
    obj["is_agent"] = true;
    return obj;
}

void Agent::startExecution() {
    if (m_process->state() == QProcess::NotRunning) {
        m_process->start("python", {"-c", wrapCode(m_pythonCode)});
    }
}

void Agent::stopExecution() {
    if (m_process->state() == QProcess::Running) {
        m_process->terminate();
        m_process->waitForFinished(500);
    }
}

QString Agent::wrapCode(const QString& code) const {
    return QString(R"(
import json
import math

class AgentAPI:
    def __init__(self, agent_id):
        self.id = "%1"

    def move(self, dx, dy):
        return {"command": "move", "dx": dx, "dy": dy, "agent_id": self.id}

    def get_position(self):
        return self.position

agent = AgentAPI()
agent.position = {"x": 0, "y": 0}

def process_tick(state):
    agent.position = state['agents'][agent.id]['position']
    %2

while True:
    try:
        state = json.loads(input())
        response = process_tick(state)
        if response:
            print(json.dumps(response))
    except Exception as e:
        print(json.dumps({"error": str(e)}))
    )").arg(m_id).arg(code);
}

void Agent::handleSimulationTick(const QJsonObject& state) {
    if (m_process->state() == QProcess::Running) {
        m_process->write(QJsonDocument(state).toJson().append('\n'));
    }
}
