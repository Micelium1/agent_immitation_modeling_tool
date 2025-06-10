#include "agentlogicinput.h"
#include "qmessagebox.h"
#include "ui_agentlogicinput.h"
#include <Qsci/qsciscintilla.h>
#include <QSci/qscilexerpython.h>
#include <QFileDialog>

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
    editor->setText("'''Введите логику агента\nЗдесь указаны способы взаимодействия с симуляцией\n"
                    "   Список команд:\n"
                    "agent.move(dx,dy) -- сдвинуться на dx и dy\n"
                    "agent.set_position(x, y) -- установить позицию x,y\n"
                    "agent.get_position() -- узнать позицию агента в формате dict['x'] = x; dict['y'] = y\n"
                    "agent.set_color(r,g,b) -- установить цвет агента в формате RGB\n"
                    "agent.get_color() -- узнать цвет агента в формате dict['r','g','b']"
                    "agent.rotate(angle) -- повернуть агента на angle\n"
                    "agent.set_rotation(angle) -- установить угол поворота агента\n"
                    "agent.get_rotation() -- узнать угол поворота агента"
                    "agent.set_view_angle(angle) -- установить угол обзора агента от 0 до 360 градусов\n"
                    "agent.get_view_angle() -- узнать угол обзора агента\n"
                    "agent.set_view_distance(distance) -- установить радиус видимости\n"
                    "agent.get_view_distance() -- получить радиус видимости\n"
                    "agent.get_visible_objects(state) -- список видимых объектов и агентов, а так же дистанция и угол до них\n"
                    "   Анализирует игровое состояние и возвращает объекты/агентов в поле зрения текущего агента.\n"
                    "   Результат разделяется на две категории: видимые агенты и видимые объекты.\n"
                    "\n"
                    "   Args:\n"
                    "       state: Union[str, dict] - Игровое состояние в формате:\n"
                    "   - JSON-строка\n"
                    "   - Распарсенный словарь со структурой:\n"
                    "   {\n"
                    "          agents: {\n"
                    "              id1: {агент1_data},\n"
                    "              id2: {агент2_data},\n"
                    "              ...\n"
                    "          },\n"
                    "          objects: {\n"
                    "              id1: {объект1_data},\n"
                    "                      ...\n"
                    "          }\n"
                    "      }\n"
                    "      Где данные агента/объекта содержат как минимум:\n"
                    "      {\n"
                    "          id: int,\n"
                    "          position: {x: float, y: float},\n"
                    "          ...\n"
                    "      }\n"
                    "\n"
                    "      Returns:\n"
                    "      dict: {\n"
                    "          agents: [\n"
                    "              {\n"
                    "                  id: int,             # ID агента\n"
                    "                  type: agent,       # Тип сущности\n"
                    "                  name: str,           # Имя агента\n"
                    "                  distance: float,     # Расстояние до агента\n"
                    "                  angle: float,        # Абсолютный угол к агенту (0-360°)\n"
                    "                  relative_angle: float, # Угол относительно направления (-180°..180°)\n"
                    "                  position: {x: float, y: float},\n"
                    "                  shape: dict          # Геометрические параметры\n"
                    "              },\n"
                    "              ...\n"
                    "          ],\n"
                    "          objects: [\n"
                    "           Аналогичная структура для объектов\n"
                    "              {\n"
                    "                  type: object\n"
                    "                  ...\n"
                    "              },\n"
                    "              ...\n"
                    "          ]\n"
                    "      }\n"
                    "      Note:\n"
                    "      - Текущий агент исключается из результатов\n"
                    "   - Результаты сортируются по расстоянию (от ближнего к дальнему)\n"
                    "   - Только объекты/агенты в пределах view_distance и view_angle\n"
                    "calculate_relative_position(pos1, pos2, angle1_deg=0) -- Универсальная функция для расчета относительного положения между двумя точками\n"
                    "   Возвращает словарь с информацией о расстоянии и углах\n"
                    "\n"
                    "Args:\n"
                    "   pos1: dict {x: x1, y: y1} - позиция первого объекта\n"
                    "   pos2: dict {x: x2, y: y2} - позиция второго объекта\n"
                    "   angle1_deg: float - угол направления первого объекта в градусах (необязательный)\n"

                    "Returns:\n"
                    "   dict: {\n"
                    "       distance: float,  # расстояние между объектами\n"
                    "           dx: float,        # разница по x (pos2.x - pos1.x)\n"
                    "           dy: float,        # разница по y (pos2.y - pos1.y)\n"
                    "           absolute_angle: float,  # абсолютный угол от pos1 к pos2 (0-360°)\n"
                    "           relative_angle: float   # угол относительно направления pos1 (-180° до +180°)\n"
                    "}\n'''\n"
                    "def process_tick(state): ");
    connect(ui->loadButton,&QPushButton::clicked,this,&AgentLogicInput::loadLogicButtonPress);
}

AgentLogicInput::~AgentLogicInput()
{
    delete ui;
}

QString AgentLogicInput::getCode() const
{
    return editor->text();
}

void AgentLogicInput::loadLogicButtonPress()
{
    if (!editor) {
        qWarning("QsciScintilla editor is null!");
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
        editor,
        "Выберите файл для загрузки",
        QDir().dirName(),
        "Все файлы (*.*);;Текстовые файлы (*.txt);;Python (*.py)"
        );
    if (filePath.isEmpty()) {
        return;
    }
    QFile code_file(filePath);
    if (!code_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this,"Ошибка открытия файла","Не удалось открыть файл" + filePath);
    }
    QTextStream in(&code_file);
    editor->setText(in.readAll());
    code_file.close();
}
