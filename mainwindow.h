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

#include "subject.h"
#include "agent.h"
#include "objecteditor.h"

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

private:
    void createObject();
    void createAgent();
    void saveSimulation();
    void loadSimulation();
    void startSimulation();
    void stopSimulation();
    void addObjectToToolbar(QSharedPointer<QGraphicsPathItem> item, const QString &name, bool hasCollision);
    void addAgentToToolbar(Agent *agent, const QString &name);

    QList<Agent*> agents;
    QList<QSharedPointer<QGraphicsItem>> objects;

    Subject *subject;
    bool isSimulationRunning;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
