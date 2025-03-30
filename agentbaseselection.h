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

    QString getName() const;

    QString getCode() const;

private:

    Ui::AgentBaseSelection *ui;
    const QHash<QString,QPainterPath>& objects;
    QGraphicsPathItem* currentObject;
    QString name;
    QString code;

    void okButtonPress();
    void OnSelectionUpdate();
    void updatePreview(const QPainterPath &newObject);
};

#endif // AGENTBASESELECTION_H
