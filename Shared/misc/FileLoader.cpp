#include "FileLoader.h"

#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <QApplication>
#include <QStandardPaths>

// public
Json::Value FileLoader::loadProject(QString filename)
{
    QFileInfo initFileInfo = QFileInfo(filename);
    QString filePath = initFileInfo.absoluteFilePath();

    return loadJsonFile(filePath);
}

void FileLoader::saveInitData(Json::Value newOptionalData)
{
    QFileInfo initFileInfo = QFileInfo(QDir::currentPath(), QString("init.json"));
    QString filePath = initFileInfo.absoluteFilePath();

    Json::StyledWriter writer;
    std::string outputJson = writer.write(newOptionalData);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &file );
        file.write(outputJson.c_str(), outputJson.length());
        file.close();
    }
}

std::string FileLoader::getFileData(QString path)
{
    // Get json file data
    QFile file(path);
   printf("PATH = %s\n", path.toStdString().c_str());

    QString fileData;
    if (file.open( QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            fileData = stream.readAll();
        }
        file.close();
    }
    return fileData.toStdString();
}

Json::Value FileLoader::loadJsonFile(QString path)
{
    std::string inputJson = getFileData(path);

    Json::Reader reader;
    Json::Value root;
    if(!reader.parse(inputJson, root))
    {
        printf("json parse error");
    }
    return root;
}

QString FileLoader::loadProjectPath()
{
 QString settingsFilePath = QStandardPaths::standardLocations(QStandardPaths::CacheLocation).last() + "/settings.ini";
 QSettings settings(settingsFilePath, QSettings::IniFormat);
 return settings.value("project_path", "").toString();
}

void FileLoader::saveProjectPath(QString path)
{
 QString settingsFileDir = QStandardPaths::standardLocations(QStandardPaths::CacheLocation).last();
 if (QDir().mkpath(settingsFileDir)) {
     QString settingsFilePath = settingsFileDir + "/settings.ini";
     QSettings settings(settingsFilePath, QSettings::IniFormat);
     settings.setValue("project_path", path);
     settings.sync();
 } else {
     printf("Failed to create path: %s", settingsFileDir.toStdString().c_str());
 }
}
