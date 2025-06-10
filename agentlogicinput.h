#ifndef AGENTLOGICINPUT_H
#define AGENTLOGICINPUT_H

#include <QDialog>

class QsciScintilla;
namespace Ui {
class AgentLogicInput;
}

class AgentLogicInput : public QDialog
{
    Q_OBJECT

public:
    explicit AgentLogicInput(QWidget *parent = nullptr);
    ~AgentLogicInput();

    QString getCode() const;

private:
    Ui::AgentLogicInput *ui;
    QsciScintilla* editor;
    void loadLogicButtonPress();
};

#endif // AGENTLOGICINPUT_H
