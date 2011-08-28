#include "AnimationViewerPanel.h"
#include "DataModels/CelModel.h"
#include "DataModels/KeyFrame.h"
#include "DataModels/KeyFrameData.h"
#include "ResourceManager.h"
#include "GLSprite.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <QColor>
#include <QMessageBox>

#define TARGET_SCREEN_WIDTH 320
#define TARGET_SCREEN_HEIGHT 240

AnimationViewerPanel::AnimationViewerPanel(QWidget* parent, AnimationModel* pAnimationModel, CelModel* const pSelectedCelModel)
        : QGLWidget(parent),
          mpAnimationModel(pAnimationModel),
          mCurrentFrameNo(),
          mClearColor(Qt::black),
          mSelectedOffset(QPoint(0, 0)),
          mpSelectedCelModel(pSelectedCelModel),
          mIsAnimationPlaying(false),
          mCelGrabbed(false),
          mTargetGrabbed(false),
          mShowAnimationUI(true),
          mShowTarget(true),
          mShowCamera(true)
{
    setAutoFillBackground(false);


    connect(mpAnimationModel, SIGNAL(refreshTimeLine()), this, SLOT(refresh()));
    connect(mpAnimationModel, SIGNAL(selectedKeyFramePositionChanged(int, int)), this, SLOT(refresh()));
    connect(mpAnimationModel, SIGNAL(targetPositionMoved(int, int)), this, SLOT(refresh()));
}

AnimationViewerPanel::~AnimationViewerPanel()
{

}

bool AnimationViewerPanel::isAnimationPlaying() const
{
    return mIsAnimationPlaying;
}

void AnimationViewerPanel::setShowAnimationUI(bool showUI)
{
    mShowAnimationUI = showUI;
    refresh();
}

void AnimationViewerPanel::setShowTarget(bool value)
{
    mShowTarget = value;
    refresh();
}

void AnimationViewerPanel::setShowCamera(bool value)
{
    mShowCamera = value;
    refresh();
}
void AnimationViewerPanel::playAnimation()
{
    mIsAnimationPlaying = true;
}

void AnimationViewerPanel::stopAnimation()
{
    for (int lineNo = 0; lineNo < AnimationModel::LINE_COUNT; lineNo++)
    {
        while (!mEmittedAnimationList[lineNo].empty()) { delete mEmittedAnimationList[lineNo].takeFirst();}
    }
    mIsAnimationPlaying = false;
}

void AnimationViewerPanel::gotoNextFrame()
{
    const KeyFrame::KeyFramePosition& keyframePosition = mpAnimationModel->getCurrentKeyFramePosition();
    int frameNo =  keyframePosition.mFrameNo;
    if (frameNo < mpAnimationModel->getMaxFrameCount())
    {
       mpAnimationModel->selectCurrentKeyFramePosition(keyframePosition.mLineNo, keyframePosition.mFrameNo + 1);
    }
    else
    {
        refresh();
    }
}

bool AnimationViewerPanel::isAnimationExist() const
{
    bool emittedAnimationExists = false;
    for (int lineNo = 0; lineNo < AnimationModel::LINE_COUNT; lineNo++)
    {
        if (!mEmittedAnimationList[lineNo].empty())
        {
            emittedAnimationExists = true;
        }
    }
    return (mpAnimationModel->getCurrentKeyFramePosition().mFrameNo < mpAnimationModel->getMaxFrameCount()) || emittedAnimationExists;
}

void AnimationViewerPanel::resizeEvent(QResizeEvent *event)
{
}

QPoint AnimationViewerPanel::getCenterPoint() const
{
    return QPoint(
        (width()) / 2,
        (height()) / 2
    );
}

void AnimationViewerPanel::keyPressEvent (QKeyEvent* e)
{
    if (!mIsAnimationPlaying)
    {
        KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
        switch(e->key())
        {
            // Delete selected cel
            case Qt::Key_Delete:
                mpAnimationModel->clearFrames(currentPosition.mLineNo, currentPosition.mFrameNo, currentPosition.mFrameNo);
            break;
        }
    }
}

static bool refreshed = false;
void AnimationViewerPanel::refresh()
{
    if (!isAnimationPlaying() || isAnimationExist())
    {
        refreshed = true;
        clearSprites();
    }

    if (!isAnimationPlaying())
    {
        float targetX = mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mX;
        float targetY = mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mY;

        QPoint centerPoint = getCenterPoint();

        if (targetX < -centerPoint.x()){targetX = -centerPoint.x();}
        if (targetX > centerPoint.x()){targetX = centerPoint.x();}
        if (targetY < -centerPoint.y()){targetY = -centerPoint.y();}
        if (targetY > centerPoint.y()){targetY = centerPoint.y();}

        mpAnimationModel->setTargetSpritePosition(targetX, targetY);

        // set cel reference
        KeyFrame::KeyFramePosition keyframePosition = mpAnimationModel->getCurrentKeyFramePosition();
        KeyFrame* pKeyframe = mpAnimationModel->getKeyFrame(keyframePosition.mLineNo, keyframePosition.mFrameNo);

        if (pKeyframe)
        {
            mpSelectedCelModel->setKeyFrameDataReference(pKeyframe->mpKeyFrameData);
            emit celSelected(pKeyframe->mpKeyFrameData);
        }
        else
        {
            mpSelectedCelModel->setKeyFrameDataReference(NULL);
            emit celSelected(NULL);
        }
    }

     // Update target info
    GLSprite* pTargetMonsterSprite = mpAnimationModel->createGLSpriteAt(NULL, mpAnimationModel->getCurrentKeyFramePosition().mFrameNo, AnimationModel::LINE_target);
    if (pTargetMonsterSprite)
    {
        if (pTargetMonsterSprite->mLineNo == AnimationModel::LINE_target)
        {
            GLSprite::Point2 pt = spTargetSprite->mSpriteDescriptor.mPosition;
            pt.mX = pTargetMonsterSprite->mSpriteDescriptor.mPosition.mX + AnimationModel::TARGET_originX;
            pt.mY = pTargetMonsterSprite->mSpriteDescriptor.mPosition.mY + AnimationModel::TARGET_originY;
            spTargetSprite->mSpriteDescriptor.mPosition = pt;
            //spTargetSprite->mSpriteDescriptor.mTextureSrcRect = pTargetMonsterSprite->mSpriteDescriptor.mTextureSrcRect;
            spTargetSprite->mSpriteDescriptor.mCenter.mX = 0;
            spTargetSprite->mSpriteDescriptor.mCenter.mY = 0;
        }
        delete pTargetMonsterSprite;
    }

    mGlSpriteList = mpAnimationModel->createGLSpriteListAt(NULL, mpAnimationModel->getCurrentKeyFramePosition().mFrameNo);

    mRenderSpriteList.append(mGlSpriteList);
    for (int lineNo = 0; lineNo < AnimationModel::LINE_COUNT; lineNo++)
    {
        for (int i = mEmittedAnimationList[lineNo].count() - 1; i >= 0; i--)
        {
            mRenderSpriteList.push_back(mEmittedAnimationList[lineNo][i]->getSprite());
        }
    }
    qSort(mRenderSpriteList.begin(), mRenderSpriteList.end(), GLSprite::priorityLessThan);

    repaint();
}

void AnimationViewerPanel::paintEvent(QPaintEvent *event)
{
    if (mIsAnimationPlaying && !refreshed){return;}
    refreshed = false;
    // Get center point, all cel position should be relative to this
    QPoint centerPoint = getCenterPoint();

    // Start painter
    QPainter painter(this);

    // clear screen
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.fillRect(QRect(0, 0, width() - 1, height() - 1), Qt::SolidPattern);

    if (mShowAnimationUI) { renderCross(painter); }
    renderCelSprites(centerPoint, painter);
    if (mShowAnimationUI) { renderTargetSprite(centerPoint, painter);}

    painter.end();
}


void AnimationViewerPanel::renderCross(QPainter& painter)
{
    painter.setPen(QColor(120, 150, 200));
    painter.drawLine(QPoint(0, height() / 2), QPoint(width(), height() / 2));
    painter.drawLine(QPoint(width() / 2, 0), QPoint(width() / 2, height()));
}

void AnimationViewerPanel::renderCelSprites(const QPoint& centerPoint, QPainter& painter)
{
    KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();

    if (mIsAnimationPlaying)
    {
        mpAnimationModel->executeCommand(currentPosition.mFrameNo);
    }

    QList<const GLSprite*>::Iterator iter = mRenderSpriteList.begin();
    while (iter != mRenderSpriteList.end())
    {
        GLSprite* glSprite = (GLSprite*)*iter;

        if (glSprite->mLineNo == AnimationModel::LINE_target)
        {
            if (!mShowTarget)
            {
                iter++;
                continue;
            }
            GLSprite::Point2 position;
            position.mX = glSprite->mSpriteDescriptor.mPosition.mX + AnimationModel::TARGET_originX;
            position.mY = glSprite->mSpriteDescriptor.mPosition.mY + AnimationModel::TARGET_originY;
            glSprite->mSpriteDescriptor.mPosition = position;
         }

        if (!mShowCamera && glSprite->mLineNo == AnimationModel::LINE_camera)
        {
           iter++;
           continue;
        }

        // render sprite
        // Move rendering position depends on targeting position option
        int dx = 0;
        int dy = 0;
//        if (glSprite->mLineNo == AnimationModel::LINE_target)
//        {
//            dx = glSprite->mSpriteDescriptor.mTextureSrcRect.width() / 2;
//            dy = glSprite->mSpriteDescriptor.mTextureSrcRect.height() / 2;
//        }

        painter.translate(centerPoint.x() + dx, centerPoint.y() + dy);
        if (glSprite)
        {
            glSprite->render(QPoint(0, 0), painter,AnimationModel::getTargetSprite(), mIsAnimationPlaying, mEmittedAnimationList);
        }

        painter.translate(-centerPoint.x() - dx, -centerPoint.y() - dy);


        if (glSprite && !mIsAnimationPlaying && mShowAnimationUI)
        {
            renderCelBox(painter, glSprite, centerPoint - glSprite->mSpriteDescriptor.center());
        }

    iter++;
    }

    if (mIsAnimationPlaying)
    {
        // update emitted animations
        for (int lineNo = 0; lineNo < AnimationModel::LINE_COUNT; lineNo++)
        {
            for (int i = mEmittedAnimationList[lineNo].count() - 1; i >= 0; i--)
            {
                mEmittedAnimationList[lineNo][i]->update();
                if (mEmittedAnimationList[lineNo][i]->isDone())
                {
                    mEmittedAnimationList[lineNo].removeAt(i);
                }
            }
        }
    }
}

void AnimationViewerPanel::renderCelBox(QPainter& painter, const GLSprite* glSprite, QPoint spriteRenderPoint)
{
   KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();

    if (!glSprite->isSelectable())
    {
        // unselectable cel
        painter.setPen(QColor(100, 100, 200));
        painter.setOpacity(0.5);
    }
    else if (glSprite->mID == currentPosition.mLineNo)
    {
        renderCenterPointSprite(glSprite, spriteRenderPoint, painter);
        // Cel selecte color
        painter.setPen(Qt::yellow);
        painter.setOpacity(1.0);
    }
    else
    {
        // unselected cel
        painter.setPen(Qt::white);
        painter.setOpacity(0.5);
    }

    // Draw image border rectanble
    // Calculate draw position and draw
    QRect rect = glSprite->getRect();
    rect.translate(spriteRenderPoint);
    painter.drawRect(rect);

    // Draw Text
    painter.drawText(QRect(
                            spriteRenderPoint.x() + (int)glSprite->mSpriteDescriptor.mPosition.mX,
                            spriteRenderPoint.y() + (int)glSprite->mSpriteDescriptor.mPosition.mY,
                            16,
                            16
                           ),
                     Qt::AlignCenter, QString("%0").arg(glSprite->mID + 1));
}

void AnimationViewerPanel::renderCenterPointSprite(const GLSprite* pGlSprite, const QPoint& centerPoint, QPainter& painter)
{
    if (pGlSprite->mSpriteDescriptor.isImage())
    {
        // render center point
        GLSprite* centerPointSprite = AnimationModel::getCenterPointSprite();
        QPointF offset = QPointF(
                (pGlSprite->mSpriteDescriptor.mPosition.mX + pGlSprite->mSpriteDescriptor.center().x()),
                (pGlSprite->mSpriteDescriptor.mPosition.mY + pGlSprite->mSpriteDescriptor.center().y())
        );

        QPoint centerPointCenterPoint = centerPoint + offset.toPoint();

        painter.translate(centerPointCenterPoint.x(), centerPointCenterPoint.y());
        centerPointSprite->render(QPoint(0, 0), painter, AnimationModel::getTargetSprite(), false, mEmittedAnimationList);
        painter.translate(-centerPointCenterPoint.x(), -centerPointCenterPoint.y());
    }
}

void AnimationViewerPanel::renderTargetSprite(const QPoint& centerPoint, QPainter& painter)
{
    painter.setOpacity(mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mAlpha);

    painter.translate(centerPoint.x(), centerPoint.y());
    mpAnimationModel->getTargetSprite()->render(QPoint(0, 0), painter, NULL, false, mEmittedAnimationList);
    painter.translate(-centerPoint.x(), -centerPoint.y());
}

void AnimationViewerPanel::clearSprites()
{
    while (!mGlSpriteList.empty())
    {
        delete mGlSpriteList.takeFirst();
    }
    mRenderSpriteList.clear();
}

/* -------------------------------------------------------------------

 Cel list change event

---------------------------------------------------------------------*/
void AnimationViewerPanel::addNewCel(QPoint& relativePressedPosition)
{
    if (mpAnimationModel->getSelectedSourcePath() != "")
    {
        GLSprite::Point2 position;
        position.mX = relativePressedPosition.x();
        position.mY = relativePressedPosition.y();

        KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
        mpAnimationModel->setKeyFrame(currentPosition.mLineNo, currentPosition.mFrameNo, position);
    }
}

void AnimationViewerPanel::selectCel(int lineNo)
{
    KeyFrame::KeyFramePosition keyframePosition = mpAnimationModel->getCurrentKeyFramePosition();
    if (keyframePosition.mLineNo != lineNo)
    {
        mpAnimationModel->selectCurrentKeyFramePosition(lineNo, keyframePosition.mFrameNo);
    }
}

bool AnimationViewerPanel::grabTarget(QPoint& relativePressedPosition)
{
    if(mpAnimationModel->getTargetSprite()->contains(
            relativePressedPosition
       )
    )
    {
        mSelectedOffset = mpAnimationModel->getTargetSprite()->getRect().topLeft() - relativePressedPosition;
        mTargetGrabbed = true;
    }
    return mTargetGrabbed;
}

void AnimationViewerPanel::grabCel(QPoint& relativePressedPosition)
{
    // Select a cel, if mouse has clicked on it
    for (int i = mGlSpriteList.count() - 1; i >= 0; i--)
    {
        const GLSprite* glSprite = mGlSpriteList[i];
        if (glSprite->isSelectable() &&
            glSprite->contains(
                    relativePressedPosition                   )
            )
        {
            mSelectedOffset = glSprite->getRect().topLeft() - relativePressedPosition;
            selectCel(glSprite->mID);
            mCelGrabbed = true;
            break;
        }
    }
}

void AnimationViewerPanel::mousePressEvent(QMouseEvent *event)
{
    this->setFocus();

    // Get center point here
    QPoint centerPoint = getCenterPoint();
    // Calculate pressed position relative from center
    QPoint relativePressedPosition = QPoint(event->x(), event->y()) - centerPoint;

    mTargetGrabbed  = false;
    mCelGrabbed = false;

    // only for edit mode
    if (!mIsAnimationPlaying)
    {
        // center point mode
        if (event->modifiers() & Qt::ControlModifier)
        {
            setCenterPoint(event);
            return;
        }

        grabCel(relativePressedPosition);

        if(!mCelGrabbed)
        {
            QString path1 = mpAnimationModel->getLoadedAnimationPath();
            QString path2 = mpAnimationModel->getSelectedSourcePath();
          if (mpAnimationModel->getLoadedAnimationPath() == mpAnimationModel->getSelectedSourcePath())
          {
              QMessageBox::information(window(), tr("Animation nest error"),
                     tr("You cannot nest the same animation"));
          }
          else
          {
            KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
            switch(ResourceManager::getFileType(mpAnimationModel->getSelectedSourcePath()))
            {
                case ResourceManager::FileType_Animation:
                case ResourceManager::FileType_Image:
                    if (mpAnimationModel->getKeyFrameIndex(currentPosition.mLineNo, currentPosition.mFrameNo) == -1)
                    {
                        GLSprite::Point2 pt;
                        pt.mX = relativePressedPosition.x();
                        pt.mY = relativePressedPosition.y();

                        mpAnimationModel->setKeyFrame(currentPosition.mLineNo, currentPosition.mFrameNo, pt);
                    }
                    else
                    {
                        if (event->modifiers() & Qt::ShiftModifier)
                        {
                            swapSourceTexture();
                        }
                    }
                break;

                default:
                break;
            }
          }
        }
    }

    if (!mCelGrabbed)
    {
        grabTarget(relativePressedPosition);
    }
}

// When the user clicked on the canvas with Shift key pressed,
// it causes the cel to swap its texture to currenly selected palet
void AnimationViewerPanel::swapSourceTexture()
{
    KeyFrameData* pKeyFrameData = mpSelectedCelModel->getKeyFrameDataReference();
    if (pKeyFrameData)
    {
        QString path = mpAnimationModel->getSelectedSourcePath();
        if (path != "")
        {
            if (ResourceManager::getFileType(path) == ResourceManager::FileType_Animation)
            {
                // Animation is not swappable!
                mpSelectedCelModel->setSwapTargetType(KeyFrameData::SwapTargetType_None);
            }

            mpSelectedCelModel->setSourceTexture(path, mpAnimationModel->mSelectedPaletTextureSrcRect);
            refresh();
        }
    }
}

void AnimationViewerPanel::setCenterPoint(QMouseEvent *event)
{
    KeyFrameData* pKeyFrameData = mpSelectedCelModel->getKeyFrameDataReference();
    if (pKeyFrameData && pKeyFrameData->mSpriteDescriptor.isImage())
    {
        QPoint centerPoint = getCenterPoint();
        int centerX = (int)(event->x() - centerPoint.x() - pKeyFrameData->mSpriteDescriptor.mPosition.mX + pKeyFrameData->mSpriteDescriptor.mCenter.mX);
        int centerY = (int)(event->y() - centerPoint.y() - pKeyFrameData->mSpriteDescriptor.mPosition.mY + pKeyFrameData->mSpriteDescriptor.mCenter.mY);

        mpSelectedCelModel->setCenterX(centerX);// / pKeyFrameData->mSpriteDescriptor.textureCenter().x());
        mpSelectedCelModel->setCenterY(centerY);// / pKeyFrameData->mSpriteDescriptor.textureCenter().y());
    }
}

void AnimationViewerPanel::mouseMoveEvent(QMouseEvent *event)
{
    QPoint centerPoint = getCenterPoint();
    int newPosX = event->x() - centerPoint.x() + mSelectedOffset.x();
    int newPosY = event->y() - centerPoint.y() + mSelectedOffset.y();
    KeyFrameData* pKeyFrameData = mpSelectedCelModel->getKeyFrameDataReference();

    if (event->modifiers() & Qt::ControlModifier)
    {
        setCenterPoint(event);
    }
    else if (mCelGrabbed)
    {
        // Move cel if it is selected
        if (pKeyFrameData)
        {
            const KeyFrame::KeyFramePosition& keyframePosition = mpAnimationModel->getCurrentKeyFramePosition();
            if (pKeyFrameData->mSpriteDescriptor.mPositionType != GLSprite::PositionType_None)
            {
                newPosX -= (int)mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mX;
                newPosY -= (int)mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mY;
            }

            if(keyframePosition.mLineNo == AnimationModel::LINE_target)
            {
                newPosX -= AnimationModel::TARGET_originX;
                newPosY -= AnimationModel::TARGET_originY;
            }
            mpSelectedCelModel->setPositionX(newPosX);
            mpSelectedCelModel->setPositionY(newPosY);
        }
    }
    else if (mTargetGrabbed)
    {
        mpAnimationModel->setTargetSpritePosition(newPosX, newPosY);
        refresh();
    }
}

void AnimationViewerPanel::mouseReleaseEvent(QMouseEvent *event)
{
    mCelGrabbed = false;
    mTargetGrabbed = false;
}

void AnimationViewerPanel::timerEvent()
{
 
}
