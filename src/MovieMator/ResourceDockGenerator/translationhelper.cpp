#include "translationhelper.h"

#include <QFile>
#include <Logger.h>
#include <settings.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>


void TranslationHelper::readJsonFile(QString strFilePath, QJsonObject &jsonObj)
{
    QFile jsonFile(strFilePath);
    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"sll-----could't open"<<strFilePath;
        return;
    }

    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData, &json_error));
    if (json_error.error != QJsonParseError::NoError)
    {
        qDebug()<<"sll-----json error!";
        return;
    }

    jsonObj = jsonDoc.object();
}
// 根据字符串 srcStr从 translationInfo中寻获取对应的翻译内容
QString TranslationHelper::getTranslationStr(QString srcStr, QJsonObject translationInfo)
{
    if (translationInfo.isEmpty())
    {
        qDebug()<<"sll-----QJsonObject is empty!";
        return srcStr;
    }

    QString result = srcStr;
    if (translationInfo.contains(srcStr))
    {
        QJsonObject subObj  = translationInfo.value(srcStr).toObject();
        QString strLanguage = Settings.language();

        if((strLanguage == "zh") || (strLanguage == "zh_CN"))
        {
            strLanguage = "zh_CN";
        }
        else
        {
            strLanguage = "en";
        }
        result = subObj.value(strLanguage).toString();

        if (result.isEmpty())
        {
            result = subObj.value("en").toString();
        }
    }

    return result;
}
