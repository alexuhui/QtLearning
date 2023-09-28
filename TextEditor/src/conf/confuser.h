#ifndef CONFUSER_H
#define CONFUSER_H

#include<QString>
#include "qtextdocument.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


class ConfUser
{
public:
    ConfUser();

private:
    const QString defConf;
    const QString localConf;
    QJsonDocument defJsonDoc;
    QJsonDocument localJsonDoc;
    QJsonObject localJsonObj;

private:
    const char* keyFindFlags;
    QTextDocument::FindFlags findFlags;
    int findFlagsInt;

private:
    void init();
    void parseFindFlags();

public:
    QTextDocument::FindFlags getFindFlags();
    void setFindFlags(QTextDocument::FindFlags flags);
    void save();

};

#endif // CONFUSER_H
