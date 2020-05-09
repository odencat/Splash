#ifndef BaseKEYFRAME_H
#define BaseKEYFRAME_H

#include "SpriteDescriptor.h"
#include "GLSprite.h"
class KeyFrameData
{
public:
    enum SwapTargetType
    {
        SwapTargetType_None,
        SwapTargetType_Character,
        SwapTargetType_Weapon,

        SwapTargetType_COUNT
    };


    enum TweenType
    {
        eTT_None,
        eTT_Fix,
        eTT_Linear,
        eTT_EaseIn,
        eTT_EaseOut,
        eTT_EaseInOut,

        eTT_COUNT
    };

    enum TweenAttribute
    {
      TweenAttribute_any = -1,
      TweenAttribute_position = 0,
      TweenAttribute_rotation,
      TweenAttribute_scale,
      TweenAttribute_alpha,
      TweenAttribute_color,

      TweenAttribute_COUNT
    };

    KeyFrameData();

    SpriteDescriptor mSpriteDescriptor;
    SwapTargetType mSwapTargetType;
    bool mHideActor;
    TweenType mTweenTypes[TweenAttribute_COUNT];
    bool hasTween() const;
    bool allAttributesNone() const;

    static QString swapTargetTypeString[SwapTargetType_COUNT];
    static QString positionTypeString[PositionType_COUNT];
    static QString positionTypeOptionString[PositionTypeOption_COUNT];
    static QString tweenAttributeSting[TweenAttribute_COUNT];
    static QString tweenTypeSting[eTT_COUNT];


    static PositionTypeOption getPositionTypeOptionByString(QString typeString);
    static SwapTargetType getSwapTargetTypeByString(QString typeString);
    static PositionType getPositionTypeByString(QString typeString);
    static TweenType getTweenTypeByString(QString typeString);
};

#endif // KEYFRAMEDATA_H
