#ifndef SPRITERDESCRIPTOR_H
#define SPRITERDESCRIPTOR_H

#include "qgl.h"
#include "Types.h"

class GLSprite;
struct SpriteDescriptor
{
    QString mSourcePath;

    PositionType mPositionType;
    PositionTypeOption mPositionTypeOption;
    FacingOptionType mFacingOptionType;

    QRect mTextureSrcRect; // only valid when it is not a child animation
    BlendType mBlendType;

    Point2 mCenter;
    Point2 mScale;
    Point2 mPosition;
    int mRotation;

    Color mColor;

    float mAlpha;
    float mPriority;

    // Sub animation data
    int mFrameNo; // frameNo for sub animation

    // tell whether this is an emitter of subanimation (valid only this sprite is sub animation
    bool mEmitter;

    float mMinEmitSpeed;
    float mMaxEmitSpeed;
    int mMinEmitAngle;
    int mMaxEmitAngle;
    QTransform mOptionalTransform;

    void operator=(SpriteDescriptor spriteDescriptor);
    QTransform getTransform() const;

    bool isImage() const;

    QPoint textureCenter() const;
    QPoint center() const;
    QPoint getPosition(const GLSprite* pTargetSprite) const;
    static SpriteDescriptor makeDefaultSpriteDescriptor();
};


#endif // SPRITERDESCRIPTOR_H
