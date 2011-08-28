#include "FileLoader.h"

#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>

static Json::Value optionalData;

// private
void FileLoader::loadOptionData()
{
    Json::Value root = getInitData();
    optionalData = root["option"];
}

Json::Value FileLoader::getInitData()
{
    QFileInfo initFileInfo = QFileInfo(QDir::currentPath(), QString("init.json"));
    QString filePath = initFileInfo.absoluteFilePath();

    return loadJsonFile(filePath);
}

void FileLoader::saveInitFile(QString targetpath, Json::Value newOptionalData)
{
    QFileInfo initFileInfo = QFileInfo(QDir::currentPath(), QString("init.json"));
    QString filePath = initFileInfo.absoluteFilePath();

    Json::StyledWriter writer;
    Json::Value data;
    data["initFilePath"] = targetpath.toStdString();
    data["option"] = newOptionalData;
    std::string outputJson = writer.write(data);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &file );
        file.write(outputJson.c_str(), outputJson.length());
        file.close();
    }
}

// public
std::string FileLoader::getFileData(QString path)
{
    // Get json file data
    QFile file(path);
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

void FileLoader::saveInitData(QString filePath)
{
    Json::Value emptyValue;
    optionalData = emptyValue;
    saveInitFile(filePath, optionalData);
}

void FileLoader::saveInitData(QString filePath, Json::Value newOptionalData)
{
    optionalData = newOptionalData;
    saveInitFile(filePath, optionalData);
}

void FileLoader::save(QString filePath, Json::Value root)
{
    // Save to file
    Json::StyledWriter writer;
    std::string outputJson = writer.write(root);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &file );
        file.write(outputJson.c_str(), outputJson.length());
        file.close();
    }

    saveInitFile(filePath, optionalData);
}

Json::Value FileLoader::load(QString filePath)
{
    Json::Value root = loadJsonFile(filePath);

    if (!root.empty())
    {
        // load succeeded
        // update initfile
        saveInitFile(filePath, optionalData);
    }

    return root;
}

Json::Value FileLoader::getInitOptionData()
{
    loadOptionData();
    return optionalData;
}

// Get starting path
QString FileLoader::getInitpath()
{
    Json::Value root = getInitData();

    if (root.empty())
    {
        return "";
    }
    else
    {
        return QString::fromStdString(root["initFilePath"].asString());
    }
}
