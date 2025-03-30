#include "agentlogicinput.h"
#include "ui_agentlogicinput.h"
#include <Qsci/qsciscintilla.h>
#include <QSci/qscilexerpython.h>

AgentLogicInput::AgentLogicInput(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AgentLogicInput)
{
    ui->setupUi(this);
    editor = new QsciScintilla(this);
    QsciLexerPython *lexer = new QsciLexerPython(editor);
    editor->setLexer(lexer);
    ui->verticalLayout->insertWidget(0,editor);
    editor->setMarginLineNumbers(1, true);
    editor->setMarginWidth(1, "0000");
    editor->setText("'''Введите логику агента\nЗдесь указаны способы взаимодействия с симуляцией\n''' ");
}

AgentLogicInput::~AgentLogicInput()
{
    delete ui;
}

QString AgentLogicInput::getCode() const
{
    return editor->text();
}
