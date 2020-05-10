#ifndef ANIMATION_TYPES_H
#define ANIMATION_TYPES_H

#include "qgl.h"

enum PositionType
{
    PositionType_None,
    PositionType_RelativeToTarget,
    PositionType_RelativeToTargetOrigin,

    PositionType_COUNT
};

enum PositionTypeOption
{
    PositionTypeOption_TopLeft,
    PositionTypeOption_TopCenter,
    PositionTypeOption_TopRight,
    PositionTypeOption_CenterLeft,
    PositionTypeOption_Center,
    PositionTypeOption_CenterRight,
    PositionTypeOption_BottomLeft,
    PositionTypeOption_BottomCenter,
    PositionTypeOption_BottomRight,

    PositionTypeOption_COUNT
};

enum FacingOptionType
{
    FacingOptionType_none,
    FacingOptionType_lookAtTarget,
    FacingOptionType_FaceToMovingDir,

    FacingOptionType_COUNT
};

enum BlendType
{
    eBT_Alpha,
    eBT_Add,
    eBT_Sub,

    eBT_COUNT
};

struct Point2
{
    float mX;
    float mY;

    void operator=(const Point2& point)
    {
        mX = point.mX;
        mY = point.mY;
    }

    bool operator==(const QPoint& point)
    {
        return mX == point.x() && mY == point.y();
    }

    bool operator!=(const QPoint& point)
    {
        return !(mX == point.x() && mY == point.y());
    }
};

struct Color
{
    float mR;
    float mG;
    float mB;

    bool isZero() const
    {
        return mR == 0 && mG == 0 && mB == 0;
    }

    void operator=(const Color& color)
    {
        mR = color.mR;
        mG = color.mG;
        mB = color.mB;
    }

    bool operator==(const Color& color)
    {
        return  (mR == color.mR &&
                mG == color.mG &&
                mB == color.mB);
    }

    bool operator!=(const Color& color)
    {
        return  !(mR == color.mR &&
                mG == color.mG &&
                mB == color.mB);
    }

    Color ()
    {
        mR = 0;
        mG = 0;
        mB = 0;
    }

    Color (float r, float g, float b)
    {
        mR = r;
        mG = g;
        mB = b;
    }
};

static QPoint getPositionWithPositionType(QPointF basePosition, PositionTypeOption positionTypeOption, int width, int height, int selfWidth, int selfHeight)
{
    int dx = 0;
    int dy = 0;
    switch(positionTypeOption)
    {
        case PositionTypeOption_TopLeft:
            dx = -1;
            dy = -1;
        break;
        case PositionTypeOption_TopCenter:
            dx = 0;
            dy = -1;
        break;
        case PositionTypeOption_TopRight:
            dx = 1;
            dy = -1;
        break;
        break;
        case PositionTypeOption_CenterLeft:
            dx = -1;
            dy = 0;
        break;
        case PositionTypeOption_Center:
            dx = 0;
            dy = 0;
        break;
        case PositionTypeOption_CenterRight:
            dx = 1;
            dy = 0;
        break;
        case PositionTypeOption_BottomLeft:
            dx = -1;
            dy = 1;
        break;
        case PositionTypeOption_BottomCenter:
            dx = 0;
            dy = 1;
        break;
        case PositionTypeOption_BottomRight:
            dx = 1;
            dy = 1;
        break;
        default:
        break;
    }

    int deltaY = 0;
    if (dy == 1) {
        deltaY = -selfHeight;
    }
    return QPointF(basePosition.x() + dx * width, basePosition.y() + dy * height + deltaY).toPoint();
}
#endif // ANIMATION_TYPES_H
