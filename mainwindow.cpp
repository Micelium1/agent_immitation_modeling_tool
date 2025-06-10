#include "mainwindow.h"
#include "agentbaseselection.h"
#include "agentcommand.h"
#include "agentfactory.h"
#include "mailmanager.h"
#include "projectmanager.h"
#include "ui_mainwindow.h"
#include "object.h"

MainWindow::MainWindow(QString ProjectPath, QWidget *parent)
    : QMainWindow(parent)
    , isSimulationRunning(false)
    , m_projectPath(ProjectPath)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene);

    subject = new Subject(this);

    QPushButton *createObjectButton = new QPushButton("Создать объект", this);
    QPushButton *createAgentButton = new QPushButton("Создать агента", this);
    if (!objectsDir().exists() or !agentsDir().exists() or !pythonDir().exists()) {
        if (!objectsDir().exists()) {
            objectsDir().mkdir(".");
        }

        if (!agentsDir().exists()) {
            agentsDir().mkdir(".");
        }

        if (!pythonDir().exists()) {
            pythonDir().mkdir(".");
        }
    }
    else {
        loadProject(m_projectPath);
    }
    ui->objectToolBar->addWidget(createObjectButton);
    ui->agentToolBar->addWidget(createAgentButton);


    connect(createObjectButton, &QPushButton::clicked, this, &MainWindow::createObject);

    connect(createAgentButton, &QPushButton::clicked, this, &MainWindow::createAgent);

    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveProjectButtonPress);

    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadProjectButtonPress);

    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::startSimulation);

    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::stopSimulation);
}

void MainWindow::createObject() {
    ObjectEditor editor(this);
    editor.setModal(true);
    if (editor.exec() == QDialog::Accepted) {
        QPainterPath rawItem = editor.getCreatedItem()->path();
        if (!rawItem.isEmpty()) {

            shapes[editor.getObjectName()] = rawItem;

            addObjectToToolbar(editor.getObjectName());
        }
    }
}

void MainWindow::createAgent() {


    AgentBaseSelection selection(this);
    if (selection.exec() == QDialog::Accepted) {
        qDebug() << selection.getAgentName();
        QFile templateFile(":/python/template");
        if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Failed to open template file: " << templateFile.errorString();
            return;
        }
        QDir outputDir(m_projectPath + "/python");

        QString outputPath = outputDir.filePath(selection.getAgentName()+QString(".py"));
        QFile code(outputPath);
        if (!code.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Failed to create/re-create code file: " << code.errorString() << " " << code.fileName();
            return;
        }
        QTextStream in(&templateFile);
        QString templ = in.readAll();
        templateFile.close();
        templ.replace("{USER_CODE}",selection.getCode());
        QTextStream out(&code);
        out << templ;
        code.close();

        agentFactories.emplace(selection.getAgentName(), AgentFactory(shapes[selection.getShapeName()], code.fileName(),selection.getAgentName(),selection.getShapeName()));
        addAgentToToolbar(selection.getAgentName());
    }
}
QJsonArray shapeToJson(const QPainterPath& path) {
    QJsonArray pathData;
    for (int i = 0; i < path.elementCount(); ++i) {
        const QPainterPath::Element& element = path.elementAt(i);
        QJsonObject elemObj;
        elemObj["type"] = element.type;
        elemObj["x"] = element.x;
        elemObj["y"] = element.y;
        pathData.append(elemObj);
    }
    return pathData;
}
QPainterPath JsonToShape(const QJsonArray& pathData) {
    QPainterPath path;

    for (const QJsonValue& value : pathData) {
        QJsonObject elemObj = value.toObject();

        int type = elemObj["type"].toInt();
        qreal x = elemObj["x"].toDouble();
        qreal y = elemObj["y"].toDouble();

        switch (type) {
        case QPainterPath::MoveToElement:
            path.moveTo(x, y);
            break;
        case QPainterPath::LineToElement:
            path.lineTo(x, y);
            break;
        }
    }
    return path;
}


void MainWindow::saveProject(QString projectPath)
{
    qDebug() <<"Начало сохранения " << projectPath;
    for(auto shape = shapes.begin();shape != shapes.end();++shape) {

        QFile Fshape(projectPath+ "/objects/" + shape.key() + ".json");
        Fshape.open(QIODevice::WriteOnly);
        Fshape.write(QJsonDocument(shapeToJson(shape.value())).toJson());
        Fshape.close();
    }
    for (auto agent = agentFactories.begin(); agent != agentFactories.end(); ++agent) {
        QJsonObject Jagent;
        Jagent = agent.value().toJson();
        QFile Fagent(projectPath + "/agents/" + agent.key() + ".json");
        Fagent.open(QIODevice::WriteOnly);
        Fagent.write(QJsonDocument(Jagent).toJson());
        Fagent.close();
    }
    QFile Fstate(projectPath + "/state.json");
    Fstate.open(QIODevice::WriteOnly);
    Fstate.write(QJsonDocument(getCurrentSimulationState()).toJson());
    Fstate.close();

}
void MainWindow::loadObjectsFromFiles() {

    QDir objectsDirC = objectsDir();
    objectsDirC.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    objectsDirC.setNameFilters(QStringList() << "*.json");
    QStringList objectsList = objectsDirC.entryList();

    for (const QString& fileName:objectsList) {
        QFile objectFile = objectsDirC.filePath(fileName);
        if (!objectFile.open(QIODevice::ReadOnly)) {
            qDebug() << "failed to open object file " << fileName;
        }
        QByteArray jsonData = objectFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        QPainterPath shape = JsonToShape(doc.array());
        QFileInfo info(objectFile);
        QString objectName = info.completeBaseName();
        qDebug() << "Send filename as key " << objectName;
        shapes[objectName] = shape;
        addObjectToToolbar(objectName);
    }
}

void MainWindow::loadAgentsFromFiles()
{


    QDir agentsDirC = agentsDir();
    agentsDirC.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    agentsDirC.setNameFilters(QStringList() << "*.json");
    QStringList agentsList = agentsDirC.entryList();

    for (const QString& fileName:agentsList) {
        QFile agentFile = agentsDirC.filePath(fileName);
        if (!agentFile.open(QIODevice::ReadOnly)) {
            qDebug() << "failed to open object file " << fileName;
        }
        QJsonDocument doc = QJsonDocument::fromJson(agentFile.readAll());
        QFileInfo info(agentFile);
        QString agentName = info.completeBaseName();
        qDebug() << "Agent name:" << agentName << "\n In document: " << doc;
        QJsonValueConstRef serializedAgent = doc.object()[agentName];
        qDebug() << "Serialized agent " << serializedAgent;
        qDebug() << "after toObject" << serializedAgent.toObject();
        QString shapeName = serializedAgent.toObject()["shape_name"].toString();
        agentFactories.emplace(agentName,shapes.find(shapeName).value(),pythonDir().absoluteFilePath(agentName + ".py"),agentName,shapeName);
        addAgentToToolbar(agentName);
    }
}

void MainWindow::loadStateFromFile()
{

    QFile Fstate = stateFile();
    if (!Fstate.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open state file " << Fstate.fileName();
    }
    QJsonDocument stateDoc = QJsonDocument::fromJson(Fstate.readAll());
    QJsonObject stateObj = stateDoc.object();
    QJsonObject objectsJson = stateObj["objects"].toObject();
    QJsonObject agentsJson = stateObj["agents"].toObject();
    long maxId = 0;
    for (auto object = objectsJson.constBegin();object != objectsJson.constEnd();++object) {
        long id = object.key().toLong();
        QString shapeName = object.value().toObject()["shapeName"].toString();
        QJsonObject pos = object.value().toObject()["position"].toObject();
        objects.insert(id,QSharedPointer<Object>::create(id,shapeName,shapes[shapeName],QPointF(pos["x"].toDouble(),pos["y"].toDouble())));
        auto object_ = objects.find(id);
        if (object_ != objects.end()) {
            ui->graphicsView->scene()->addItem(object_.value()->graphicsItem());
        }
        maxId = id > maxId ? id : maxId;

    }
    setObjectsIdCounter(maxId + 1);
    maxId = 0;
    for (auto agent = agentsJson.constBegin();agent != agentsJson.constEnd();++agent) {
        long id = agent.key().toLong();
        QString agentName = agent.value().toObject()["agent_name"].toString();
        auto corresponding_factory = agentFactories.find(agentName);
        if (corresponding_factory != agentFactories.end()){
            QJsonObject pos = agent.value().toObject()["position"].toObject();
            agents.insert(id,corresponding_factory->createAgent(id,QPointF(pos["x"].toDouble(),pos["y"].toDouble())));
            Agent* agent = agents[id].data();
            qDebug() << "Found factory " << "and created agent " << agent;
            ui->graphicsView->scene()->addItem(agent->graphicsItem());
            connect(agent,&Agent::commandReady,this,&MainWindow::onAgentResponse,Qt::QueuedConnection);
            connect(subject, &Subject::notify, agent, [this,agent]() {
                agent->handleSimulationTick(this->getCurrentSimulationState());
            },Qt::QueuedConnection);
        }
        maxId = id > maxId ? id : maxId;
    }
    setAgentsIdCounter(maxId + 1);
}


void MainWindow::loadProject(QString projectPath)
{
    m_projectPath = projectPath;
    clearProject();

    loadObjectsFromFiles();

    loadAgentsFromFiles();

    loadStateFromFile();
}

void MainWindow::clearProject()
{
    agents.clear();
    objects.clear();
    shapes.clear();
    agentFactories.clear();
    clearToolBar(ui->agentToolBar);
    clearToolBar(ui->objectToolBar);
}

void MainWindow::clearToolBar(QToolBar* bar)
{
    QList<QAction*> actions = bar->actions();

    for (QAction* action : actions) {
        if (QToolButton* button = qobject_cast<QToolButton*>(bar->widgetForAction(action))) {
            if (button->objectName().isEmpty() ||
                !button->objectName().startsWith("qt_")) {

                bar->removeAction(action);
                button->disconnect();
                button->deleteLater();
            }
        }
    }
}
void inline MainWindow::saveProjectButtonPress()
{
    saveProject(m_projectPath);
}

void MainWindow::loadProjectButtonPress()
{
    ProjectManager manager;
    manager.exec();
    if (manager.projectPath().isEmpty()) {
        return;
    }
    if (manager.isNew()) {
        m_projectPath = manager.projectPath();
        if (!objectsDir().exists() or !agentsDir().exists() or !pythonDir().exists()) {
            if (!objectsDir().exists()) {
                objectsDir().mkdir(".");
            }

            if (!agentsDir().exists()) {
                agentsDir().mkdir(".");
            }

            if (!pythonDir().exists()) {
                pythonDir().mkdir(".");
            }
        }
        clearProject();
    }
    else loadProject(manager.projectPath());
}

void MainWindow::startSimulation() {
    if (isSimulationRunning) return;
    isSimulationRunning = true;
    subject->start(100);
}

void MainWindow::stopSimulation() {
    if (!isSimulationRunning) return;
    isSimulationRunning = false;
    subject->stop();
}

void MainWindow::spawnObject(QString name, QPointF pos)
{
    long id = getObjectsId();

    objects.insert(id,QSharedPointer<Object>::create(id,name,shapes[name],pos));
    auto object = objects.find(id);
    if (object != objects.end()) {
        ui->graphicsView->scene()->addItem(object.value()->graphicsItem());
    }
}

void MainWindow::spawnAgent(QString name, QPointF pos)
{
    long id = getAgentsId();

    auto corresponding_factory = agentFactories.find(name);
    if (corresponding_factory != agentFactories.end()){
        agents.insert(id,corresponding_factory->createAgent(id,pos));
        Agent* agent = agents[id].data();

        ui->graphicsView->scene()->addItem(agent->graphicsItem());
        connect(agent,&Agent::commandReady,this,&MainWindow::onAgentResponse,Qt::QueuedConnection);
        connect(subject, &Subject::notify, agent, [this,agent]() {
            agent->handleSimulationTick(this->getCurrentSimulationState());
        },Qt::QueuedConnection);
    }
}

void MainWindow::addObjectToToolbar(const QString& name) {
    QToolButton* button = new QToolButton(this);

    button->setText(name);
    connect (button, &QToolButton::clicked, this, [this, name]() {
        QPointF Scenter = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect().center());
        spawnObject(name,Scenter);
    });
    ui->objectToolBar->addWidget(button);
}


void MainWindow::addAgentToToolbar(const QString &agentName) {
    QToolButton *button = new QToolButton(this);
    button->setText(agentName);
    connect(button, &QToolButton::clicked, this,[agentName, this](){
        QPointF Scenter = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect().center());
        spawnAgent(agentName,Scenter);
    });
    ui->agentToolBar->addWidget(button);
}

void MainWindow::loadObjectFromJson(const QJsonObject)
{

}

void MainWindow::onAgentResponse(const QJsonObject &comand)
{
    AgentCommand cmd = AgentCommand::fromJson(comand);
    if (cmd.type == AgentCommand::Type::MOVE)
    {
        agents[cmd.id].get()->graphicsItem()->moveBy(cmd.params["dx"].toDouble(),cmd.params["dy"].toDouble());

    }
    if (cmd.type == AgentCommand::SET_POSITION)
    {
        agents[cmd.id].get()->graphicsItem()->setPos(cmd.params["x"].toDouble(),cmd.params["y"].toDouble());
    }
    if (cmd.type == AgentCommand::SET_COLOR)
    {
        agents[cmd.id].get()->graphicsItem()->setBrush(QColor(cmd.params["r"].toInt(),cmd.params["g"].toInt(),cmd.params["b"].toInt()));
    }
    if (cmd.type == AgentCommand::SPAWN)
    {
        QPointF pos(cmd.params["entity_pos"].toMap()["x"].toDouble(),cmd.params["entity_pos"].toMap()["y"].toDouble());
        if (cmd.params["entity_type"].toString() == "object")
        {
            spawnObject(cmd.params["entity_name"].toString(),pos);
        }
        else if (cmd.params["entity_type"].toString() == "agent")
        {
            spawnAgent(cmd.params["entity_name"].toString(),pos);
        }
    }
    if (cmd.type == AgentCommand::ROTATE)
    {
        QGraphicsItem* agent = agents[cmd.id].get()->graphicsItem();


        agent->setRotation(agent->rotation() + cmd.params["angle"].toDouble());
    }
    if (cmd.type == AgentCommand::MESSAGE)
    {
        QJsonObject mail = MailManager::formMail(QString::number(cmd.id),cmd.params["message"].toString());
        agents.find(cmd.params["reciver"].toInt()).value() -> sendMessage(mail);
    }
    if (cmd.type == AgentCommand::STOP)
    {
        agents.remove(cmd.id);
    }
    if(cmd.type == AgentCommand::DELETE)
    {
        QJsonObject mail = MailManager::formMail("system","delete");
        agents.find(cmd.id).value()->sendMessage(mail);
        agents[cmd.id].clear();
        agents.remove(cmd.id);
    }

}


QJsonObject MainWindow::getCurrentSimulationState()
{
QJsonObject root;

QJsonObject objectsArray;
for (auto it = objects.begin(); it != objects.end(); ++it) {
    QJsonObject obj;
    QString id = QString::number(it.key());
    objectsArray[id] = it.value()->toJson();
}
root["objects"] = objectsArray;

QJsonObject agentsArray;
for (auto it = agents.begin(); it != agents.end(); ++it) {
    QString id = QString::number(it.key());
    agentsArray[id] = it.value()->toJson();
}
root["agents"] = agentsArray;

return root;
}


const QHash<QString,QPainterPath>& MainWindow::getObjects() const
{
    return shapes;
}

void MainWindow::setProjectDir(const QString&& path)
{
    m_projectPath = path;
}


MainWindow::~MainWindow()
{
        qDebug() << "MainWindow уничтожен!" << this;

    delete ui;
}
