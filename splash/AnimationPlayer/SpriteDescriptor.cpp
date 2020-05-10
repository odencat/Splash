#include "SpriteDescriptor.h"
#include "GLSprite.h"

void SpriteDescriptor::operator=(SpriteDescriptor spriteDescriptor)
{
    mEmitter = spriteDescriptor.mEmitter;
    mSourcePath = spriteDescriptor.mSourcePath;
    mPositionType = spriteDescriptor.mPositionType;
    mPositionTypeOption = spriteDescriptor.mPositionTypeOption;
    mFacingOptionType = spriteDescriptor.mFacingOptionType;
    mAlpha = spriteDescriptor.mAlpha;
    mPriority = spriteDescriptor.mPriority;
    mTextureSrcRect = spriteDescriptor.mTextureSrcRect;
    mBlendType = spriteDescriptor.mBlendType;
    mCenter = spriteDescriptor.mCenter;
    mScale = spriteDescriptor.mScale;
    mPosition = spriteDescriptor.mPosition;
    mRotation = spriteDescriptor.mRotation;
    mColor = spriteDescriptor.mColor;

    mMinEmitSpeed = spriteDescriptor.mMinEmitSpeed;
    mMaxEmitSpeed = spriteDescriptor.mMaxEmitSpeed;
    mMinEmitAngle = spriteDescriptor.mMinEmitAngle;
    mMaxEmitAngle = spriteDescriptor.mMaxEmitAngle;
}

QTransform SpriteDescriptor::getTransform() const
{
    QTransform transform;
    transform.translate(mPosition.mX, mPosition.mY);
    transform.rotate(mRotation);
    transform.scale(mScale.mX, mScale.mY);

    return transform * mOptionalTransform;
}

bool SpriteDescriptor::isImage() const
{
    return ResourceManager::getFileType(mSourcePath) == ResourceManager::FileType_Image;
}

QPoint SpriteDescriptor::textureCenter() const
{
    return QPoint(mTextureSrcRect.width()/2, mTextureSrcRect.height()/2);
}

QPoint SpriteDescriptor::center() const
{
    return QPointF(textureCenter().x() + mCenter.mX, textureCenter().y() + mCenter.mY).toPoint();
}

QPoint SpriteDescriptor::getPosition(const GLSprite* pTargetSprite) const
{
    if (mPositionType != PositionType_None)
    {
        int selfHeight = this->mTextureSrcRect.height();

        return getPositionWithPositionType(QPointF(mPosition.mX, mPosition.mY), mPositionTypeOption, pTargetSprite->mSpriteDescriptor.mTextureSrcRect.width(), pTargetSprite->mSpriteDescriptor.mTextureSrcRect.height(), selfHeight);
    }
    return QPointF(mPosition.mX, mPosition.mY).toPoint();
}


SpriteDescriptor SpriteDescriptor::makeDefaultSpriteDescriptor()
{
    SpriteDescriptor spriteDescriptor;
    spriteDescriptor.mSourcePath = "";
    spriteDescriptor.mBlendType = eBT_Alpha;
    spriteDescriptor.mFacingOptionType = FacingOptionType_none;
    spriteDescriptor.mPositionType = PositionType_None;
    spriteDescriptor.mPositionTypeOption = PositionTypeOption_Center;

    spriteDescriptor.mCenter.mX = 0;
    spriteDescriptor.mCenter.mY = 0;
    spriteDescriptor.mFrameNo = 0;

    spriteDescriptor.mColor.mR = 0;
    spriteDescriptor.mColor.mG = 0;
    spriteDescriptor.mColor.mB = 0;

    spriteDescriptor.mAlpha = 1.0;
    spriteDescriptor.mPriority = 0.5;
    spriteDescriptor.mPosition.mX = 0;
    spriteDescriptor.mPosition.mY = 0;

    spriteDescriptor.mRotation = 0;

    spriteDescriptor.mScale.mX = 1.0f;
    spriteDescriptor.mScale.mY = 1.0f;

    spriteDescriptor.mEmitter = false;
    spriteDescriptor.mMinEmitSpeed = 0;
    spriteDescriptor.mMaxEmitSpeed = 0;
    spriteDescriptor.mMinEmitAngle = 0;
    spriteDescriptor.mMaxEmitAngle = 0;

    spriteDescriptor.mTextureSrcRect = QRect(0, 0, 0, 0);

    return spriteDescriptor;
}
