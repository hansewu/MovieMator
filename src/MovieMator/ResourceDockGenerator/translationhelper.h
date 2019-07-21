#ifndef TRANSLATIONHELPER_H
#define TRANSLATIONHELPER_H

#include <QJsonObject>

class TranslationHelper
{
public:
    TranslationHelper(){}
    ~TranslationHelper(){}

    static void readJsonFile(QString strFilePath, QJsonObject &jsonObj);
    // 根据字符串 srcStr从 translationInfo中寻获取对应的翻译内容
    static QString getTranslationStr(QString srcStr, QJsonObject translationInfo);
};


#endif // TRANSLATIONHELPER_H
