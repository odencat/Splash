#ifndef FILELOADER_H
#define FILELOADER_H

#include "json/reader.h"
#include "json/writer.h"
#include <QString>

class FileLoader
{
public:
    static Json::Value loadInitData();
    static void saveInitData(Json::Value newOptionalData);

    static std::string getFileData(QString path);
    static Json::Value loadJsonFile(QString path);
    static Json::Value load(QString filePath);

private:


};

#endif // FILELOADER_H
