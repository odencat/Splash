#include "KeyFrameData.h"
#include "GLSprite.h"
#include "SpriteDescriptor.h"

QString KeyFrameData::swapTargetTypeString[KeyFrameData::SwapTargetType_COUNT] =
{
    "none",
    "character",
    "weapon"
};

KeyFrameData::SwapTargetType KeyFrameData::getSwapTargetTypeByString(QString typeString)
{
    for (int i = 0; i < SwapTargetType_COUNT; i++)
    {
        if (typeString == swapTargetTypeString[i])
        {
            return static_cast<KeyFrameData::SwapTargetType>(i);
        }
    }
    return SwapTargetType_None;
}

QString KeyFrameData::positionTypeOptionString[PositionTypeOption_COUNT] =
{
    "topLeft",
    "topCenter",
    "topRight",

    "centerLeft",
    "center",
    "centerRight",

    "bottomLeft",
    "bottomCenter",
    "bottomRight"
};

PositionTypeOption KeyFrameData::getPositionTypeOptionByString(QString typeString)
{
    for (int i = 0; i < PositionTypeOption_COUNT; i++)
    {
        if (typeString == positionTypeOptionString[i])
        {
            return static_cast<PositionTypeOption>(i);
        }
    }
    return PositionTypeOption_Center;
}


QString KeyFrameData::positionTypeString[PositionType_COUNT] =
{
    "none",
    "relativeToTarget",
    "relativeToTargetOrigin"
};

PositionType KeyFrameData::getPositionTypeByString(QString typeString)
{
    for (int i = 0; i < PositionType_COUNT; i++)
    {
        if (typeString == positionTypeString[i])
        {
            return static_cast<PositionType>(i);
        }
    }
    return PositionType_None;
}

QString KeyFrameData::tweenAttributeSting[KeyFrameData::TweenAttribute_COUNT] =
{
    "positionTween",
    "rotationTween",
    "scaleTween",
    "alphaTween",
    "colorTween"
};

QString KeyFrameData::tweenTypeSting[KeyFrameData::eTT_COUNT] =
{
    "none",
    "fix",
    "linear",
    "easeIn",
    "easeOut",
    "easeInOut"
};

KeyFrameData::TweenType KeyFrameData::getTweenTypeByString(QString typeString)
{
    for (int i = 0; i < eTT_COUNT; i++)
    {
        if (typeString == tweenTypeSting[i])
        {
            return static_cast<KeyFrameData::TweenType>(i);
        }
    }
    return eTT_None;
}

KeyFrameData::KeyFrameData()
{
    mSpriteDescriptor = SpriteDescriptor::makeDefaultSpriteDescriptor();

    for (int i = 0; i < TweenAttribute_COUNT; i++)
    {
        mTweenTypes[i] = eTT_None;
    }
    mSwapTargetType = SwapTargetType_None;
    mHideActor = false;
}

bool KeyFrameData::hasTween() const
{
    bool tween = false;
    for (int i = 0; i < TweenAttribute_COUNT; i++)
    {
        if (mTweenTypes[i] != eTT_None && mTweenTypes[i] != eTT_Fix)
        {
            tween = true;
        }
    }

    return tween;
}

bool KeyFrameData::allAttributesNone() const
{
    bool allNone = true;
    for (int i = 0; i < TweenAttribute_COUNT; i++)
    {
        if (mTweenTypes[i] != eTT_None)
        {
            allNone = false;
        }
    }

    return allNone;
}
