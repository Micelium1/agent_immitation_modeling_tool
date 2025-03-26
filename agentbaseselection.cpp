#include "agentbaseselection.h"
#include "ui_agentbaseselection.h"

AgentBaseSelection::AgentBaseSelection(MainWindow *parent)
    : QDialog(parent)
    , ui(new Ui::AgentBaseSelection)
{
    ui->setupUi(this);
}

AgentBaseSelection::~AgentBaseSelection()
{
    delete ui;
}
