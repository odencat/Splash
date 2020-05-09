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
#define SNAP_PIXELS 5

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
    connect(mpAnimationModel, SIGNAL(animationTypeChanged(int)), this, SLOT(refresh()));
}

AnimationViewerPanel::~AnimationViewerPanel()
{

}

bool AnimationViewerPanel::isAnimationPlaying() const
{
    return mIsAnimationPlaying;
}


void AnimationViewerPanel::setZoom(float value) {
    mZoom = value;
    refresh();
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
    while (!mEmittedAnimationList.empty())
    {
        delete mEmittedAnimationList.takeFirst();
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
    return (mpAnimationModel->getCurrentKeyFramePosition().mFrameNo < mpAnimationModel->getMaxFrameCount()) || !mEmittedAnimationList.empty();
}

void AnimationViewerPanel::resizeEvent(QResizeEvent *event)
{
}

QPoint AnimationViewerPanel::getCenterPoint() const
{
    QSize guideSize = mpAnimationModel->guideSize();
    return QPoint(
        (width()) / 2 - guideSize.width() / 2,
        (height()) / 2 + guideSize.height() / 2
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
            case Qt::Key_D:
                mpAnimationModel->clearFrames(currentPosition.mLineNo, currentPosition.mFrameNo, currentPosition.mFrameNo);
            break;
        case Qt::Key_Right:
            mpSelectedCelModel->movePosition(1, 0);
        break;
        case Qt::Key_Up:
            mpSelectedCelModel->movePosition(0, -1);
        break;
        case Qt::Key_Left:
            mpSelectedCelModel->movePosition(-1, 0);
        break;
        case Qt::Key_Down:
            mpSelectedCelModel->movePosition(0, 1);
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
        int x1 = -centerPoint.x();
        int y1 = -centerPoint.y();
        int x2 = width() - centerPoint.x();
        int y2 = height() - centerPoint.y();
        if (targetX < x1){targetX = x1;}
        if (targetX > x2){targetX = x2;}
        if (targetY < y1){targetY = y1;}
        if (targetY > y2){targetY = y2;}

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

    mGlSpriteList = mpAnimationModel->createGLSpriteListAt(NULL, mpAnimationModel->getCurrentKeyFramePosition().mFrameNo);

    mRenderSpriteList.append(mGlSpriteList);
    for (int i = mEmittedAnimationList.count() - 1; i >= 0; i--)
    {
        mRenderSpriteList.push_back(mEmittedAnimationList[i]->getSprite());
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
    painter.fillRect(QRect(0, 0, width() , height() ), Qt::SolidPattern);

    painter.translate(width() / 2, height() / 2);
    painter.scale(mZoom, mZoom);
    painter.translate(-width() / 2, -height() / 2);

    renderCelSprites(centerPoint, painter);
    if (mShowAnimationUI) {
        renderMask(painter);
        renderCross(painter);
    }
    if (mShowTarget) {
        renderTargetSprite(centerPoint, painter);
    }

    painter.end();
}

void AnimationViewerPanel::renderMask(QPainter& painter) const
{
    QSize guideSize = mpAnimationModel->guideSize();
    if (guideSize.width() == 0 || guideSize.height() == 0) {
        return;
    }
    // Background Guide
    int x1 = (this->width() - guideSize.width()) / 2;
    int y1 = (this->height() - guideSize.height()) / 2;
    int x2 = this->width() - x1;
    int y2 = this->height() - y1;
    int w = x1;
    int h = y1;

    painter.setPen(QColor(220, 150, 200, 100));
    painter.drawLine(QLine(x1, y1, x1, y2));
    painter.drawLine(QLine(x1, y2, x2, y2));
    painter.drawLine(QLine(x2, y2, x2, y1));
    painter.drawLine(QLine(x2, y1, x1, y1));
    if (mIsAnimationPlaying) {
        painter.fillRect(QRect(0, 0, w, this->height()), QColor(0,0,0,255));
        painter.fillRect(QRect(this->width() - w, 0, w, this->height()), QColor(0,0,0,255));
        painter.fillRect(QRect(0, 0, this->width(), h), QColor(0,0,0,255));
        painter.fillRect(QRect(0, this->height() - h, this->width(), h), QColor(0,0,0,255));
    }
}

void AnimationViewerPanel::renderCross(QPainter& painter) const
{
    QPoint centerPoint = getCenterPoint();
    // Background Guide
    int x1 = centerPoint.x();
    int y1 = centerPoint.y();

    painter.setPen(QColor(120, 150, 200));
    painter.drawLine(QPoint(0, y1), QPoint(width(), y1));
    painter.drawLine(QPoint(x1, 0), QPoint(x1, height()));
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

        // render sprite
        // Move rendering position depends on targeting position option
        int dx = 0;
        int dy = 0;

        painter.translate(centerPoint.x() + dx, centerPoint.y() + dy);
        if (glSprite)
        {
            glSprite->render(QPoint(0, 0), painter,AnimationModel::getTargetSprite(), mIsAnimationPlaying, &mEmittedAnimationList);
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
        for (int i = mEmittedAnimationList.count() - 1; i >= 0; i--)
        {
            mEmittedAnimationList[i]->update();
            if (mEmittedAnimationList[i]->isDone())
            {
                mEmittedAnimationList.removeAt(i);
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
        centerPointSprite->render(QPoint(0, 0), painter, AnimationModel::getTargetSprite(), false, &mEmittedAnimationList);
        painter.translate(-centerPointCenterPoint.x(), -centerPointCenterPoint.y());
    }
}

void AnimationViewerPanel::renderTargetSprite(const QPoint& centerPoint, QPainter& painter)
{
    painter.setOpacity(mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mAlpha);

    painter.translate(centerPoint.x(), centerPoint.y());
    mpAnimationModel->getTargetSprite()->render(QPoint(0, 0), painter, NULL, false, &mEmittedAnimationList);
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
        Point2 position;
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

void AnimationViewerPanel::mouseDoubleClickEvent(QMouseEvent *event)
{
    // only for edit mode
    if (mIsAnimationPlaying)
    {
        return;
    }

    this->setFocus();

    // Get center point here
    QPoint centerPoint = getCenterPoint();
    // Calculate pressed position relative from center
    QPoint relativePressedPosition = (QPoint(event->x(), event->y()) - centerPoint)  / mZoom;

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
                if (mpAnimationModel->getKeyFrameIndex(currentPosition.mLineNo, currentPosition.mFrameNo) == -1) {
                    Point2 pt;
                    pt.mX = relativePressedPosition.x();
                    pt.mY = relativePressedPosition.y();

                    mpAnimationModel->setKeyFrame(currentPosition.mLineNo, currentPosition.mFrameNo, pt);
                } else {
                    for (int i = 0; i < mpAnimationModel->getLineCount(); i++) {
                        if (mpAnimationModel->getKeyFrameIndex(i, currentPosition.mFrameNo) == -1)
                        {
                            Point2 pt;
                            pt.mX = relativePressedPosition.x();
                            pt.mY = relativePressedPosition.y();

                            mpAnimationModel->setKeyFrame(i, currentPosition.mFrameNo, pt);
                            break;
                        }
                    }
                }
            break;

            default:
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
    QPoint relativePressedPosition = (QPoint(event->x(), event->y()) - centerPoint)  / mZoom;

    mTargetGrabbed  = false;
    mCelGrabbed = false;

    grabTarget(relativePressedPosition);

    // only for edit mode
    if (mIsAnimationPlaying || mTargetGrabbed)
    {
        return;
    }

    // center point mode
    if (event->modifiers() & Qt::ControlModifier)
    {
        setCenterPoint(event);
        return;
    }

    grabCel(relativePressedPosition);

    if(mCelGrabbed)
    {
        return;
    }
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
                if (mpAnimationModel->getKeyFrameIndex(currentPosition.mLineNo, currentPosition.mFrameNo) > -1)
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
        int centerX = (int)((event->x() - centerPoint.x()) / mZoom - pKeyFrameData->mSpriteDescriptor.mPosition.mX + pKeyFrameData->mSpriteDescriptor.mCenter.mX);
        int centerY = (int)((event->y() - centerPoint.y()) / mZoom - pKeyFrameData->mSpriteDescriptor.mPosition.mY + pKeyFrameData->mSpriteDescriptor.mCenter.mY);

        int w = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.width() / 2;
        int h = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.height() / 2;

        // Snap!
        if (centerX > -w-SNAP_PIXELS && centerX < -w + SNAP_PIXELS) {
            centerX = -w;
        }

        if (centerX > -SNAP_PIXELS && centerX < SNAP_PIXELS) {
            centerX = 0;
        }


        if (centerX  > w -SNAP_PIXELS && centerX < w+SNAP_PIXELS) {
            centerX = w;
        }


        if (centerY > -h-SNAP_PIXELS && centerY < -h+SNAP_PIXELS) {
            centerY = -h;
        }

        if (centerY > -SNAP_PIXELS && centerY < SNAP_PIXELS) {
            centerY = 0;
        }

        if (centerY  > h -SNAP_PIXELS && centerY < h+SNAP_PIXELS) {
            centerY = h;
        }

        mpSelectedCelModel->setCenterX(centerX);// / pKeyFrameData->mSpriteDescriptor.textureCenter().x());
        mpSelectedCelModel->setCenterY(centerY);// / pKeyFrameData->mSpriteDescriptor.textureCenter().y());
    }
}

void AnimationViewerPanel::mouseMoveEvent(QMouseEvent *event)
{
    QPoint centerPoint = getCenterPoint();
    int newPosX = (event->x() - centerPoint.x()) / mZoom + mSelectedOffset.x();
    int newPosY = (event->y() - centerPoint.y()) / mZoom + mSelectedOffset.y();
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
            if (pKeyFrameData->mSpriteDescriptor.mPositionType != PositionType_None)
            {
                newPosX -= (int)mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mX;
                newPosY -= (int)mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mY;
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
