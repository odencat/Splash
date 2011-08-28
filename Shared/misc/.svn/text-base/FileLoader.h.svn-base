#ifndef FILELOADER_H
#define FILELOADER_H

#include "json/reader.h"
#include "json/writer.h"
#include <QString>

class FileLoader
{
public:
    static std::string getFileData(QString path);
    static Json::Value loadJsonFile(QString path);

    static void saveInitData(QString filePath);
    static void saveInitData(QString filePath, Json::Value newOptionalData);
    static void save(QString filePath, Json::Value root);
    static Json::Value load(QString filePath);
    static Json::Value getInitOptionData();
    static QString getInitpath();

private:
    static void loadOptionData();
    static void saveInitFile(QString targetpath);
    static void saveInitFile(QString targetpath, Json::Value newOptionalData);
    static Json::Value getInitData();
};

#endif // FILELOADER_H
