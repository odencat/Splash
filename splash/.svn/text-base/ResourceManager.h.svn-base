#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QString>
#include "json/reader.h"

#define ANIMATION_FORMAT "ani"
#define IMAGE_FORMAT "png"
#define SOUND_FORMAT "ogg"

#define ANIMATION_DIR QDir(QString("GameData/Animations"))
#define ANIMATION_IMAGE_DIR QDir(QString("Images"))
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
        FileType_Invalid,
        FileType_Image,
        FileType_Animation
    };
    ResourceManager();

    static QImage* getImage(QString path, bool recache = false);
    static void clearImageCache();
    static AnimationModel* getAnimation(QString path);
    static void clearAnimationCache();
    static QString getResourcePath(QString path);
    static FileType getFileType(QString path);
    static void playSound(QString path);

    static QString loadWorkingDirectory();
    static void setWorkingDirectory(QString newDirectory);
};

#endif // RESOURCEMANAGER_H
