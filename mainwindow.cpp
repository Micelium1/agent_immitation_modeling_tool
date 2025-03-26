#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , isSimulationRunning(false)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene);


    QPushButton *createObjectButton = new QPushButton("Создать объект", this);
    QPushButton *createAgentButton = new QPushButton("Создать агента", this);

    ui->objectToolBar->addWidget(createObjectButton);
    ui->agentToolBar->addWidget(createAgentButton);


    // Subject (таймер)
    subject = new Subject(this);

    // Создание объекта
    connect(createObjectButton, &QPushButton::clicked, this, &MainWindow::createObject);
    // Создание агента
    connect(createAgentButton, &QPushButton::clicked, this, &MainWindow::createAgent);

    // Сохранение симуляции
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveSimulation);

    // Загрузка симуляции
    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadSimulation);

    // Запуск симуляции
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::startSimulation);
    // Остановка симуляции
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::stopSimulation);
    //createObject();
}

void MainWindow::createObject() {
    ObjectEditor editor(this);
    editor.setModal(true);
    if (editor.exec() == QDialog::Accepted) {
        QGraphicsPathItem* rawItem = editor.getCreatedItem();
        if (rawItem) {
            QSharedPointer<QGraphicsPathItem> item(rawItem);

            qDebug() << "5. Объект получен:" << item;
            objects.append(item);
            addObjectToToolbar(item, editor.getObjectName(), editor.hasCollision());
        }
    }
}

void MainWindow::createAgent() {



    Agent *agent = new Agent(item, this);
    agents.append(agent);
    connect(subject, &Subject::notify, agent, &Agent::update); // Подписываем агента на сигнал
    addAgentToToolbar(agent, "Агент " + QString::number(agents.size()));
}

void MainWindow::saveSimulation() {
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить симуляцию", "", "JSON Files (*.json)");
    if (fileName.isEmpty()) return;

    QJsonArray objectsArray;
    for (auto item : objects) {
        QJsonObject obj;
        obj["type"] = "object";
        obj["x"] = item->x();
        obj["y"] = item->y();
        obj["name"] = item->data(Qt::UserRole).toString(); // Сохраняем имя
        obj["hasCollision"] = item->flags().testFlag(QGraphicsItem::ItemIsSelectable);
        objectsArray.append(obj);
    }

    QJsonArray agentsArray;
    for (Agent *agent : agents) {
        QJsonObject obj;
        obj["type"] = "agent";
        obj["x"] = agent->getItem()->x();
        obj["y"] = agent->getItem()->y();
        obj["name"] = agent->getItem()->data(Qt::UserRole).toString(); // Сохраняем имя
        agentsArray.append(obj);
    }

    QJsonObject root;
    root["objects"] = objectsArray;
    root["agents"] = agentsArray;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}

void MainWindow::loadSimulation() {
    QString fileName = QFileDialog::getOpenFileName(this, "Загрузить симуляцию", "", "JSON Files (*.json)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();

    // Очищаем сцену
    ui->graphicsView->scene()->clear();
    objects.clear();
    agents.clear();

    // Загружаем объекты
    QJsonArray objectsArray = root["objects"].toArray();
    for (const QJsonValue &value : objectsArray) {
        QJsonObject obj = value.toObject();
        QSharedPointer<QGraphicsItem> item(ui->graphicsView->scene()->addRect(0, 0, 50, 50, QPen(Qt::black), QBrush(Qt::blue)));
        item->setPos(obj["x"].toDouble(), obj["y"].toDouble());
        item->setData(Qt::UserRole, obj["name"].toString());
        objects.append(item);
    }

    // Загружаем агентов
    QJsonArray agentsArray = root["agents"].toArray();
    for (const QJsonValue &value : agentsArray) {
        QJsonObject obj = value.toObject();
        QGraphicsItem *item = ui->graphicsView->scene()->addRect(0, 0, 50, 50, QPen(Qt::red), QBrush(Qt::yellow));
        item->setPos(obj["x"].toDouble(), obj["y"].toDouble());
        item->setData(Qt::UserRole, obj["name"].toString());
        Agent *agent = new Agent(item, this);
        agents.append(agent);
        connect(subject, &Subject::notify, agent, &Agent::update); // Подписываем агента на сигнал
    }
}

void MainWindow::startSimulation() {
    if (isSimulationRunning) return;
    isSimulationRunning = true;
}

void MainWindow::stopSimulation() {
    isSimulationRunning = false;
}

QGraphicsPathItem* clonePathItem(const QGraphicsPathItem* original) {
    if (!original) return nullptr;
    QGraphicsPathItem* clone = new QGraphicsPathItem();
    clone->setPath(original->path());
    clone->setPen(original->pen());
    clone->setBrush(original->brush());
    clone->setFlags(original->flags());
    return clone;
}
void MainWindow::addObjectToToolbar(QSharedPointer<QGraphicsPathItem> item, const QString &name, bool hasCollision) {
    QToolButton* button = new QToolButton(this);
    button->setText(name);
    connect(button, &QToolButton::clicked, this, [this, item]() {
        QSharedPointer<QGraphicsPathItem> pathItem = QSharedPointer<QGraphicsPathItem>(clonePathItem(item.get()));
        if (pathItem){
        pathItem->setFlag(QGraphicsItem::ItemIsMovable);
        ui->graphicsView->scene()->addItem(pathItem.get());
        objects.append(pathItem);
        }
    });
    ui->objectToolBar->addWidget(button);
}


void MainWindow::addAgentToToolbar(Agent *agent, const QString &name) {
    QToolButton *button = new QToolButton(this);
    button->setText(name);
    connect(button, &QToolButton::clicked, this, [this, agent]() {
        Agent *newAgent = new Agent(agent->getItem(), this);
        agents.append(newAgent);
        connect(subject, &Subject::notify, newAgent, &Agent::update); // Подписываем нового агента на сигнал
        ui->graphicsView->scene()->addItem(newAgent->getItem());
    });
    ui->agentToolBar->addWidget(button);
}

MainWindow::~MainWindow()
{
    // В MainWindow
        qDebug() << "MainWindow уничтожен!" << this;

    delete ui;
}
