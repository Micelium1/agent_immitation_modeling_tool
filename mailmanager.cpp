#include "mailmanager.h"

MailManager::MailManager() {}

QJsonObject MailManager::formMail(QString sender, QString message)
{
    QJsonObject mail;
    mail["sender"] = sender;
    mail["message"] = message;
    return mail;
}
