#ifndef CONFUSER_H
#define CONFUSER_H

#include<QString>
#include "qtextdocument.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <set>

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
    void init();
    QJsonValue getJsonValue(const char* key);
public:
    void save();

//---------------- findFlags
private:
    const char* keyFindFlags;
    QTextDocument::FindFlags findFlags;
    int findFlagsInt;
    void parseFindFlags();

public:
    QTextDocument::FindFlags getFindFlags();
    void setFindFlags(QTextDocument::FindFlags flags);

//------------------ openHistory
private:
    const char* keyOpenHistory;
    const int maxHistoryCnt;
    std::set<QString> openHistory;
    void parseOpenHistory();

public:
    QString getLastOpenFilePath(bool checkExist = true);
    void setLastOpenFilePath(QString path);

};

#endif // CONFUSER_H
