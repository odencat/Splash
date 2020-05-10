#ifndef GLSPRITE_H
#define GLSPRITE_H
#include "qgl.h"
#include "Types.h"
#include <QMatrix>
#include "DataModels/KeyFrame.h"
#include "ResourceManager.h"
#include "EmittedAnimation.h"
#include "SpriteDescriptor.h"

class QPainter;
class QPoint;
class AnimationModel;
class QPixmap;
class GLSprite
{
public:
    static QString blendTypeSting[eBT_COUNT];
    static BlendType getBlendTypeByString(QString typeString) ;
    static QString facingOptionTypeSting[eBT_COUNT];
    static FacingOptionType getFacingOptionTypeByString(QString typeString) ;
    static bool priorityLessThan(const GLSprite* pItem1, const GLSprite* pItem2);

    GLSprite(const GLSprite* pGLSprite, const AnimationModel* pAnimationModel, const int& id,  const SpriteDescriptor& spriteDescriptor, bool selectable, int lineNo, int frameNo, bool emitted, float dx, float dy);
    GLSprite(const GLSprite* pGLSprite, const AnimationModel* pAnimationModel, const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, QPixmap* pPixmap);

    const AnimationModel* getParentAnimationModel() const;
    const GLSprite* getParentSprite() const;
    const GLSprite* getRootSprite() const;
    float getAbsoluteAlpha() const;
    Color getAbsoluteColor() const;

    bool isEmitted() const;
    QList<KeyFrame::KeyFramePosition> getNodePath() const;

    bool isSelectable() const;

    void renderColored8(QPainter& painter, QImage* pImage, const QPointF& dstPoint, const QRect& srcRect, Color color) const;
    void renderColored32(QPainter& painter, QImage* pImage, const QPointF& dstPoint, const QRect& srcRect, Color color) const;
    bool renderColored(QPainter& painter, const QPointF& dstPoint, const QRect& srcRect) const;
    void render(QPoint offset, QPainter& painter, const GLSprite* pTargetSprite, bool isPlaying, QList<EmittedAnimation*>* emittedAnimationList) const;
    QRect getRect() const;
    bool contains(QPoint point) const;

    QTransform getTransform() const;
    QTransform getTotalTransform() const;
    QTransform getParentTransform() const;

    int mID;
    SpriteDescriptor mSpriteDescriptor;

    const int mLineNo;
    const int mFrameNo;

    const float mSpeedX;
    const float mSpeedY;
private:

    const GLSprite* mpParentGLSprite;
    const AnimationModel* mpParentAnimationModel;

    bool mIsSelectable;
    QPixmap* mpPixmap;
    QTransform mParentTransform;
    bool mEmitted;
};
#endif // GLSPRITE_H
