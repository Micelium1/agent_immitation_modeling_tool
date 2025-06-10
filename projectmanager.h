#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QDialog>

namespace Ui {
class ProjectManager;
}

class ProjectManager : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectManager(QWidget *parent = nullptr);

    ~ProjectManager();

    QString projectPath() const;
    bool isNew() const {return m_isNew;};

private:
    Ui::ProjectManager *ui;
    QString m_projectPath;


    void newProjectButtonPress();
    void loadProjectButtonPress();
    bool isProjectFolder(const QString &path);
    bool m_isNew = false;
};

#endif // PROJECTMANAGER_H
