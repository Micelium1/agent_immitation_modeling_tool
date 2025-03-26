#include "objecteditor.h"
#include "ui_objecteditor.h"
#include "qgraphicsscene.h"
#include "objecteditor.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsPathItem>
#include <QMouseEvent>
#include <QDebug>
#include <QMessageBox>

CustomGraphicsView::CustomGraphicsView(QWidget *parent) : QGraphicsView(parent) {
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::NoAnchor);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event) {
    emit mousePressed(event);
    event->accept();
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    emit mouseMoved(event);
    event->accept();
}

void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    emit mouseReleased(event);
    event->accept();
}

ObjectEditor::ObjectEditor(QWidget *parent) :
    QDialog(parent),
    currentPathItem(nullptr),
    isDrawing(false),
    ui(new Ui::ObjectEditor) {

    // Графическая сцена
    ui->setupUi(this);
    CustomGraphicsView* customView = new CustomGraphicsView();

    // Настройка сцены
    QGraphicsScene *scene = new QGraphicsScene(this);
    customView->setScene(scene);
    ui->verticalLayout->replaceWidget(ui->view,customView);
    delete ui->view;
    ui->view = customView;
    // Явно устанавливаем размер сцены
    ui->view->scene()->setSceneRect(-1000, -1000, 2000, 2000);

    // Отключаем все автоматические трансформации
    ui->view->setTransformationAnchor(QGraphicsView::NoAnchor);
    ui->view->setResizeAnchor(QGraphicsView::NoAnchor);
    ui->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->view->setRenderHint(QPainter::Antialiasing);
    ui->view->setTransform(QTransform()); // Сбросить масштаб/поворот
    ui->view->setSceneRect(QRectF());    // Убедиться, что сцена не масштабируется



    connect(customView,&CustomGraphicsView::mousePressed,this,&ObjectEditor::mousePressEvent);
    connect(customView,&CustomGraphicsView::mouseMoved,this,&ObjectEditor::mouseMoveEvent);
    connect(customView,&CustomGraphicsView::mouseReleased,this,&ObjectEditor::mouseReleaseEvent);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ObjectEditor::okButtonPress);
}
void ObjectEditor::okButtonPress()
{
    if (currentPathItem) {
        currentPathItem->setFlag(QGraphicsItem::ItemIsMovable);
        accept();
    } else {
        QMessageBox::warning(this, "Ошибка", "Нарисуйте объект перед сохранением!");
    }
}
void ObjectEditor::mousePressEvent(QMouseEvent *event) {
    if (lastPoint.isNull())
    {
        if (event->button() == Qt::LeftButton && ui->view->underMouse())
        {
            qDebug() << "First start";
            isDrawing = true;
            currentPath = QPainterPath();
            QPointF scenePos = ui->view->mapToScene(event->pos());
            currentPath.moveTo(scenePos);
            currentPathItem = ui->view->scene()->addPath(currentPath, QPen(Qt::black, 2));
            currentPathItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        }
    }
    else
    {
        if (event->button() == Qt::LeftButton && ui->view->underMouse())
        {
            qDebug() << "Other start";
            QPointF scenePos = ui->view->mapToScene(event->pos());
            isDrawing = true;
            currentPath.lineTo(scenePos);
            currentPathItem->setPath(currentPath);
        }
    }
    event->accept();
}

void ObjectEditor::mouseMoveEvent(QMouseEvent *event) {
    if (isDrawing && ui->view->underMouse()) {
        QPointF scenePos = ui->view->mapToScene(event->pos());

        currentPath.lineTo(scenePos);
        currentPathItem->setPath(currentPath);
    }
    event->accept();
}

void ObjectEditor::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastPoint = ui->view->mapToScene(event->pos());
        isDrawing = false;
    }
    event->accept();
}
QGraphicsPathItem *ObjectEditor::getCreatedItem() const {
    QGraphicsPathItem *item = currentPathItem;
    return item;
}

QString ObjectEditor::getObjectName() const {
    return ui->nameEdit->text();
}

bool ObjectEditor::hasCollision() const {
    return ui->collisionCheckBox->isChecked();
}

ObjectEditor::~ObjectEditor()
{
    qDebug() << "ObjectEditor уничтожен!" << this;
    ui->view->scene()->removeItem(currentPathItem);
    delete ui;
}
