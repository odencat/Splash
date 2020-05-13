#ifndef FILELOADER_H
#define FILELOADER_H

#include "json/reader.h"
#include "json/writer.h"
#include <QString>

class FileLoader
{
public:
    static Json::Value loadProject(QString filename);
    static void saveInitData(Json::Value newOptionalData);

    static std::string getFileData(QString path);
    static Json::Value loadJsonFile(QString path);
    static Json::Value load(QString filePath);
    static QString loadProjectPath();
    static void saveProjectPath(QString path);

private:


};

#endif // FILELOADER_H
