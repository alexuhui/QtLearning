#include "confuser.h"
#include <QFile>
#include <QJsonObject>
#include <QDebug>
#include <QFileInfo>

ConfUser::ConfUser():
    defConf (":/conf/conf_user_def.json"),
    localConf ("./conf_user.json"),
    keyFindFlags("findFlags"),
    keyOpenHistory("openHistory"),
    maxHistoryCnt(5)
{
    init();
}

void ConfUser::init()
{
    // 读取JSON文件
    QFile localFile(localConf);
    if (localFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // 解析JSON数据
        QJsonParseError error;
        localJsonDoc = QJsonDocument::fromJson(localFile.readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug() << "本地用户配置JSON解析错误:" << error.errorString();
        }
    }

    // 读取JSON文件
    QFile defFile(localConf);
    if (defFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // 解析JSON数据
        QJsonParseError error;
        defJsonDoc = QJsonDocument::fromJson(defFile.readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug() << "默认用户配置JSON解析错误:" << error.errorString();
        }
    }

    // -- 文本查找规则
    parseFindFlags();
    // -- 历史打开文件路径
    parseOpenHistory();
}

QJsonValue  ConfUser::getJsonValue(const char* key)
{
    QJsonValue jvalue = localJsonDoc[key];
    if(jvalue.isNull())
    {
        jvalue = defJsonDoc[key];
    }
    return jvalue;
}

// -- 文本查找规则
void ConfUser::parseFindFlags()
{
    QJsonValue jsFindFlags = getJsonValue(keyFindFlags);
    int flagInt = jsFindFlags.toInt(0);
    auto flags = QTextDocument::FindFlags(flagInt);
    setFindFlags(flags);
}

void ConfUser::setFindFlags(QTextDocument::FindFlags flags)
{
    findFlags = flags;
    findFlagsInt = findFlags;
    localJsonObj[keyFindFlags] = findFlagsInt;
}

QTextDocument::FindFlags ConfUser::getFindFlags()
{
    return findFlags;
}

void ConfUser::parseOpenHistory()
{
    QJsonValue jsHistory = getJsonValue(keyOpenHistory);
    auto arr = jsHistory.toArray();
    foreach (auto item, arr) {
        openHistory.insert(item.toString());
    }
}

QString ConfUser::getLastOpenFilePath(bool checkExist)
{
    std::set<QString>::reverse_iterator rit = openHistory.rbegin();
    QFileInfo fileInfo;
    while(checkExist && !fileInfo.isFile() && rit != openHistory.rend())
    {
        fileInfo = *rit;
        rit ++;
    }
    return fileInfo.fileName();
}

void ConfUser::setLastOpenFilePath(QString path)
{
    openHistory.insert(path);
    int size = openHistory.size();
    if(size > maxHistoryCnt)
    {
        std::set<QString>::iterator it = openHistory.begin();
        openHistory.erase(it, it ++);
    }

    QJsonArray jarr;
    foreach (QString it, openHistory) {
        jarr.append(it);
        qDebug() << "history : " << it ;
    }
    localJsonObj[keyOpenHistory] = jarr;
}

void ConfUser::save()
{
    localJsonDoc.setObject(localJsonObj);
    QFile localFile(localConf);

    if (!localFile.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "本地用户配置JSON无法写入文件：" << localConf << " error:" << localFile.errorString();
        return;
    }
    QTextStream out(&localFile);
    out << localJsonDoc.toJson(QJsonDocument::Indented);
    auto path = QFileInfo(localFile).canonicalFilePath();
    qDebug() << "本地用户配置已保存：" << path;
}
