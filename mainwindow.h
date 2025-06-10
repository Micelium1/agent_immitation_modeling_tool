#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QToolBar>
#include <QList>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QGraphicsItem>

#include "agentfactory.h"
#include "subject.h"
#include "agent.h"
#include "objecteditor.h"
#include "object.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString ProjectPath,QWidget *parent = nullptr);
    ~MainWindow();

    const QHash<QString, QPainterPath>& getObjects() const;
    void setProjectDir(const QString&& path);


    QDir projectDir() const { return QDir(m_projectPath); };
    QDir objectsDir() const { return projectDir().filePath("objects");};
    QDir agentsDir() const { return projectDir().filePath("agents");};
    QDir pythonDir() const { return projectDir().filePath("python");};
    QFile stateFile() const {return projectDir().filePath("state.json");};

    long getObjectsId() {return objectsIdCounter.fetch_add(1);};
    long getAgentsId() {return agentsIdCounter.fetch_add(1);};

private:
    void createObject();
    void createAgent();

    void saveProject(QString projectPath);
    void loadProject(QString projectPath);

    void clearProject();
    void clearToolBar(QToolBar* bar);

    void loadObjectsFromFiles();
    void loadAgentsFromFiles();
    void loadStateFromFile();

    void inline saveProjectButtonPress();
    void loadProjectButtonPress();

    void startSimulation();
    void stopSimulation();

    void spawnObject(QString name, QPointF pos = QPointF(0,0));
    void spawnAgent(QString name, QPointF pos = QPointF(0,0));

    void addObjectToToolbar(const QString &name);
    void addAgentToToolbar(const QString &agentName);

    void loadObjectFromJson(const QJsonObject);

    void onAgentResponse(const QJsonObject& comand);

    QJsonObject getCurrentSimulationState();

    void setObjectsIdCounter(long new_start) {objectsIdCounter = new_start;};
    void setAgentsIdCounter(long new_start) {agentsIdCounter = new_start;};

    QHash<QString,AgentFactory> agentFactories;
    QHash<QString,QPainterPath> shapes;

    QHash<long int,QSharedPointer<Object>> objects;
    QHash<long int,QSharedPointer<Agent>> agents;


    std::atomic<long> objectsIdCounter;
    std::atomic<long> agentsIdCounter;

    Subject *subject;
    bool isSimulationRunning;
    QString m_projectPath;
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
