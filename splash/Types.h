#ifndef TYPES_H
#define TYPES_H
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
#endif // TYPES_H
