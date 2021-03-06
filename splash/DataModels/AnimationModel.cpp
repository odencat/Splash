#include "AnimationModel.h"
#include <QPixmap>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include "ResourceManager.h"
#include "Macros.h"
#include "json/writer.h"
#include "json/reader.h"
#include "KeyFrameData.h"
#include <math.h>

#define DEFAULT_SELECTION_RECT QRect(0, 0, 32, 32);

static bool sIsNesting = false;

QString AnimationModel::animationTypeSting[AnimationModel::AnimationType_COUNT] =
{
    "normal",
    "background"
};

QSize AnimationModel::animationTypeSize[AnimationModel::AnimationType_COUNT] =
{
    QSize(0, 0),
    QSize(288, 384)
};

int AnimationModel::animationTypeDefaultZoom[AnimationModel::AnimationType_COUNT] =
{
    200,
    100
};

static QPixmap* spTargetPixmap = NULL;
GLSprite* spTargetSprite = NULL;
static QPixmap* spCenterPointPixmap = NULL;
static GLSprite* spCenterPointSprite = NULL;


GLSprite* AnimationModel::getTargetSprite()
{
    return spTargetSprite;
}

GLSprite* AnimationModel::getCenterPointSprite()
{
    return spCenterPointSprite;
}

AnimationModel::AnimationModel(QWidget* parent)
    : mpParent(parent),
      mAnimationName(QString("")),
      mAnimationDirectory(QString("")),
      mAnimationID(QString("")),
      mOriginalAnimationID(QString("")),
      mAnimationType(AnimationType_Normal),
      mIsDataChanged(false)
{
    setup();
}

AnimationModel::AnimationModel(QWidget* parent, QString animationDir, QString animationID)
    : mpParent(parent),
      mAnimationName(QString("")),
      mAnimationDirectory(QString(animationDir)),
      mAnimationID(QString(animationID)),
      mOriginalAnimationID(QString(animationID)),
      mAnimationType(AnimationType_Normal),
      mIsDataChanged(false)
{
    setup();
}

void AnimationModel::setup()
{
    mSelectedSourcePath = "";
    mSelectedKeyFramePosition.mFrameNo = -1;
    mSelectedKeyFramePosition.mLineNo = -1;

    // Target sprite
    if (spTargetPixmap == NULL)
    {
        spTargetPixmap = new QPixmap(":/resource/target.png");
        SpriteDescriptor spriteDescriptor = SpriteDescriptor::makeDefaultSpriteDescriptor();

        spriteDescriptor.mTextureSrcRect = DEFAULT_SELECTION_RECT;
        spriteDescriptor.mCenter.mX = 0;
        spriteDescriptor.mCenter.mY = 0;
        spriteDescriptor.mPosition.mX = -75;

        spTargetSprite = new GLSprite(NULL, NULL, -1, spriteDescriptor, false, spTargetPixmap);
    }

    // Center sprite
    if (spCenterPointPixmap == NULL)
    {
        spCenterPointPixmap = new QPixmap(":/resource/center_point.png");
        SpriteDescriptor spriteDescriptor = SpriteDescriptor::makeDefaultSpriteDescriptor();
        spriteDescriptor.mTextureSrcRect.setWidth(8);
        spriteDescriptor.mTextureSrcRect.setHeight(8);
        spriteDescriptor.mCenter.mX = 0;
        spriteDescriptor.mCenter.mY = 0;

        spCenterPointSprite = new GLSprite(NULL, NULL, -1, spriteDescriptor, false, spCenterPointPixmap);
    }

    for (int i = 0; i < MIN_LINE_COUNT; i++) {
        QList<KeyFrame*> q;
        mTimeline.push_back(q);
    }
}

AnimationModel::~AnimationModel()
{
}

void AnimationModel::setTargetSpritePosition(float x, float y)
{
    if (spTargetSprite->mSpriteDescriptor.mPosition.mX != x || spTargetSprite->mSpriteDescriptor.mPosition.mY != y)
    {
        spTargetSprite->mSpriteDescriptor.mPosition.mX = x;
        spTargetSprite->mSpriteDescriptor.mPosition.mY = y;

        emit targetPositionMoved((int)x, (int)y);
    }
}

void AnimationModel::setAnimationType(int type)
{
    mAnimationType = (AnimationType)type;
    emit animationTypeChanged((int)mAnimationType);
}

void AnimationModel::setAnimationName(QString name)
{
    if (mAnimationName != name)
    {
        mAnimationName = name;
        emit animationNameChanged(name);
    }
}

void AnimationModel::setAnimationID(QString id)
{
    if (mAnimationID != id)
    {
        mAnimationID = id;
        emit animationIDChanged(id);
    }
}

int AnimationModel::getLastEventFrameNo()
{
    int lastFrameNo = -1;
    for(QHash<int, AnimationModel::EventList>::iterator iter = mEvents.begin() ; mEvents.end() != iter ; iter++)
    {
        if (iter.key() > lastFrameNo)
        {
            lastFrameNo = iter.key();
        }
    }

    return lastFrameNo;
}

int AnimationModel::getLineCount() const {
    return mTimeline.count();
}

int AnimationModel::getMaxFrameCount(int lineNo) const
{
    int max = 0;
    if (mTimeline[lineNo].count() > 0)
    {
        max = mTimeline[lineNo].last()->mFrameNo + 1;
    }

    return max;
}

int AnimationModel::getMaxFrameCount() const
{
    int max = 0;
    for (int i = 0; i < mTimeline.count(); i++)
    {
        int t = getMaxFrameCount(i);
        if (t > max)
        {
            max = t;
        }
    }

    return max;
}

bool AnimationModel::isDataChanged() const
{
    // TODO Implement
    return mIsDataChanged;
}

AnimationModel::EventList AnimationModel::getEventList(int frameNo) const
{
    if(mEvents.contains(frameNo))
    {
        return mEvents[frameNo];
    }
    else
    {
        return EventList();
    }
}

void AnimationModel::setEventList(int frameNo, AnimationModel::EventList eventList)
{
    mEvents[frameNo] = eventList;
    emit refreshTimeLine();
}

void AnimationModel::setEventText(int frameNo, int index, QString text)
{
    if(index >= 0 && mEvents.contains(frameNo))
    {
        mEvents[frameNo].mList[index] = text;
    }

}

KeyFrame* AnimationModel::getKeyFrame(int lineNo, int frameNo) const
{
    if (lineNo >= mTimeline.count() || lineNo < 0 || frameNo < 0){return NULL;}
    for (int i = 0; i < mTimeline[lineNo].count(); i++)
    {
        if (mTimeline[lineNo][i]->mFrameNo == frameNo)
        {
            return mTimeline[lineNo][i];
        }
    }

    return NULL;
}

int AnimationModel::getKeyFrameIndex(int lineNo, int frameNo) const
{
    if (lineNo < 0 || frameNo < 0) {return -2;}
    for (int i = 0; i < mTimeline[lineNo].count(); i++)
    {
        if (mTimeline[lineNo][i]->mFrameNo == frameNo)
        {
            return i;
        }
    }
    return -1;
}

bool AnimationModel::isKeyData(KeyFrameData::TweenAttribute tweenAttribute, const KeyFrame* pKeyframe) const
{
    // empty keyframe
    if(pKeyframe->getKeyFrameType() == KeyFrame::KeyFrameType_empty) {return true;}

    // Whatever keyframe
    if (tweenAttribute == KeyFrameData::TweenAttribute_any) {return true;}

    // keyframe with specified tween attribute
    if( pKeyframe->mpKeyFrameData->mTweenTypes[tweenAttribute] != KeyFrameData::eTT_None ) { return true; }

    return false;
}

int AnimationModel::getPreviousKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const
{
    const QList<KeyFrame*>& keyframeList = mTimeline[lineNo];
    for (int i = keyframeList.count() - 1; i >= 0; i--)
    {
        const KeyFrame* pKeyframe = keyframeList[i];
        if (frameNo >= pKeyframe->mFrameNo)
        {
            if (isKeyData(tweenAttribute, pKeyframe))
            {
                return i;
            }
        }
    }

    return -1;
}

int AnimationModel::getNextKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const
{
    const QList<KeyFrame*>& keyframeList = mTimeline[lineNo];
    for (int i = 0; i < keyframeList.count(); i++)
    {
        const KeyFrame* pKeyframe = keyframeList[i];
        if (pKeyframe->mFrameNo >= frameNo)
        {
            if (isKeyData(tweenAttribute, pKeyframe))
            {
                return i;
            }
        }
    }

    return -1;
}


int AnimationModel::getSubanimationStartKeyFrameIndex(int lineNo, int frameNo) const
{
    int prevIndex = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);
    if (prevIndex < 0){return -1;}
    if (!mTimeline[lineNo][prevIndex]->mpKeyFrameData){return -1;}
    QString sourthPath = mTimeline[lineNo][prevIndex]->mpKeyFrameData->mSpriteDescriptor.mSourcePath;

    for (int i = prevIndex - 1; i >= 0; i--)
    {
        if(!mTimeline[lineNo][i]->mpKeyFrameData || sourthPath != mTimeline[lineNo][i]->mpKeyFrameData->mSpriteDescriptor.mSourcePath)
        {
            return i + 1;
        }
    }

    return 0;
}

void AnimationModel::addEvent(int frameNo)
{
    if(!mEvents.contains(frameNo))
    {
        mEvents.insert(frameNo, EventList());
    }
    mEvents[frameNo].mList.push_back(QString(""));
    mIsDataChanged = true;
}

void AnimationModel::removeEvent(int frameNo, int index)
{
    if(mEvents.contains(frameNo))
    {
        mEvents[frameNo].mList.removeAt(index);
        if(mEvents[frameNo].mList.count() == 0)
        {
            mEvents.remove(frameNo);
        }
        mIsDataChanged = true;
    }
}

// set new key frame
void AnimationModel::setKeyFrame(int lineNo, int frameNo, const Point2& position)
{
    if (lineNo >= mTimeline.count()){return;}
    // if a keframe already exists, don't add any keyframe
    if (getKeyFrameIndex(lineNo, frameNo) == -1)
    {
        int index = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);

        KeyFrameData* pKeyframeData = new KeyFrameData();
        pKeyframeData->mSpriteDescriptor.mPosition = position;
        pKeyframeData->mSpriteDescriptor.mTextureSrcRect = mSelectedPaletTextureSrcRect;
        pKeyframeData->mSpriteDescriptor.mSourcePath = mSelectedSourcePath;

        KeyFrame* pKeyFrame = new KeyFrame(lineNo, frameNo, pKeyframeData);
        mTimeline[lineNo].insert(index + 1, pKeyFrame);

        emit refreshTimeLine();
        mIsDataChanged = true;
    }
}

void AnimationModel::setKeyFrame(int lineNo, int frameNo, KeyFrameData* pKeyframeData)
{
    if (lineNo >= mTimeline.count()){return;}
    // if a keframe already exists, don't add any keyframe
    if (getKeyFrameIndex(lineNo, frameNo) == -1)
    {
        int index = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);
        KeyFrame* pKeyFrame = new KeyFrame(lineNo, frameNo, pKeyframeData);
        mTimeline[lineNo].insert(index + 1, pKeyFrame);

        emit refreshTimeLine();
        mIsDataChanged = true;
    }
}

void AnimationModel::insertEmptyKeyFrame(int lineNo, int frameNo)
{
    if (lineNo >= mTimeline.count()){return;}
    // if a keframe already exists, don't add any keyframe
    if (getKeyFrameIndex(lineNo, frameNo) == -1)
    {
        int index = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);

        KeyFrame* pKeyframe = new KeyFrame(lineNo, frameNo, NULL);
        mTimeline[lineNo].insert(index + 1, pKeyframe);

        emit refreshTimeLine();
        mIsDataChanged = true;
    }
}

void AnimationModel::addFrameLength(int lineNo, int frameNo, int value)
{
    if (lineNo > mTimeline.count()) {return;}
    if (lineNo == mTimeline.count())
    {
        // Event frames control
        int lastFrameNo = getLastEventFrameNo();
        if (lastFrameNo > 0)
        {
            for (int i = lastFrameNo + 1; i > frameNo; i--)
            {
                if (mEvents.contains(i - 1) && !mEvents.contains(i))
                {
                    mEvents.insert(i, mEvents[i - 1]);
                    mEvents.remove(i - 1);
                }
            }
        }

        emit refreshTimeLine();
        mIsDataChanged = true;
    }
    else
    {
        if (value >= 0)
        {
            int index = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);
            for (int i = index + 1; i < mTimeline[lineNo].count(); i++)
            {
                mTimeline[lineNo][i]->mFrameNo += value;
            }

            emit refreshTimeLine();
            mIsDataChanged = true;
        }
    }
}

void AnimationModel::reduceFrameLength(int lineNo, int frameNo)
{
    if (lineNo > mTimeline.count()) {return;}

    if (lineNo == mTimeline.count())
    {
        int lastFrameNo = getLastEventFrameNo();
        if (lastFrameNo > 0)
        {
            for (int i = frameNo; i < lastFrameNo; i++)
            {
                if (!mEvents.contains(i) && mEvents.contains(i + 1))
                {
                    mEvents.insert(i, mEvents[i + 1]);
                    mEvents.remove(i + 1);
                }
            }
        }

        emit refreshTimeLine();
        mIsDataChanged = true;
    }
    else
    {
        int endKeyFrameIndex = getNextKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);

        if (endKeyFrameIndex < 0) {
            return;
        }

        // If it cannot reduce frame length more, return it
        if (
                endKeyFrameIndex > 0 &&
                mTimeline[lineNo][endKeyFrameIndex]->mFrameNo - mTimeline[lineNo][endKeyFrameIndex - 1]->mFrameNo <= 1
        )
        {
            return;
        }

        // If keyframe exists on this timeline
        if (endKeyFrameIndex >= 0)
        {
            // Move position of keyframes effected
            for (int i = endKeyFrameIndex; i < mTimeline[lineNo].count(); i++)
            {
                mTimeline[lineNo][i]->mFrameNo -= 1;
            }

            emit refreshTimeLine();
            mIsDataChanged = true;
        }
    }
}

void AnimationModel::clearFrames(int lineNo, int startFrameNo, int endFrameNo)
{
    // Remove frames
    for (int i = endFrameNo; i >= startFrameNo; i--)
    {
        if (lineNo == mTimeline.count())
        {
            mEvents.remove(i);
        }
        else
        {
            int keyframeIndex = getKeyFrameIndex(lineNo, i);
            if (keyframeIndex >= 0)
            {
                delete mTimeline[lineNo][keyframeIndex];
                mTimeline[lineNo].removeAt(keyframeIndex);
            }
        }
    }

    emit refreshTimeLine();
    mIsDataChanged = true;
}

void AnimationModel::clearAllKeyFrames()
{
    for (int i = 0;  i < mTimeline.count(); i++)
    {
        for (int j = 0;  j < mTimeline[i].count(); j++)
        {
            delete mTimeline[i][j];
        }
        mTimeline[i].clear();
    }

    for (int i = mTimeline.count(); i >= MIN_LINE_COUNT; i--) {
        mTimeline.removeAt(i);
    }

    emit refreshTimeLine();
}

QSize AnimationModel::guideSize() const
{
    return animationTypeSize[mAnimationType];
}

const QList<KeyFrame*>& AnimationModel::getKeyFrameList(int lineNo) const
{
    return mTimeline[lineNo];
}


QString AnimationModel::getSelectedSourcePath() const
{
    return mSelectedSourcePath;
}

void AnimationModel::setSelectedSourcePath(QString path)
{
    if (ResourceManager::getFileType(path) == ResourceManager::FileType_Invalid) { path = ""; }

    if (mSelectedSourcePath != path)
    {
        mSelectedSourcePath = path;
        mSelectedPaletTextureSrcRect = DEFAULT_SELECTION_RECT;
        emit selectedPaletChanged(path);
    }
}

// Tween Related Stuff
GLSprite* AnimationModel::createGLSpriteAt(const GLSprite* parentGLSprite, int frameNo, int lineNo) const
{
    // no keyframes in this line
    if (mTimeline[lineNo].count() == 0) {return NULL;}

    // exceeds max frame count
    if(frameNo >= getMaxFrameCount(lineNo)){return NULL;}

    // Set up base for keyframe.(inherit textureID etc from previous keyframe
    int baseIndex = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);
    if (baseIndex < 0){return NULL;} // no keyframe exists

    KeyFrame* pBaseKeyFrame = mTimeline[lineNo][baseIndex];
    KeyFrameData* pBaseKeyFrameData = pBaseKeyFrame->mpKeyFrameData;
    if (!pBaseKeyFrameData) {return NULL;} // empty keyframe

    SpriteDescriptor baseSpriteDescriptor = pBaseKeyFrameData->mSpriteDescriptor;

    setFinalAlpha(parentGLSprite, baseSpriteDescriptor);
    setFinalPosition(parentGLSprite, baseSpriteDescriptor);

    // Tween for each attribute
    for (int i = 0; i < KeyFrameData::TweenAttribute_COUNT; i++)
    {
        copyTweenedAttribute(parentGLSprite, baseSpriteDescriptor, lineNo, frameNo, (KeyFrameData::TweenAttribute)i);
    }

    // start of subanimation
    int subAnimationStartIndex = getSubanimationStartKeyFrameIndex(lineNo, frameNo);

    // frame no for subanimation in the frame
    if (subAnimationStartIndex < 0)
    {
        baseSpriteDescriptor.mFrameNo = 0;
    }
    else
    {
        baseSpriteDescriptor.mFrameNo = frameNo - mTimeline[lineNo][subAnimationStartIndex]->mFrameNo;
    }
    baseSpriteDescriptor.mPriority += lineNo * 0.001f;

    bool isTweenCel  = (pBaseKeyFrame->mFrameNo == frameNo);
    return new GLSprite(parentGLSprite, this, lineNo, baseSpriteDescriptor, isTweenCel, lineNo, frameNo, false, 0, 0);
}

// currently parentGLSprite is used only for emitted animation
// currently this method is only used for facetodir option, which needs future frame to compare
GLSprite* AnimationModel::createGLSpriteAt(const GLSprite* parentGLSprite, QList<KeyFrame::KeyFramePosition>nodePath) const
{
    GLSprite* pGLSprite = NULL;
    const AnimationModel* pAnimationModel = this;
    for (int i = 0; i < nodePath.count(); i++)
    {
        pGLSprite = pAnimationModel->createGLSpriteAt(pGLSprite, nodePath[i].mFrameNo, nodePath[i].mLineNo);
        if (!pGLSprite) {return NULL;}

        // If we have a parent, apply transformation to its children
        if (parentGLSprite)
        {
            // Note: Might be wrong!
            // We just assign the speed, we really don't care subanimation's state..
            // I don't care too much about this, since this only effects faceToDir option..
            if (parentGLSprite->mSpeedX != 0  || parentGLSprite->mSpeedY != 0 )
            {
                pGLSprite->mSpriteDescriptor.mPosition.mX = parentGLSprite->mSpeedX;
                pGLSprite->mSpriteDescriptor.mPosition.mY = parentGLSprite->mSpeedY;
            }

            pGLSprite->mSpriteDescriptor.mOptionalTransform = parentGLSprite->mSpriteDescriptor.getTransform();
            pGLSprite->mSpriteDescriptor.mPriority = parentGLSprite->getRootSprite()->mSpriteDescriptor.mPriority;
            pGLSprite->mSpriteDescriptor.mAlpha *= parentGLSprite->getAbsoluteAlpha();

            parentGLSprite = NULL;
        }

        if (ResourceManager::getFileType(pGLSprite->mSpriteDescriptor.mSourcePath) == ResourceManager::FileType_Animation)
        {
            pAnimationModel = ResourceManager::getAnimation(pGLSprite->mSpriteDescriptor.mSourcePath);
        }
    }

    return pGLSprite;
}

const QList<const GLSprite*> AnimationModel::createGLSpriteListAt(const GLSprite* parentGLSprite, int frameNo) const
{
    QList<const GLSprite*> glSpriteList;
    const GLSprite* pGLSprite = parentGLSprite;
    for (int lineNo = 0; lineNo < mTimeline.count(); lineNo++)
    {
        pGLSprite = createGLSpriteAt(parentGLSprite, frameNo, lineNo);
        if (pGLSprite)
        {
            glSpriteList.push_back(pGLSprite);
        }
    }

    return glSpriteList;
}

template <class T> T AnimationModel::tweenValue(KeyFrameData::TweenType tweenType, T startValue, T endValue, int frameNo, int startFrameNo, int endFrameNo) const
{
    if (endFrameNo <= startFrameNo) {
        return startValue;
    }
    switch (tweenType){
    case KeyFrameData::eTT_EaseIn:
        return EASE_IN(startValue, endValue, frameNo, startFrameNo, endFrameNo);
    break;
    case KeyFrameData::eTT_EaseOut:
        return EASE_OUT(startValue, endValue, frameNo, startFrameNo, endFrameNo);
    break;
    case KeyFrameData::eTT_Linear:
        return LERP(startValue, endValue, frameNo, startFrameNo, endFrameNo);
    break;
    case KeyFrameData::eTT_Fix:
        return LERP(startValue, startValue, frameNo, startFrameNo, endFrameNo);
    break;
    default:
        return startValue;
    }
}

void AnimationModel::tweenElement(SpriteDescriptor& spriteDescriptor, KeyFrameData::TweenAttribute tweenAttribute, KeyFrameData::TweenType tweenType, SpriteDescriptor& startDescriptor, SpriteDescriptor& endDescriptor, int frameNo, int startFrameNo, int endFrameNo) const
{
    switch(tweenAttribute)
    {
        case KeyFrameData::TweenAttribute_alpha:
            spriteDescriptor.mAlpha = tweenValue<float>(tweenType, startDescriptor.mAlpha, endDescriptor.mAlpha, frameNo, startFrameNo, endFrameNo);
            break;
        case KeyFrameData::TweenAttribute_position:
            spriteDescriptor.mPosition.mX = tweenValue<int>(tweenType, startDescriptor.mPosition.mX, endDescriptor.mPosition.mX, frameNo, startFrameNo, endFrameNo);
            spriteDescriptor.mPosition.mY = tweenValue<int>(tweenType, startDescriptor.mPosition.mY, endDescriptor.mPosition.mY, frameNo, startFrameNo, endFrameNo);
            break;
        case KeyFrameData::TweenAttribute_rotation:
            spriteDescriptor.mRotation = tweenValue<int>(tweenType, startDescriptor.mRotation, endDescriptor.mRotation, frameNo, startFrameNo, endFrameNo);
            break;
        case KeyFrameData::TweenAttribute_scale:
            spriteDescriptor.mScale.mX = tweenValue<float>(tweenType, startDescriptor.mScale.mX, endDescriptor.mScale.mX, frameNo, startFrameNo, endFrameNo);
            spriteDescriptor.mScale.mY = tweenValue<float>(tweenType, startDescriptor.mScale.mY, endDescriptor.mScale.mY, frameNo, startFrameNo, endFrameNo);
            break;
        case KeyFrameData::TweenAttribute_color:
            spriteDescriptor.mColor.mR = tweenValue<float>(tweenType, startDescriptor.mColor.mR, endDescriptor.mColor.mR, frameNo, startFrameNo, endFrameNo);
            spriteDescriptor.mColor.mG = tweenValue<float>(tweenType, startDescriptor.mColor.mG, endDescriptor.mColor.mG, frameNo, startFrameNo, endFrameNo);
            spriteDescriptor.mColor.mB = tweenValue<float>(tweenType, startDescriptor.mColor.mB, endDescriptor.mColor.mB, frameNo, startFrameNo, endFrameNo);
            break;
        default:
            break;
    }
}

// Update the sprite so that it reflects some options
void AnimationModel::setFinalAlpha(const GLSprite* parentGLSprite, SpriteDescriptor& spriteDescriptor) const
{
    float dAlpha = 1.0;
    if (parentGLSprite)
    {
        dAlpha = parentGLSprite->mSpriteDescriptor.mAlpha;
    }
    spriteDescriptor.mAlpha *= dAlpha;
}



void AnimationModel::setFinalPosition(const GLSprite* parentGLSprite, SpriteDescriptor& spriteDescriptor) const
{
    if(spriteDescriptor.mPositionType != PositionType_None)
    {
        QPoint point = spriteDescriptor.getPosition(spTargetSprite);
        if(spriteDescriptor.mPositionType == PositionType_RelativeToTarget)
        {
            // Transform current position based on target point
            if(parentGLSprite)
            {
                point = point * spTargetSprite->getTotalTransform() * parentGLSprite->getTotalTransform().inverted();
            }
            else
            {
                point = point * spTargetSprite->getTransform();
            }

        }
        else if(spriteDescriptor.mPositionType == PositionType_RelativeToTargetOrigin)
        {
            QPoint targetPoint = getPositionWithPositionType(QPoint(TARGET_originX, TARGET_originY), spriteDescriptor.mPositionTypeOption, spTargetSprite->mSpriteDescriptor.mTextureSrcRect.width(), spTargetSprite->mSpriteDescriptor.mTextureSrcRect.height(),spriteDescriptor.mTextureSrcRect.height());
            QTransform transform;
            transform.translate(targetPoint.x(), targetPoint.y());
            transform.rotate(0);
            transform.scale(0, 0);

            // Transform current position based on target point
            if(parentGLSprite)
            {
                point = point * transform * parentGLSprite->getTotalTransform().inverted();
            }
            else
            {
                point = point * transform;
            }
        }

        spriteDescriptor.mPosition.mX = point.x();
        spriteDescriptor.mPosition.mY = point.y();
    }
}

void AnimationModel::setFinalRotation(const GLSprite* parentGLSprite, int lineNo, int frameNo, SpriteDescriptor& spriteDescriptor) const
{
    if (sIsNesting){return;} // if it is nesting, don't do this part
    sIsNesting = true;
    switch(spriteDescriptor.mFacingOptionType)
    {
        // Look at target sprite
        case FacingOptionType_lookAtTarget:
        {
            QPoint point = QPointF(spriteDescriptor.mPosition.mX, spriteDescriptor.mPosition.mY).toPoint();

            // Transform current position to screen coordinate
            if (parentGLSprite)
            {
                point = point * (parentGLSprite->getTotalTransform());
            }
            else
            {
                point = point * (spTargetSprite->getTotalTransform() * spTargetSprite->getTransform().inverted());
            }

            // Transform target position to screen coordinate
            QPointF targetPoint = QPointF(spTargetSprite->mSpriteDescriptor.mPosition.mX, spTargetSprite->mSpriteDescriptor.mPosition.mY);
            targetPoint = targetPoint * spTargetSprite->getTotalTransform() * spTargetSprite->getTransform().inverted();

            // Compare target and current sprite in screen coordinate
            float dx = targetPoint.x() - point.x();
            float dy = targetPoint.y() - point.y();

            if (dx == 0 && dy == 0)
            {
                SpriteDescriptor tempSpriteDescriptor = spriteDescriptor;
                // In order to face to correct direction, it needs to calculate the position first
                copyTweenedAttribute(parentGLSprite, tempSpriteDescriptor, lineNo, frameNo - 1, KeyFrameData::TweenAttribute_position);
                copyTweenedAttribute(parentGLSprite, tempSpriteDescriptor, lineNo, frameNo - 1, KeyFrameData::TweenAttribute_rotation);
                spriteDescriptor.mRotation = tempSpriteDescriptor.mRotation;
                int angleOffset = (int)floor((180 * atan2(dy, dx)) / PI);
                spriteDescriptor.mRotation += angleOffset;
            }
            else
            {
                int angleOffset = (int)floor((180 * atan2(dy, dx)) / PI);
                spriteDescriptor.mRotation += angleOffset;
            }
       }
       break;

       // Face to the direction where it is going to move
       case FacingOptionType_FaceToMovingDir:
           {
                const AnimationModel* pAnimationModel = this;
                QList<KeyFrame::KeyFramePosition> list;
                const GLSprite* pRootSprite = NULL;
                if (parentGLSprite)
                {
                    list = parentGLSprite->getNodePath();
                    pRootSprite = parentGLSprite->getRootSprite();
                    pAnimationModel = pRootSprite->getParentAnimationModel();
                }
                list.push_back(KeyFrame::KeyFramePosition(lineNo, frameNo));

                // Get next frame in world coordinate
                for (int i = 0; i < list.count(); i++)
                {
                    list[i].mFrameNo += 1;
                }
                const GLSprite* pTargetSprite = NULL;
                if (pRootSprite && pRootSprite->isEmitted())
                {
                    pTargetSprite = pAnimationModel->createGLSpriteAt(pRootSprite, list);
                }
                else
                {
                    pTargetSprite = pAnimationModel->createGLSpriteAt(NULL, list);
                }

                // If there is no next frame, use previous frame as target
                bool reverse = false;
                if (!pTargetSprite)
                {
                    // Get previous frame in world coordinate
                    for (int i = 0; i < list.count(); i++)
                    {
                        list[i].mFrameNo -= 2;
                    }
                    pTargetSprite = pAnimationModel->createGLSpriteAt(NULL, list);
                    reverse = true;
                }

                if (pTargetSprite)
                {
                    QPointF point = QPointF(spriteDescriptor.mPosition.mX, spriteDescriptor.mPosition.mY);

                    // Transform current position to screen coordinate
                    if (parentGLSprite)
                    {
                        point = point * (parentGLSprite->getTotalTransform());
                    }
                    else
                    {
                        // The use of spTargetSprite is very tricky, it is nothing to do with target sprite position
                        point = point * (spTargetSprite->getTotalTransform() * spTargetSprite->getTransform().inverted());
                    }
                    // Transform target position to screen coordinate
                    QPointF targetPoint = QPointF(pTargetSprite->mSpriteDescriptor.mPosition.mX, pTargetSprite->mSpriteDescriptor.mPosition.mY);
                    if (pTargetSprite->getParentSprite())
                    {
                        targetPoint = targetPoint * pTargetSprite->getParentSprite()->getTotalTransform();
                    }
                    else
                    {
                        targetPoint = targetPoint * (spTargetSprite->getTotalTransform() * spTargetSprite->getTransform().inverted());
                    }
                    // Compare target and current sprite in screen coordinate
                    float dx = targetPoint.x() - point.x();
                    float dy = targetPoint.y() - point.y();

                    if (reverse)
                    {
                        dx = point.x() - targetPoint.x();
                        dy = point.y() - targetPoint.y();
                    }

                    int angleOffset = (int)floor((180 * atan2(dy, dx)) / PI);
                    spriteDescriptor.mRotation += angleOffset;
                }

                // delete all temporaly generated Sprites.
                const GLSprite* pSprite = pTargetSprite;
                while (pSprite)
                {
                    const GLSprite* tSprite = pSprite->getParentSprite();
                    delete pSprite;
                    pSprite = tSprite;
                }
           }
           break;
       default:

       break;
    }
    sIsNesting = false;
}

void AnimationModel::copyTweenedAttribute(const GLSprite* pParentGLSprite, SpriteDescriptor& spriteDescriptor, int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const
{
    bool tweenFound = false;
    int startIndex = getPreviousKeyFrameIndex(lineNo, frameNo, tweenAttribute);
    int endIndex = getNextKeyFrameIndex(lineNo, frameNo, tweenAttribute);
    if (endIndex < 0) {
        endIndex = startIndex;
    }
    if (startIndex >= 0 && endIndex >= startIndex)
    {
        tweenFound = true;
        KeyFrame* pStartKeyFrame = mTimeline[lineNo][startIndex];
        KeyFrame* pEndKeyFrame = mTimeline[lineNo][endIndex];

        // Let's skip empty frames
        if (!pStartKeyFrame->mpKeyFrameData || !pStartKeyFrame->mpKeyFrameData)
        {
            return;
        }

        SpriteDescriptor startDescriptor = pStartKeyFrame->mpKeyFrameData->mSpriteDescriptor;

        setFinalAlpha(pParentGLSprite, startDescriptor);
        setFinalPosition(pParentGLSprite, startDescriptor);

        SpriteDescriptor endDescriptor;
        if (pEndKeyFrame->mpKeyFrameData)
        {
            endDescriptor = pEndKeyFrame->mpKeyFrameData->mSpriteDescriptor;
            setFinalAlpha(pParentGLSprite, endDescriptor);
            setFinalPosition(pParentGLSprite, endDescriptor);
        }
        else
        {
            endDescriptor = startDescriptor;
        }

        tweenElement(spriteDescriptor, tweenAttribute, pStartKeyFrame->mpKeyFrameData->mTweenTypes[tweenAttribute], startDescriptor, endDescriptor, frameNo, pStartKeyFrame->mFrameNo, pEndKeyFrame->mFrameNo);

        if(tweenAttribute == KeyFrameData::TweenAttribute_rotation)
        {
            setFinalRotation(pParentGLSprite, lineNo, frameNo, spriteDescriptor);
        }
    }
    else if (endIndex == startIndex)
    {
        tweenFound = true;
        if (tweenAttribute == KeyFrameData::TweenAttribute_rotation)
        {
            setFinalRotation(pParentGLSprite, lineNo, frameNo, spriteDescriptor);
        }
    }
}

KeyFrame::KeyFramePosition AnimationModel::getCurrentKeyFramePosition()
{
    return mSelectedKeyFramePosition;
}

void AnimationModel::selectCurrentKeyFramePosition(int lineNo, int frameNo)
{
    KeyFrame::KeyFramePosition keyframePosition;
    keyframePosition.mLineNo = lineNo;
    keyframePosition.mFrameNo =  frameNo;

    if (mSelectedKeyFramePosition != keyframePosition)
    {
        mSelectedKeyFramePosition = keyframePosition;

        emit selectedKeyFramePositionChanged(mSelectedKeyFramePosition.mLineNo, mSelectedKeyFramePosition.mFrameNo);
    }
}

bool AnimationModel::saveData()
{
    // Get path with directory info
    QString rootPath = ResourceManager::GetDirectoryPath(ResourceManager::FileType_Animation);

    // we don't want to store absolute path
    QString saveDirName = rootPath.append(mAnimationDirectory).append("/");
    QDir saveDir = QDir(saveDirName);
    QString savePath = saveDirName.append(mAnimationID).append(".").append(ANIMATION_FORMAT);
    QString originalPath = rootPath.append(mAnimationDirectory).append("/").append(mOriginalAnimationID).append(".").append(ANIMATION_FORMAT);
    QFileInfo info = QFileInfo(savePath);

    QString error = QString("File %0.ani already exists.").arg(mAnimationID);
    // unless you are overriding loaded animation ID, we won't allow to override existing files
    if (mOriginalAnimationID != mAnimationID && info.exists())
    {
        QMessageBox::information(mpParent->window(), tr("File saving error"),
                             tr(error.toStdString().c_str()));
        return false;
    }

    Json::Value root;

    // save animation name
    root["name"] = mAnimationName.toStdString();
    root["type"] = (int)mAnimationType;

    // save keyframes
    Json::Value keyframesData;
    for (int i = 0; i < mTimeline.count(); i++)
    {
        //if no data exists in this line, ignore it.
        if(mTimeline[i].count() == 0)
        {
            continue;
        }

        for (int j = 0; j < mTimeline[i].count(); j++)
        {
            const KeyFrame* keyframe = mTimeline[i][j];

            Json::Value keyframeData;
            keyframeData["frameNo"] = keyframe->mFrameNo;

            if (keyframe->mpKeyFrameData)
            {
                KeyFrameData* pKeyFrameData = keyframe->mpKeyFrameData;
                keyframeData["sourcePath"] = pKeyFrameData->mSpriteDescriptor.mSourcePath.toStdString();

                if (pKeyFrameData->mSpriteDescriptor.mFacingOptionType != FacingOptionType_none)
                {
                    keyframeData["facingOption"] = GLSprite::facingOptionTypeSting[pKeyFrameData->mSpriteDescriptor.mFacingOptionType].toStdString();
                }

                if (pKeyFrameData->mSpriteDescriptor.mPositionTypeOption != PositionTypeOption_Center)
                {
                    keyframeData["positionTypeOption"] = KeyFrameData::positionTypeOptionString[pKeyFrameData->mSpriteDescriptor.mPositionTypeOption].toStdString();
                }

                if (pKeyFrameData->mSpriteDescriptor.mPositionType != PositionType_None)
                {
                    keyframeData["positionType"] = KeyFrameData::positionTypeString[pKeyFrameData->mSpriteDescriptor.mPositionType].toStdString();
                }

                if (pKeyFrameData->mSpriteDescriptor.mEmitter)
                {
                    keyframeData["emitter"] = pKeyFrameData->mSpriteDescriptor.mEmitter;
                    keyframeData["minEmitSpeed"] = pKeyFrameData->mSpriteDescriptor.mMinEmitSpeed;
                    keyframeData["maxEmitSpeed"] = pKeyFrameData->mSpriteDescriptor.mMaxEmitSpeed;
                    keyframeData["minEmitAngle"] = pKeyFrameData->mSpriteDescriptor.mMinEmitAngle;
                    keyframeData["maxEmitAngle"] = pKeyFrameData->mSpriteDescriptor.mMaxEmitAngle;
                }

                if (ResourceManager::getFileType(pKeyFrameData->mSpriteDescriptor.mSourcePath) == ResourceManager::FileType_Image)
                {
                    QImage* pImage = ResourceManager::getImage(pKeyFrameData->mSpriteDescriptor.mSourcePath);

                    if (pImage->rect() != pKeyFrameData->mSpriteDescriptor.mTextureSrcRect)
                    {
                        Json::Value textureRect;
                        textureRect[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.x();
                        textureRect[1] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.y();
                        textureRect[2] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.width();
                        textureRect[3] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.height();
                        keyframeData["textureRect"] = textureRect;
                    }
                }

                // Swap Target Type
                if (pKeyFrameData->mSwapTargetType != KeyFrameData::SwapTargetType_None)
                {
                    keyframeData["swapTargetType"] = KeyFrameData::swapTargetTypeString[pKeyFrameData->mSwapTargetType].toStdString();
                }

                if (pKeyFrameData->mHideActor)
                {
                    keyframeData["hideActor"] = pKeyFrameData->mHideActor;
                }

                if (pKeyFrameData->mSpriteDescriptor.mBlendType != eBT_Alpha)
                {
                    keyframeData["blendType"] = GLSprite::blendTypeSting[pKeyFrameData->mSpriteDescriptor.mBlendType].toStdString();
                }

                if (pKeyFrameData->mSpriteDescriptor.mCenter != QPoint(0, 0))
                {
                    Json::Value centerPoint;
                    centerPoint[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mCenter.mX;
                    centerPoint[1] = pKeyFrameData->mSpriteDescriptor.mCenter.mY;
                    keyframeData["center"] = centerPoint;
                }

                 if (pKeyFrameData->mSpriteDescriptor.mPosition != QPoint(0, 0))
                 {
                    Json::Value position;
                    position[static_cast<unsigned int>(0)] = static_cast<int>(pKeyFrameData->mSpriteDescriptor.mPosition.mX);
                    position[1] = static_cast<int>(pKeyFrameData->mSpriteDescriptor.mPosition.mY);
                    keyframeData["position"] = position;
                 }

                 if (pKeyFrameData->mSpriteDescriptor.mScale != QPoint(1, 1))
                 {
                    Json::Value scale;
                    scale[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mScale.mX;
                    scale[1] = pKeyFrameData->mSpriteDescriptor.mScale.mY;
                    keyframeData["scale"] = scale;
                 }

                 if (pKeyFrameData->mSpriteDescriptor.mRotation != 0)
                 {
                    keyframeData["rotation"] = pKeyFrameData->mSpriteDescriptor.mRotation;
                 }

                 if (pKeyFrameData->mSpriteDescriptor.mColor != Color(0, 0, 0))
                 {
                    Json::Value color;
                    color[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mColor.mR;
                    color[1] = pKeyFrameData->mSpriteDescriptor.mColor.mG;
                    color[2] = pKeyFrameData->mSpriteDescriptor.mColor.mB;
                    keyframeData["color"] = color;
                 }

                 if (pKeyFrameData->mSpriteDescriptor.mPriority != 0.5)
                 {
                    keyframeData["priority"] = pKeyFrameData->mSpriteDescriptor.mPriority;
                 }

                 if (pKeyFrameData->mSpriteDescriptor.mAlpha < 1)
                 {
                    keyframeData["alpha"] = pKeyFrameData->mSpriteDescriptor.mAlpha;
                 }


                for (int k = 0; k < KeyFrameData::TweenAttribute_COUNT; k++)
                {
                    if (pKeyFrameData->mTweenTypes[k] != KeyFrameData::eTT_None)
                    {
                        keyframeData[KeyFrameData::tweenAttributeSting[k].toStdString()] = KeyFrameData::tweenTypeSting[pKeyFrameData->mTweenTypes[k]].toStdString();
                    }
                }
            }
            else
            {
                keyframeData["isEmpty"] = true;
            }

            keyframesData[QString::number(i).toStdString()][j] = keyframeData;
        }
    }

    if (!keyframesData.empty())
    {
        root["keyframes"] = keyframesData;
    }

    // Write event data
    if(mEvents.count() > 0)
    {
        Json::Value events;
        for(QHash<int, AnimationModel::EventList>::iterator iter = mEvents.begin() ; mEvents.end() != iter ; iter++)
        {
            int frameNo = iter.key();
            AnimationModel::EventList eventList = static_cast<AnimationModel::EventList>(*iter);
            Json::Value list;
            for (int i = 0; i < eventList.mList.count(); i++)
            {
                list[static_cast<unsigned int>(i)] = eventList.mList[i].toStdString();
            }

            events[QString::number(frameNo).toStdString()] = list;
        }
        root["events"] = events;
    }

    saveDir.remove(mOriginalAnimationID.append(".").append(ANIMATION_FORMAT));
    Json::StyledWriter writer;
    std::string outputJson = writer.write(root);

    QFile file(savePath);
    QString fileData;
    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &file );
        file.write(outputJson.c_str(), outputJson.length());
        while ( !stream.atEnd() ) {
            fileData = stream.readAll();
        }
        file.close();
    }

    mOriginalAnimationID = mAnimationID;
    emit fileChanged();
    mIsDataChanged = false;

    return true;
}


QList<KeyFrame*> AnimationModel::loadLine(int lineNo, Json::Value& line)
{
    QList<KeyFrame*> keyframes;
    for (unsigned int j = 0; j < line.size(); j++)
    {
        Json::Value& keyframe = line[j];

        int frameNo = keyframe["frameNo"].asInt();
        KeyFrame* pKeyFrame = NULL;
        if (!keyframe["isEmpty"].asBool())
        {
            KeyFrameData* pKeyFrameData = new KeyFrameData();

            // Tween valid data
            for (int k = 0; k < KeyFrameData::TweenAttribute_COUNT; k++)
            {
                if (keyframe[KeyFrameData::tweenAttributeSting[k].toStdString()].isString())
                {
                    pKeyFrameData->mTweenTypes[k] = KeyFrameData::getTweenTypeByString(QString::fromStdString(keyframe[KeyFrameData::tweenAttributeSting[k].toStdString()].asString()));
                }
            }

            pKeyFrameData->mSpriteDescriptor.mFacingOptionType = GLSprite::getFacingOptionTypeByString(QString::fromStdString(keyframe["facingOption"].asString()));

            if (keyframe["positionTypeOption"].isString())
            {
                pKeyFrameData->mSpriteDescriptor.mPositionTypeOption = KeyFrameData::getPositionTypeOptionByString(QString::fromStdString(keyframe["positionTypeOption"].asString()));
            }

            if (keyframe["positionType"].isString())
            {
                pKeyFrameData->mSpriteDescriptor.mPositionType = KeyFrameData::getPositionTypeByString(QString::fromStdString(keyframe["positionType"].asString()));
            }

            if (keyframe["emitter"].isBool())
            {
                pKeyFrameData->mSpriteDescriptor.mEmitter = keyframe["emitter"].asBool();
                if (keyframe["minEmitSpeed"].isDouble())
                {
                    pKeyFrameData->mSpriteDescriptor.mMinEmitSpeed = keyframe["minEmitSpeed"].asDouble();
                }
                if (keyframe["maxEmitSpeed"].isDouble())
                {
                    pKeyFrameData->mSpriteDescriptor.mMaxEmitSpeed = keyframe["maxEmitSpeed"].asDouble();
                }
                if (keyframe["minEmitAngle"].isInt())
                {
                    pKeyFrameData->mSpriteDescriptor.mMinEmitAngle = keyframe["minEmitAngle"].asInt();
                }
                if (keyframe["maxEmitAngle"].isInt())
                {
                    pKeyFrameData->mSpriteDescriptor.mMaxEmitAngle = keyframe["maxEmitAngle"].asInt();
                }
            }

            pKeyFrameData->mSpriteDescriptor.mSourcePath = QString::fromStdString(keyframe["sourcePath"].asString());

            // Swap Target Type
            if (keyframe["swapTargetType"].isString())
            {
                pKeyFrameData->mSwapTargetType = KeyFrameData::getSwapTargetTypeByString(QString::fromStdString(keyframe["swapTargetType"].asString()));
            }

            if (keyframe["hideActor"].isBool())
            {
                pKeyFrameData->mHideActor = keyframe["hideActor"].asBool();
            }

            // Blend
            if (keyframe["blendType"].isString())
            {
                pKeyFrameData->mSpriteDescriptor.mBlendType = GLSprite::getBlendTypeByString(QString::fromStdString(keyframe["blendType"].asString()));
            }

            // Alpha
            if (keyframe["alpha"].isNumeric())
            {
                pKeyFrameData->mSpriteDescriptor.mAlpha = keyframe["alpha"].asDouble();
            }

            // priority data
            if (!keyframe["priority"].isNull())
            {
                pKeyFrameData->mSpriteDescriptor.mPriority = keyframe["priority"].asDouble();
            }

            // position data
            if (!keyframe["position"].isNull())
            {
                pKeyFrameData->mSpriteDescriptor.mPosition.mX = keyframe["position"][static_cast<unsigned int>(0)].asInt();
                pKeyFrameData->mSpriteDescriptor.mPosition.mY = keyframe["position"][1].asInt();
            }

            if (!keyframe["rotation"].isNull())
            {
                pKeyFrameData->mSpriteDescriptor.mRotation = keyframe["rotation"].asInt();
            }

            if (!keyframe["scale"].isNull())
            {
                pKeyFrameData->mSpriteDescriptor.mScale.mX = keyframe["scale"][static_cast<unsigned int>(0)].asDouble();
                pKeyFrameData->mSpriteDescriptor.mScale.mY = keyframe["scale"][1].asDouble();
            }

            if (!keyframe["color"].isNull())
            {
                pKeyFrameData->mSpriteDescriptor.mColor.mR = keyframe["color"][static_cast<unsigned int>(0)].asDouble();
                pKeyFrameData->mSpriteDescriptor.mColor.mG = keyframe["color"][1].asDouble();
                pKeyFrameData->mSpriteDescriptor.mColor.mB = keyframe["color"][2].asDouble();
            }

            if (!keyframe["center"].isNull())
            {
                pKeyFrameData->mSpriteDescriptor.mCenter.mX = keyframe["center"][static_cast<unsigned int>(0)].asInt();
                pKeyFrameData->mSpriteDescriptor.mCenter.mY = keyframe["center"][1].asInt();
            }

            if (!keyframe["textureRect"].isNull())
            {
                pKeyFrameData->mSpriteDescriptor.mTextureSrcRect = QRect(
                        keyframe["textureRect"][static_cast<unsigned int>(0)].asInt(),
                        keyframe["textureRect"][1].asInt(),
                        keyframe["textureRect"][2].asInt(),
                        keyframe["textureRect"][3].asInt()
                        );
            }
            else
            {
                if (ResourceManager::getFileType(pKeyFrameData->mSpriteDescriptor.mSourcePath) == ResourceManager::FileType_Image)
                {
                    QImage* pImage = ResourceManager::getImage(pKeyFrameData->mSpriteDescriptor.mSourcePath);
                    pKeyFrameData->mSpriteDescriptor.mTextureSrcRect = pImage->rect();
                }
                else
                {
                    pKeyFrameData->mSpriteDescriptor.mTextureSrcRect = DEFAULT_SELECTION_RECT;
                }
            }

            pKeyFrame = new KeyFrame(lineNo, frameNo, pKeyFrameData);
        }
        else
        {
            pKeyFrame = new KeyFrame(lineNo, frameNo, NULL);
        }
        keyframes.push_back(pKeyFrame);
    }

    return keyframes;
}
// Load animation file, return true if loading was succeeded
bool AnimationModel::loadData(QString path)
{
    mIsDataChanged = false;
    clearAllKeyFrames();
    mEvents.clear();

    Json::Value root;
    Json::Reader reader;

    QFile file(path);
    QString fileData;
    if ( file.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &file );
        while ( !stream.atEnd() ) {
            fileData = stream.readAll();
        }
        file.close();
    }
    std::string inputJson = fileData.toStdString();

    //std::string json_doc = readInputFile(path);
    if(!reader.parse(inputJson, root))
    {
        QString error = QString("File %0 is not correct JSON format").arg(path);
        printf("%s", error.toStdString().c_str());
        return false;
    }

    // Get path without directory info & extension
    QString rootPath = ResourceManager::GetDirectoryPath(ResourceManager::FileType_Animation);

    QFileInfo fileInfo = QFileInfo(path);

    mAnimationDirectory = fileInfo.absolutePath().replace(rootPath, "");
    mOriginalAnimationID = fileInfo.baseName();
    setAnimationID(mOriginalAnimationID);

    // load animation name
    setAnimationName(QString::fromStdString(root["name"].asString()).toUtf8());
    setAnimationType(AnimationType(root["type"].asInt()));
    Json::Value& lines = root["keyframes"];
    for(Json::Value::iterator iter = lines.begin() ; lines.end() != iter ; iter++)
    {
        const char* c_str = iter.name().c_str();
        int lineNo = atoi(c_str);
        Json::Value& line =  *iter;
        mTimeline[lineNo] = loadLine(lineNo, line);
    }


    Json::Value& events = root["events"];
    if (!events.isNull())
    {
        for(Json::Value::iterator iter = events.begin() ; events.end() != iter ; iter++)
        {
            // Get frameNo for the event and add to hash
            const char* c_str = iter.name().c_str();
            int frameNo = atoi(c_str);
            mEvents.insert(frameNo, EventList());

            // add event from event list
            Json::Value& eventList = *iter;
            for (unsigned int i = 0; i < eventList.size(); i++)
            {
                mEvents[frameNo].mList.push_back(QString::fromStdString(eventList[i].asString()));
            }
        }
    }
    // event data

    mSelectedKeyFramePosition.mFrameNo = 0;
    mSelectedKeyFramePosition.mLineNo = 0;

    emit refreshTimeLine();
    emit animationLoaded();
    return true;
}

QString AnimationModel::getLoadedAnimationPath() const
{
    QString path = mAnimationDirectory;
    return path.append("/").append(ResourceManager::GetDirectoryId(ResourceManager::FileType_Animation)).append("/").append(mAnimationID).append(".").append(ANIMATION_FORMAT);
}


void AnimationModel::executeCommand(int frameNo)
{
    // Execute event, it possible
    AnimationModel::EventList eventList = getEventList(frameNo);
    for (int i = 0; i < eventList.mList.count(); i++)
    {
        readCommand(eventList.mList[i]);
    }
}

void AnimationModel::readCommand(QString command)
{
    QStringList list = command.split(":");
    if(list.size() >=2)
    {
        if(list[0] == "snd")
        {
            QString soundName = QString(list[1]);
            ResourceManager::playSound(soundName);
        }
    }
}

void AnimationModel::createEmptyAnimation(QString path)
{
    // Get path without directory info & extension
    QString rootPath = ResourceManager::GetDirectoryPath(ResourceManager::FileType_Animation);

    QString dir = path;
    dir.replace(rootPath, "");

    // You cannot add file to the root
    if (dir != "")
    {
        // Get unique animation ID
        QString tempID = "new_animation%0";
        QString newID = "";
        int index = 0;
        while (true)
        {
            newID = tempID.arg(index);
            QString checkPath = path;
            checkPath.append("/").append(newID).append(".").append(ANIMATION_FORMAT);
            if (!QFile::exists(checkPath))
            {
                break;
            }
            index++;
        }

        AnimationModel* pAnimation = new AnimationModel(NULL, dir, newID);
        pAnimation->saveData();
        delete pAnimation;

        emit fileChanged();
    }
}

// --------- Timelines

// HACK: using animation model as an agent..
void AnimationModel::tellTimeLineToRefresh()
{
    emit refreshTimeLine();
}

void AnimationModel::moveTimeLine(int oldIndex, int newIndex)
{
    if (oldIndex >= mTimeline.count() || newIndex >= mTimeline.count()) {
        return;
    }
    if (newIndex > oldIndex) {
        mTimeline.insert(newIndex+1, mTimeline[oldIndex]);
        mTimeline.removeAt(oldIndex);
    } else {
        mTimeline.insert(newIndex, mTimeline[oldIndex]);
        mTimeline.removeAt(oldIndex+1);
    }
    emit refreshTimeLine();
}
