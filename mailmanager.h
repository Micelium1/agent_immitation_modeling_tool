#ifndef MAILMANAGER_H
#define MAILMANAGER_H
#include <QString>
#include <QJsonObject>
class MailManager
{
public:
    MailManager();
    static QJsonObject formMail(QString sender,QString message);
};

#endif // MAILMANAGER_H
