#include "GLSprite.h"

#include "Macros.h"
#include <math.h>
#include <QGlobal.h>
#include <QPixmap>
#include <QPainter>
#include "DataModels/AnimationModel.h"
#include "DataModels/KeyFrame.h"
#include "ResourceManager.h"

static uint tempColorTable[256];

static QImage tempImage32 = QImage(256, 256, QImage::Format_ARGB32);

bool GLSprite::priorityLessThan(const GLSprite* pItem1, const GLSprite* pItem2)
{
    return pItem1->mSpriteDescriptor.mPriority < pItem2->mSpriteDescriptor.mPriority;
}

QPoint GLSprite::getPositionWithPositionType(QPointF basePosition, GLSprite::PositionTypeOption positionTypeOption, int width, int height)
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
    return QPointF(basePosition.x() + dx * width, basePosition.y() + dy * height).toPoint();
}

QString GLSprite::blendTypeSting[GLSprite::eBT_COUNT] =
{
    "alpha",
    "add",
    "sub"
};

GLSprite::BlendType GLSprite::getBlendTypeByString(QString typeString)
{
    for (int i = 0; i < eBT_COUNT; i++)
    {
        if (typeString == blendTypeSting[i])
        {
            return static_cast<GLSprite::BlendType>(i);;
        }
    }
    return eBT_Alpha;
}

QString GLSprite::facingOptionTypeSting[GLSprite::FacingOptionType_COUNT] =
{
    "none",
    "lookAtTarget",
    "faceToMov"
};

GLSprite::FacingOptionType GLSprite::getFacingOptionTypeByString(QString typeString)
{
    for (int i = 0; i < eBT_COUNT; i++)
    {
        if (typeString == facingOptionTypeSting[i])
        {
            return static_cast<GLSprite::FacingOptionType>(i);
        }
    }
    return FacingOptionType_none;
}

GLSprite::SpriteDescriptor GLSprite::makeDefaultSpriteDescriptor()
{
    GLSprite::SpriteDescriptor spriteDescriptor;
    spriteDescriptor.mSourcePath = "";
    spriteDescriptor.mBlendType = GLSprite::eBT_Alpha;
    spriteDescriptor.mFacingOptionType = FacingOptionType_none;
    spriteDescriptor.mPositionType = GLSprite::PositionType_None;
    spriteDescriptor.mPositionTypeOption = GLSprite::PositionTypeOption_Center;

    spriteDescriptor.mBlur = 0;
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

static QPainter::CompositionMode sCompositionMode[GLSprite::eBT_COUNT] =
{
    QPainter::CompositionMode_SourceOver,
    QPainter::CompositionMode_Plus,
    QPainter::CompositionMode_SourceOver
};

GLSprite::GLSprite(const GLSprite* pGLSprite, const AnimationModel* pAnimationModel, const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, int lineNo, int frameNo, bool emitted, float speedX, float speedY)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mLineNo(lineNo),
          mFrameNo(frameNo),
          mSpeedX(speedX),
          mSpeedY(speedY),
          mpParentGLSprite(pGLSprite),
          mpParentAnimationModel(pAnimationModel),
          mIsSelectable(selectable),
          mpPixmap(NULL),
          mEmitted(emitted)

{
    mEmitted = emitted;
}

//
GLSprite::GLSprite(const GLSprite* pGLSprite, const AnimationModel* pAnimationModel, const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, QPixmap* pPixmap)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mLineNo(0),
          mFrameNo(0),
          mSpeedX(0),
          mSpeedY(0),
          mpParentGLSprite(pGLSprite),
          mpParentAnimationModel(pAnimationModel),
          mIsSelectable(selectable),
          mpPixmap(pPixmap),
          mEmitted(false)
{
}


QTransform GLSprite::getParentTransform() const
{
    QTransform transform;
    if (mpParentGLSprite)
    {
        transform = transform * mpParentGLSprite->getCombinedTransform();
    }
    return transform;
}

const GLSprite* GLSprite::getParentSprite() const
{
    return mpParentGLSprite;
}

const AnimationModel* GLSprite::getParentAnimationModel() const
{
    return mpParentAnimationModel;
}

bool GLSprite::isSelectable() const
{
    return mIsSelectable;
}

void GLSprite::renderColored8(QPainter& painter, QImage* pImage, const QPointF& dstPoint, const QRect& srcRect, Color color) const
{
    int dR = (int)(255 * color.mR);
    int dG = (int)(255 * color.mG);
    int dB = (int)(255 * color.mB);

    QVector<QRgb> colors = pImage->colorTable();
    for (int i = 1; i < pImage->numColors() ; i++)
    {
        tempColorTable[i] = colors[i];
        QRgb rgb = colors[i];
        int r = CLIP((int)(qRed(rgb) + dR), 0, 255);
        int g = CLIP((int)(qGreen(rgb) + dG), 0, 255);
        int b = CLIP((int)(qBlue(rgb) + dB), 0, 255);

        colors[i] = qRgb(r, g, b);
    }
    pImage->setColorTable(colors);

    painter.drawImage(dstPoint, *pImage, srcRect);
    for (int i = 1; i < pImage->numColors() ; i++)
    {
        colors[i] = tempColorTable[i];
    }
    pImage->setColorTable(colors);

    // set color table back
}

void GLSprite::renderColored32(QPainter& painter, QImage* pImage, const QPointF& dstPoint, const QRect& srcRect, Color color) const
{
    int dR = (int)(255 * color.mR);
    int dG = (int)(255 * color.mG);
    int dB = (int)(255 * color.mB);

    QPainter tempPainter32(&tempImage32);
    QRect tempRect = QRect(0, 0, srcRect.width(), srcRect.height());
    tempPainter32.setCompositionMode(QPainter::CompositionMode_Source);
    tempPainter32.drawImage(tempRect, *pImage, srcRect);
    tempPainter32.end();

    for (int y = 0; y < srcRect.height(); y++)
    {
        uint* row = (uint *)tempImage32.scanLine(y);
        for (int x = 0; x < srcRect.width(); x++)
        {
            uint *p = row + x;
            int r = CLIP((int)(qRed(*p) + dR), 0, 255);
            int g = CLIP((int)(qGreen(*p) + dG), 0, 255);
            int b = CLIP((int)(qBlue(*p) + dB), 0, 255);

            *p = qRgba(r, g, b, qAlpha(*p));
        }
    }

    painter.drawImage(dstPoint, tempImage32, tempRect);
}

// This is very slow implementation of color transformation
// It needs to be switched to OpenGL base if we want to have the best performance.
bool GLSprite::renderColored(QPainter& painter, const QPointF& dstPoint, const QRect& srcRect) const
{
    Color color = getAbsoluteColor();
    if (color.isZero())
    {
        return false;
    }

    // Change Color
    QImage* pImage = ResourceManager::getImage(mSpriteDescriptor.mSourcePath);

    switch(pImage->format())
    {
        case (QImage::Format_Indexed8):
            renderColored8(painter, pImage, dstPoint, srcRect, color);
            return true;

        case (QImage::Format_ARGB32):
            renderColored32(painter, pImage, dstPoint, srcRect, color);
            return true;
        default:
            break;
    }

    return false;
}

void GLSprite::render(QPoint offset, QPainter& painter, const GLSprite* pTargetSprite, bool isPlaying, QList<EmittedAnimation*>* emittedAnimationList) const
{
    QPointF spritePosition = QPointF(mSpriteDescriptor.mPosition.mX, mSpriteDescriptor.mPosition.mY);
    QPointF spriteRenderPoint = spritePosition;

    // Choose Blend Type
    painter.setCompositionMode(sCompositionMode[mSpriteDescriptor.mBlendType]);

    // Alpha
    painter.setOpacity(getAbsoluteAlpha());

   // Rotation & Scale & translate
    QTransform saveTransform = painter.combinedTransform();
    painter.setTransform(getTransform(), true);

    // Get source texture rectangle (Not used for sub animation)
    QRect srcRect = mSpriteDescriptor.mTextureSrcRect;

    // Where it actually render the iamge
    QPointF dstPoint = QPointF(-mSpriteDescriptor.mCenter.mX, -mSpriteDescriptor.mCenter.mY) - mSpriteDescriptor.textureCenter() + offset;
    QPixmap* pQPixmap;
    if (mpPixmap)
    {
        pQPixmap = mpPixmap;
        if (pQPixmap) {painter.drawPixmap(dstPoint, *pQPixmap, srcRect);}
    }
    else
    {
         QString sourcePath = mSpriteDescriptor.mSourcePath;
         switch(ResourceManager::getFileType(sourcePath))
         {
            case ResourceManager::FileType_Image:
                bool rendered = renderColored(painter, dstPoint, srcRect);
                if (!rendered)
                {
                    QImage* pImage = ResourceManager::getImage(mSpriteDescriptor.mSourcePath);
                    painter.drawImage(dstPoint, *pImage, srcRect);
                }
                break;

            case ResourceManager::FileType_Animation:
                // Render Subanimation
                AnimationModel* pAnimationModel = ResourceManager::getAnimation(sourcePath);
                if (pAnimationModel)
                {
                    if (pAnimationModel->getMaxFrameCount() > 0)
                    {
                        int subFrameNo = 0;
                        // Emitted Animation Should be always start from frame 0
                        // Non-emitted animation needs to know in what frameNo they are
                        if (!mSpriteDescriptor.mEmitter)
                        {
                            subFrameNo = mSpriteDescriptor.mFrameNo % pAnimationModel->getMaxFrameCount();
                        }

                        // If the sprite has emitter option, emit new animation process
                        if (mSpriteDescriptor.mEmitter && isPlaying)
                        {
                            float speed = randFloat(mSpriteDescriptor.mMinEmitSpeed, mSpriteDescriptor.mMaxEmitSpeed);
                            int angle = randInt(mSpriteDescriptor.mMinEmitAngle, mSpriteDescriptor.mMaxEmitAngle);

                            emittedAnimationList[mLineNo].push_back(new EmittedAnimation(pAnimationModel, this, speed, angle));
                        }
                        else
                        {
                            QList<const GLSprite*> glSpriteList = pAnimationModel->createGLSpriteListAt(this, subFrameNo);
                            for (int i = 0; i < glSpriteList.count(); i++)
                            {
                                glSpriteList[i]->render(QPoint(0, 0), painter, pTargetSprite, isPlaying, emittedAnimationList);
                            }
                            // Remove glsprites, they are not going to be used anymore.
                            while (!glSpriteList.empty()) { delete glSpriteList.takeFirst();}
                        }

                        // Execute Event Command
                        if (isPlaying)
                        {
                            pAnimationModel->executeCommand(subFrameNo);
                        }
                    }
                }

                break;
             default:

             break;
         }
    }

    painter.setTransform(saveTransform, false);
}

const GLSprite* GLSprite::getRootSprite() const
{
    const GLSprite* pRootSprite = this;

    const GLSprite* pSprite = this;
    while (pSprite = pSprite->getParentSprite())
    {
        pRootSprite = pSprite;
    }
    return pRootSprite;
}

float GLSprite::getAbsoluteAlpha() const
{
    float alpha = mSpriteDescriptor.mAlpha;

    const GLSprite* pSprite = this;
    while (pSprite = pSprite->getParentSprite())
    {
        alpha *= pSprite->mSpriteDescriptor.mAlpha;
    }
    return alpha;
}

GLSprite::Color GLSprite::getAbsoluteColor() const
{
    Color color = mSpriteDescriptor.mColor;

    const GLSprite* pSprite = this;
    while (pSprite = pSprite->getParentSprite())
    {
        color.mR += pSprite->mSpriteDescriptor.mColor.mR;
        color.mG += pSprite->mSpriteDescriptor.mColor.mG;
        color.mB += pSprite->mSpriteDescriptor.mColor.mB;
    }
    color.mR = CLIP(color.mR, -1, 1);
    color.mG = CLIP(color.mG, -1, 1);
    color.mB = CLIP(color.mB, -1, 1);

    return color;
}

QTransform GLSprite::getTransform() const
{
    return mSpriteDescriptor.getTransform();
}

QTransform GLSprite::getCombinedTransform() const
{
    return getParentTransform() * getTransform();
}

bool GLSprite::isEmitted() const
{
    return mEmitted;
}

QList<KeyFrame::KeyFramePosition> GLSprite::getNodePath() const
{
    QList<KeyFrame::KeyFramePosition> list;
    list.push_front(KeyFrame::KeyFramePosition(mLineNo, mFrameNo));

    bool isRootEmitted = false;
    const GLSprite* pSprite = this;
    while(pSprite = pSprite->getParentSprite())
    {
        isRootEmitted = pSprite->isEmitted();
        list.push_front(KeyFrame::KeyFramePosition(pSprite->mLineNo, pSprite->mFrameNo));
    }

    // if this is an emitted animation, we don't need the root node!
    if (isRootEmitted)
    {
        list.pop_front();
    }
    return list;
}

QRect GLSprite::getRect() const
{
    return QRect(
        (int)mSpriteDescriptor.mPosition.mX,
        (int)mSpriteDescriptor.mPosition.mY,
        (int)mSpriteDescriptor.mTextureSrcRect.width(),
        (int)mSpriteDescriptor.mTextureSrcRect.height()
    );
}

bool GLSprite::contains(QPoint point) const
{
    point += QPoint((int)(mSpriteDescriptor.mCenter.mX), (int)(mSpriteDescriptor.mCenter.mY));
    point += mSpriteDescriptor.textureCenter();
    return getRect().contains(point, true);
}
