#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include "qgraphicsview.h"
#include <QDialog>
#include <QDialog>
#include <QGraphicsItem>
#include <QPainterPath>

class CustomGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit CustomGraphicsView(QWidget *parent = nullptr);
signals:
    void mousePressed(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};


namespace Ui {
class ObjectEditor;
}


class QCheckBox;
class QLineEdit;
class QGraphicsScene;
class QGraphicsView;

class ObjectEditor : public QDialog {
    Q_OBJECT

public:
    explicit ObjectEditor(QWidget *parent = nullptr);
    QGraphicsPathItem *getCreatedItem() const;
    QString getObjectName() const;
    bool hasCollision() const;
    ~ObjectEditor();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


private:
    QGraphicsPathItem *currentPathItem;
    QPainterPath currentPath;
    bool isDrawing;
    Ui::ObjectEditor *ui;
    QPointF lastPoint;
    void okButtonPress();
};

#endif // OBJECTEDITOR_H
