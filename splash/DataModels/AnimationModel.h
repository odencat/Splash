#ifndef ANIMATIONMODEL_H
#define ANIMATIONMODEL_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QModelIndex>
#include <QDir>

#include "KeyFrameData.h"
#include "AnimationPlayer/GLSprite.h"
#include "DataModels/KeyFrame.h"

class AnimationModel : public QObject
{
Q_OBJECT

public:
    enum AnimationType
    {
        AnimationType_Normal,
        AnimationType_Background,

        AnimationType_COUNT
    };

    enum
    {
        TARGET_originX = -50,
        TARGET_originY = 0
    };

    struct EventList
    {
        QList<QString> mList;
    };

    static QString animationTypeSting[AnimationType::AnimationType_COUNT];
    static QSize animationTypeSize[AnimationType::AnimationType_COUNT];
    static int animationTypeDefaultZoom[AnimationType::AnimationType_COUNT];

    static GLSprite* getTargetSprite();
    static GLSprite* getCenterPointSprite();

    AnimationModel(QWidget* parent);
    AnimationModel(QWidget* parent, QString animationDir, QString animationID);

    ~AnimationModel();

    void setup();

    void setTargetSpritePosition(float x, float y);

    void clearPixmapHash();

    int getLastEventFrameNo();

    int getLineCount() const;
    int getMaxFrameCount(int lineNo) const;
    int getMaxFrameCount() const;
    bool isDataChanged() const;

    EventList getEventList(int frameNo) const;
    void setEventList(int frameNo, EventList eventList);

    void setEventText(int frameNo, int index, QString text);
    QSize guideSize() const;
    KeyFrame* getKeyFrame(int lineNo, int frameNo) const;

    int getKeyFrameIndex(int lineNo, int frameNo) const;

    bool isKeyData(KeyFrameData::TweenAttribute tweenAttribute, const KeyFrame* pKeyframe) const;
    int getPreviousKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;
    int getNextKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;
    int getSubanimationStartKeyFrameIndex(int lineNo, int frameNo) const;

    void addEvent(int frameNo);
    void removeEvent(int frameNo, int index);

    void setKeyFrame(int lineNo, int frameNo, const Point2& position);
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
    void moveTimeLine(int oldIndex, int newIndex);

    // GLSprite related functions
    GLSprite* createGLSpriteAt(const GLSprite* parentGLSprite, int frameNo, int lineNo) const;
    GLSprite* createGLSpriteAt(const GLSprite* parentGLSprite, QList<KeyFrame::KeyFramePosition>nodePath) const;
    const QList<const GLSprite*> createGLSpriteListAt(const GLSprite* parentGLSprite, int frameNo) const;
    void copyTweenedAttribute(const GLSprite* parentGLSprite, SpriteDescriptor& spriteDescriptor, int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;

    // Save & Load
    bool saveData();
    QList<KeyFrame*> loadLine(int lineNo, Json::Value& line);
    bool loadData(QString path);
    QString getLoadedAnimationPath() const;

    // event command
    void executeCommand(int frameNo);

    void createEmptyAnimation(QString path);

    // public member variables
    QRect mSelectedPaletTextureSrcRect;

    const QWidget* mpParent;

private:
    enum
    {
        MIN_LINE_COUNT = 10
    };

    void readCommand(QString command);
    QString mAnimationName;
    QString mAnimationDirectory;
    QString mAnimationID;
    QString mOriginalAnimationID;

    void setFinalAlpha(const GLSprite* parentGLSprite, SpriteDescriptor& spriteDescriptor) const;
    void setFinalPosition(const GLSprite* parentGLSprite, SpriteDescriptor& spriteDescriptor) const;
    void setFinalRotation(const GLSprite* parentGLSprite, int lineNo, int frameNo, SpriteDescriptor& spriteDescriptor) const;
    template <class T> T tweenValue(KeyFrameData::TweenType tweenType, T startValue, T endValue, int frameNo, int startFrameNo, int endFrameNo) const;
    void tweenElement(SpriteDescriptor& spriteDescriptor, KeyFrameData::TweenAttribute tweenAttribute, KeyFrameData::TweenType tweenType, SpriteDescriptor& startDescriptor, SpriteDescriptor& endDescriptor, int frameNo, int startFrameNo, int endFrameNo) const;

    // Key Frames
    QList<QList<KeyFrame*>> mTimeline;
    QHash<int, EventList> mEvents;

    QString mSelectedSourcePath;

    KeyFrame::KeyFramePosition mSelectedKeyFramePosition;
    KeyFrameData* mpTargetCel;
    AnimationType mAnimationType;
    bool mIsDataChanged;

public slots:
    void setAnimationName(QString name);
    void setAnimationID(QString id);
    void setAnimationType(int index);

signals:
    void animationNameChanged(QString animationName);
    void animationIDChanged(QString animationName);
    void animationTypeChanged(int index);

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
