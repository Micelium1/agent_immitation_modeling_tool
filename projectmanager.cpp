#include "projectmanager.h"
#include "ui_projectmanager.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

ProjectManager::ProjectManager(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProjectManager)
{
    ui->setupUi(this);
    connect(ui->loadProjectButton,&QPushButton::clicked,this,&ProjectManager::loadProjectButtonPress);
    connect(ui->newProjectButton,&QPushButton::clicked,this,&ProjectManager::newProjectButtonPress);
}
bool ProjectManager::isProjectFolder(const QString& path) {
    QFile metaFile(path + "/project.json");
    if (!metaFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonObject meta = QJsonDocument::fromJson(metaFile.readAll()).object();
    return meta["type"] == "simulator_project";  // Проверяем тип
}

void ProjectManager::newProjectButtonPress()
{
    m_isNew = true;
    QString path = QFileDialog::getExistingDirectory(nullptr, "Выберите папку для проекта");
    if (path.isEmpty()) return;

    QString name = QInputDialog::getText(nullptr, "Название проекта", "Имя проекта:");
    if (name.isEmpty()) return;

    m_projectPath = path + "/" + name;
    QDir().mkpath(m_projectPath);
    QJsonObject meta;
    meta["type"] = "simulator_project";
    meta["name"] = name;
    meta["version"] = "1.0";

    QFile metaFile(m_projectPath + "/project.json");
    metaFile.open(QIODevice::WriteOnly);
    metaFile.write(QJsonDocument(meta).toJson());
    metaFile.close();
    this->accept();
}

void ProjectManager::loadProjectButtonPress()
{
    m_isNew = false;
    QString path = QFileDialog::getExistingDirectory(nullptr, "Выберите папку проекта");
    if (path.isEmpty() || !isProjectFolder(path)) {
        QMessageBox::warning(nullptr, "Ошибка", "Это не папка проекта или она повреждена!");
        return;
    }
    m_projectPath = path;
    this->accept();
}

ProjectManager::~ProjectManager()
{
    delete ui;
}

QString ProjectManager::projectPath() const
{
    return m_projectPath;
}



