#include "agentbaseselection.h"
#include "agentlogicinput.h"
#include "ui_agentbaseselection.h"

AgentBaseSelection::AgentBaseSelection(MainWindow *parent)
    : QDialog(parent)
    , ui(new Ui::AgentBaseSelection)
    , objects(parent->getObjects())
{
    ui->setupUi(this);
    ui->objectList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->objectList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->objectList->addItems(objects.keys());

    ui->preview->setScene(new QGraphicsScene);
    currentObject = new QGraphicsPathItem;
    ui->preview->scene()->addItem(currentObject);
    connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&AgentBaseSelection::okButtonPress);
    connect(ui->objectList,&QListWidget::itemSelectionChanged,this,&AgentBaseSelection::OnSelectionUpdate);
}

void AgentBaseSelection::okButtonPress()
{
    auto selected = ui->objectList->selectedItems();
    if (!selected.isEmpty())
    {
        AgentLogicInput* inp = new AgentLogicInput();
        inp->exec();
        name = selected[0]->text();
        code = inp->getCode();
        accept();
    }
    else
    {
        QMessageBox::warning(this,"Предупреждение","Не выбран объект");
    }
}

void AgentBaseSelection::updatePreview(const QPainterPath& newObject)
{
    currentObject->setPath(newObject);
    ui->preview->scene()->update();
}

void AgentBaseSelection::OnSelectionUpdate()
{
    auto selected = ui->objectList->selectedItems();
    if (selected.empty()) {
        return;
    }
    QPainterPath object = objects[selected[0]->text()];
    updatePreview(object);
}

AgentBaseSelection::~AgentBaseSelection()
{
    delete ui;
}

QString AgentBaseSelection::getName() const
{
    return name;
}

QString AgentBaseSelection::getCode() const
{
    return code;
}
