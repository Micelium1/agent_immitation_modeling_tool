#include "agent.h"
#include "qbrush.h"
#include "qdir.h"
#include "qjsonobject.h"
#include <QJsonDocument>
#include <QString>

Agent::Agent(const long id,const QPainterPath& shape, const QString& pythonCodePath,const QString& shapeName, const QString& name,QPointF pos, QGraphicsItem* parent)
    : Object(-1,shapeName,shape,pos)
    , m_name(name)
    , m_id(id)
    , m_process(new QProcess(this))
    , m_pythonCodePath(pythonCodePath)

{
    m_graphicsItem->setData(0, QVariant::fromValue<Agent*>(this));
    qDebug() << "Agent instance:" << this;
    if (m_process) {
        qDebug() << m_process;
    }
    initPythonProcess();
}

Agent::~Agent() {
    stopExecution();
    delete m_process;
}

void Agent::initPythonProcess() {

    m_process->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_process, &QProcess::readyReadStandardOutput, this, [this]() {
        QByteArray output = m_process->readAllStandardOutput();
        QTextStream stream(output);
        QString line;
        while (stream.readLineInto(&line)) {

            QJsonParseError error;
            line.replace("\n","");
            QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &error);

            if (error.error == QJsonParseError::NoError) {
                qDebug() << "Valid JSON received:" << doc.toJson();
                emit commandReady(doc.object());
            } else {
                line.replace("'", "\"");
                doc = QJsonDocument::fromJson(line.toUtf8(), &error);
                if (error.error == QJsonParseError::NoError) {
                    qDebug() << "Fixed JSON received:" << doc.toJson();
                    emit commandReady(doc.object());
                } else {
                    qDebug() << "Invalid JSON:" << line.toUtf8() << "Error:" << error.errorString();
                }
            }
        }
    });
    connect(m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        qWarning() << "Process error:" << error << m_process->errorString();
    });
    connect(m_process, &QProcess::readyReadStandardError, [this]() {
        qDebug() << "Python error:" << m_process->readAllStandardError();
    });
    startExecution();
}


QJsonObject Agent::toJson() const {
    QJsonObject obj = Object::toJson();
    obj["is_agent"] = true;
    obj["id"] = static_cast<qint64>(m_id);
    obj["agent_name"] = m_name;
    QColor color = graphicsItem()->brush().color();
    QJsonObject colorObj;
    colorObj["r"] = color.red();
    colorObj["g"] = color.green();
    colorObj["b"] = color.blue();
    obj["color"] = colorObj;
    return obj;
}

void Agent::startExecution() {
    qDebug() << "Attempting to start process for agent" << m_id;

    if (!m_process) {
        qCritical() << "Process is null!";
        return;
    }

    if (m_process->state() != QProcess::NotRunning) {
        qDebug() << "Process already running or starting";
        return;
    }

    QFile codeFile(m_pythonCodePath);
    QString pythonCode = codeFile.open(QIODevice::ReadOnly) ? QTextStream(&codeFile).readAll() : QString();
    codeFile.close();
    pythonCode.replace("AGENT_ID_REPLACE",QString("%1").arg(m_id));
    QStringList args = {"-c", pythonCode};

    m_process->start("python", args);


    if (!m_process->waitForStarted(3000)) {
        qCritical() << "Failed to start process:" << m_process->errorString();


        QStringList pythonNames = {"python3", "python3.9", "python3.8"};
        for (const QString& name : pythonNames) {
            qDebug() << "Trying alternative:" << name;
            m_process->start(name, args);
            if (m_process->waitForStarted(1000)) break;
        }

        if (m_process->state() != QProcess::Running) {
            qCritical() << "All python start attempts failed!";
        }
    } else {
        qDebug() << "Process started successfully, PID:" << m_process->processId();
    }
}

void Agent::stopExecution() {
    if (m_process && m_process->state() == QProcess::Running) {

        m_process->terminate();

        if (!m_process->waitForFinished(500)) {

            m_process->kill();
            m_process->waitForFinished(100);
        }

        if (m_process->state() == QProcess::Running) {
            qWarning() << "Process still running after kill!";
        }
    }
}




void Agent::handleSimulationTick(QJsonObject state) {
    if (m_process == nullptr) {
        qCritical() << "Process pointer is null";
        return;
    }

    if (m_process->state() != QProcess::Running) {
        qWarning() << "Процесс не работает";
        return;
    }

    if (state.isEmpty()) {
        qWarning() << "Received empty state";
        return;
    }
    state["mail"] = collectMail();
    state["collisions"] = getCollisionList();
    QByteArray data;
    try {
        data = QJsonDocument(state).toJson(QJsonDocument::Compact);
        data.append('\n');
    } catch (...) {
        qCritical() << "Failed to serialize JSON";
        return;
    }
    qint64 bytesWritten = m_process->write(data);
    if (bytesWritten == -1) {
        qCritical() << "Write error:" << m_process->errorString();
    } else if (bytesWritten < data.size()) {
        qWarning() << "Incomplete write to process";
    }
}

QJsonArray Agent::collectMail()
{
    QJsonArray jsonMailbox;
    for (const auto &mail : mailbox) {
        jsonMailbox.append(mail);
    }
    mailbox.clear();
    return jsonMailbox;
}

QPointF findNearestPointOnPath(const QPainterPath& path, const QPointF& point) {
    qreal minDist = std::numeric_limits<qreal>::max();
    QPointF nearestPoint;

    // Перебираем все сегменты пути
    for (int i = 0; i < path.elementCount(); ++i) {
        QPainterPath::Element elem = path.elementAt(i);
        QPointF pathPoint(elem.x, elem.y);

        qreal dist = QLineF(point, pathPoint).length();
        if (dist < minDist) {
            minDist = dist;
            nearestPoint = pathPoint;
        }
    }

    return nearestPoint;
}

QPointF calculatePenetration(const QPainterPath& path1, const QPainterPath& path2) {
    if (!path1.intersects(path2)) {
        return QPointF(0, 0);
    }

    QPainterPath intersection = path1.intersected(path2);
    if (intersection.isEmpty()) {
        return QPointF(0, 0);
    }

    QPointF collisionPoint = intersection.boundingRect().center();

    QPointF nearest1 = findNearestPointOnPath(path1, collisionPoint);
    QPointF nearest2 = findNearestPointOnPath(path2, collisionPoint);

    return nearest1 - nearest2;
}



QJsonArray Agent::getCollisionList() {
    QJsonArray collisionsArray;
    if (!m_graphicsItem || !m_graphicsItem->scene()) return collisionsArray;

    const auto potentialCollisions = m_graphicsItem->collidingItems(Qt::IntersectsItemBoundingRect);

    QPainterPath thisPath = this->simple_shape().translated(this->graphicsItem()->pos());

    for (QGraphicsItem* item : potentialCollisions) {
        if (item == m_graphicsItem) continue;


        QPainterPath otherPath = item->shape().translated(item->pos());
        QPointF penetration = calculatePenetration(thisPath, otherPath);
        qreal length = hypot(penetration.x(), penetration.y());

        if (length <= 0) continue;

        QJsonObject collisionInfo;
        collisionInfo["penetration"] = QJsonObject{
            {"x", penetration.x()},
            {"y", penetration.y()},
            {"length", length}
        };

        if (item->data(0).value<Agent*>()) {
            collisionInfo["type"] = "agent";
        } else if (item->data(0).value<Object*>()) {
            collisionInfo["type"] = "object";
        }

        collisionsArray.append(collisionInfo);
    }

    return collisionsArray;
}
