#ifndef AGENTBASESELECTION_H
#define AGENTBASESELECTION_H

#include "mainwindow.h"
#include <QDialog>

namespace Ui {
class AgentBaseSelection;
}

class AgentBaseSelection : public QDialog
{
    Q_OBJECT

public:
    explicit AgentBaseSelection(MainWindow *parent = nullptr);
    ~AgentBaseSelection();

private:

    Ui::AgentBaseSelection *ui;
};

#endif // AGENTBASESELECTION_H
