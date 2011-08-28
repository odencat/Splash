#ifndef ANIMATIONMODEL_H
#define ANIMATIONMODEL_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QModelIndex>
#include <QDir>

#include "KeyFrameData.h"
#include "GLSprite.h"
#include "DataModels/KeyFrame.h"

class AnimationModel : public QObject
{
Q_OBJECT

public:

    enum
    {
        TARGET_originX = -150,
        TARGET_originY = 0
    };

    enum
    {
        LINE_target = 8,
        LINE_camera = 9,

        LINE_COUNT = 10
    };

    struct EventList
    {
        QList<QString> mList;
    };

    static GLSprite* getTargetSprite();
    static GLSprite* getCenterPointSprite();

    AnimationModel(QWidget* parent);
    AnimationModel(QWidget* parent, QString animationDir, QString animationID);

    ~AnimationModel();
    
    void setup();

    void setTargetSpritePosition(float x, float y);

    void clearPixmapHash();

    int getLastEventFrameNo();
    int getMaxFrameCount(int lineNo) const;
    int getMaxFrameCount() const;

    EventList getEventList(int frameNo) const;
    void setEventList(int frameNo, EventList eventList);

    void setEventText(int frameNo, int index, QString text);

    KeyFrame* getKeyFrame(int lineNo, int frameNo) const;

    int getKeyFrameIndex(int lineNo, int frameNo) const;

    bool isKeyData(KeyFrameData::TweenAttribute tweenAttribute, const KeyFrame* pKeyframe) const;
    int getPreviousKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;
    int getNextKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;
    int getSubanimationStartKeyFrameIndex(int lineNo, int frameNo) const;

    void addEvent(int frameNo);
    void removeEvent(int frameNo, int index);

    void setKeyFrame(int lineNo, int frameNo, const GLSprite::Point2& position);
    void setKeyFrame(int lineNo, int frameNo, KeyFrameData* pKeyframeData);

    void insertEmptyKeyFrame(int lineNo, int frameNo);
    void addFrameLength(int lineNo, int frameNo, int value);
    void reduceFrameLength(int lineNo, int frameNo);

    void clearFrames(int lineNo, int startFrameNo, int endFrameNo);
    void clearAllKeyFrames();

    const QList<KeyFrame*>& getKeyFrameList(int lineNo) const;

    void setSelectedSourcePath(QString sourcePath);
    QString getSelectedSourcePath() const;

    KeyFrame::KeyFramePosition getCurrentKeyFramePosition();
    void selectCurrentKeyFramePosition(int lineNo, int frameNo);

    void tellTimeLineToRefresh();

    // GLSprite related functions
    GLSprite* createGLSpriteAt(const GLSprite* parentGLSprite, int frameNo, int lineNo) const;
    GLSprite* createGLSpriteAt(const GLSprite* parentGLSprite, QList<KeyFrame::KeyFramePosition>nodePath) const;
    const QList<const GLSprite*> createGLSpriteListAt(const GLSprite* parentGLSprite, int frameNo) const;
    bool copyTweenedAttribute(const GLSprite* parentGLSprite, GLSprite::SpriteDescriptor& spriteDescriptor, int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;

    // Save & Load
    bool saveData();
    void loadLine(int lineNo, Json::Value& line);
    bool loadData(QString path);
    QString getLoadedAnimationPath() const;

    // event command
    void executeCommand(int frameNo);

    void createEmptyAnimation(QString path);

    // public member variables
    QRect mSelectedPaletTextureSrcRect;

    const QWidget* mpParent;

private:
    void readCommand(QString command);
    QString mAnimationName;
    QString mAnimationDirectory;
    QString mAnimationID;
    QString mOriginalAnimationID;

    void setFinalAlpha(const GLSprite* parentGLSprite, GLSprite::SpriteDescriptor& spriteDescriptor) const;
    void setFinalPosition(const GLSprite* parentGLSprite, GLSprite::SpriteDescriptor& spriteDescriptor) const;
    void setFinalRotation(const GLSprite* parentGLSprite, int lineNo, int frameNo, GLSprite::SpriteDescriptor& spriteDescriptor) const;
    void tweenElement(GLSprite::SpriteDescriptor& spriteDescriptor, KeyFrameData::TweenAttribute tweenAttribute, KeyFrameData::TweenType tweenType, GLSprite::SpriteDescriptor& startDescriptor, GLSprite::SpriteDescriptor& endDescriptor, int lineNo, int frameNo, int startFrameNo, int endFrameNo) const;
    GLSprite* tweenFrame(const GLSprite* parentGLSprite, int lineNo, int frameNo) const;

    // Key Frames
    QList<KeyFrame*> mKeyFrames[LINE_COUNT];
    QHash<int, EventList> mEvents;

    QString mSelectedSourcePath;

    KeyFrame::KeyFramePosition mSelectedKeyFramePosition;
    KeyFrameData* mpTargetCel;


public slots:
    void setAnimationName(QString name);
    void setAnimationID(QString id);

signals:
    void animationNameChanged(QString animationName);
    void animationIDChanged(QString animationName);

    void selectedPaletChanged(QString path);
    void animationDurationChanged(int length);
    void targetPositionMoved(int x, int y);

    void refreshTimeLine();
    void animationLoaded();
    void fileChanged();
    void selectedKeyFramePositionChanged(int lineNo, int frameNo);
};
extern GLSprite* spTargetSprite;

#endif // ANIMATIONMODEL_H
