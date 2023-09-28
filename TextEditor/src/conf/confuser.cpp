#include "confuser.h"
#include <QFile>
#include <QJsonObject>
#include <QDebug>
#include <QFileInfo>

ConfUser::ConfUser():
    defConf (":/conf/conf_user_def.json"),
    localConf ("./conf_user.json"),
    keyFindFlags("findFlags")
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
}

// -- 文本查找规则
void ConfUser::parseFindFlags()
{
    QJsonValue jsFindFlags = localJsonDoc[keyFindFlags];
    if(jsFindFlags.isNull())
    {
        jsFindFlags = defJsonDoc[keyFindFlags];
    }
    int flagInt = jsFindFlags.toInt(0);
    auto flags = QTextDocument::FindFlags(flagInt);
    setFindFlags(flags);
}

void ConfUser::setFindFlags(QTextDocument::FindFlags flags)
{
    findFlags = flags;
    findFlagsInt = findFlags;
    localJsonObj[keyFindFlags] = findFlagsInt;
    qDebug() << "findFlagsInt : " << findFlagsInt << " findFlags : " << findFlags;
}

QTextDocument::FindFlags ConfUser::getFindFlags()
{
    return findFlags;
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
