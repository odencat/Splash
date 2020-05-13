#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QString>
#include "json/reader.h"

#define ANIMATION_FORMAT "ani"
#define IMAGE_FORMAT "png"
#define SOUND_FORMAT "ogg"

#define SOUND_DIR QDir(QString("Sounds"))

class AnimationModel;
class GLSprite;
class QWidget;
class QImage;

class ResourceManager
{
public:
    enum FileType
    {
        FileType_Invalid = -1,
        FileType_Image = 0,
        FileType_Animation = 1,

        FileType_COUNT
    };
    ResourceManager();

    static QString GetDirectoryId(FileType fileType);
    static QString GetDirectoryPath(FileType fileType);
    static QImage* getImage(QString path, bool recache = false);
    static void clearImageCache();
    static AnimationModel* getAnimation(QString path);
    static void clearAnimationCache();
    static QString getResourcePath(QString path, ResourceManager::FileType fileType);
    static FileType getFileType(QString path);
    static void playSound(QString path);

    static void loadWorkingDirectory();
    static void setWorkingDirectory(QString newDirectory, FileType fileType);

    static void openSavedProject();
    static bool openProject(QString projectPath);
    static void setProjectPath(QString filename);
};

#endif // RESOURCEMANAGER_H
