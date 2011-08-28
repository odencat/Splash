#include "EmittedAnimation.h"
#include "GLSprite.h"
#include "DataModels/AnimationModel.h"
#include <QTransform>
#include "Macros.h"
#include <cmath>

EmittedAnimation::EmittedAnimation(AnimationModel* pAnimationModel, const GLSprite* pParentGLSprite, float speed, int angle)
   :       mCounter(0)
{
    float dx = cos(angle * PI / 180.0);
    float dy = sin(angle * PI / 180.0);

    float speedX = speed * dx /(std::fabs(dx) + std::fabs(dy));
    float speedY = speed * dy /(std::fabs(dx) + std::fabs(dy));

    GLSprite::SpriteDescriptor spriteDescriptor = pParentGLSprite->mSpriteDescriptor;

    // Kinda hacky. I couldn't decompose transformation to each attribute
    spriteDescriptor.mOptionalTransform = pParentGLSprite->getParentTransform();

    // This will emit under world coordinate, therefore the priority needs to be come from its root
    spriteDescriptor.mPriority = pParentGLSprite->getRootSprite()->mSpriteDescriptor.mPriority;
    spriteDescriptor.mAlpha = pParentGLSprite->getAbsoluteAlpha() * spriteDescriptor.mAlpha;

    spriteDescriptor.mEmitter = false;
    mpGLSprite = new GLSprite(NULL, pAnimationModel, 0, spriteDescriptor, false, 0, 0, true, speedX, speedY);
}

EmittedAnimation::~EmittedAnimation()
{
    delete mpGLSprite;
}

void EmittedAnimation::update()
{
    if (mCounter < mpGLSprite->getParentAnimationModel()->getMaxFrameCount())
    {
        mpGLSprite->mSpriteDescriptor.mFrameNo = mCounter;
       
       // Move Sprite if velocity was specified
        mpGLSprite->mSpriteDescriptor.mPosition.mX += mpGLSprite->mSpeedX;
        mpGLSprite->mSpriteDescriptor.mPosition.mY += mpGLSprite->mSpeedY;

        mCounter++;
    }
}

const GLSprite* EmittedAnimation::getSprite() const
{
    return mpGLSprite;
}

bool EmittedAnimation::isDone() const
{
    return (mCounter >= mpGLSprite->getParentAnimationModel()->getMaxFrameCount());
}
