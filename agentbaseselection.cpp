#include "agentbaseselection.h"
#include "ui_agentbaseselection.h"

AgentBaseSelection::AgentBaseSelection(MainWindow *parent)
    : QDialog(parent)
    , ui(new Ui::AgentBaseSelection)
{
    ui->setupUi(this);
    auto objects = parent->getObjects();
    for (auto object : objects)
    {

    }
}

AgentBaseSelection::~AgentBaseSelection()
{
    delete ui;
}
