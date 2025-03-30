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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    const QHash<QString, QPainterPath>& getObjects() const;

private:
    void createObject();
    void createAgent();
    void saveSimulation();
    void loadSimulation();
    void startSimulation();
    void stopSimulation();
    void addObjectToToolbar(const QString& name, bool hasCollision);
    void addAgentToToolbar(const QString &agentName = "default");

    QHash<QString,AgentFactory> agentFactories;
    QHash<QString,QPainterPath> shapes;
    QHash<long int,QSharedPointer<Object>> objects;
    QHash<long int,QSharedPointer<Agent>> agents;

    Subject *subject;
    bool isSimulationRunning;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
