#include "AnimationImagePaletpanel.h"
#include "ResourceManager.h"
#include "GLSprite.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimer>

AnimationImagePaletPanel::AnimationImagePaletPanel(AnimationModel* pAnimationModel)
        : mpAnimationModel(pAnimationModel),
          mClearColor(Qt::black),
          mPressed(false),
          mSnapGridX(0),
          mSnapGridY(0),
          mSnapGridCheck(false),
          mpPlayingAnimationModel(NULL),
          mAnimationFrameNo(0)
{
    mSelectedRect = QRect(0, 0, 96, 96);
    this->setFixedSize(320, 240);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    mpAnimationPlayTimer = new QTimer(this);
    mpAnimationPlayTimer->setInterval(30);
    connect(mpAnimationPlayTimer, SIGNAL(timeout()), this, SLOT(onTick()));
    connect(mpAnimationModel, SIGNAL(selectedPaletChanged(QString)), this, SLOT(onAnimationImagePaletChanged(QString)));
    connect(mpAnimationModel, SIGNAL(animationLoaded()), this, SLOT(reload()));
}

AnimationImagePaletPanel::~AnimationImagePaletPanel()
{
    while (!mGlSpriteList.empty()) { delete mGlSpriteList.takeFirst(); }
    delete mpAnimationPlayTimer;
}

bool AnimationImagePaletPanel::isAnimationExist() const
{
    bool emittedAnimationExists = false;
    for (int lineNo = 0; lineNo < AnimationModel::LINE_COUNT; lineNo++)
    {
        if (!mEmittedAnimationList[lineNo].empty())
        {
            emittedAnimationExists = true;
        }
    }
    return (mAnimationFrameNo < mpPlayingAnimationModel->getMaxFrameCount()) || emittedAnimationExists;
}

void AnimationImagePaletPanel::setSnapGrid(int gridX, int gridY, bool snapGridCheck)
{
    mSnapGridX = gridX;
    mSnapGridY = gridY;
    mSnapGridCheck = snapGridCheck;
}

void AnimationImagePaletPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.fillRect(QRect(0, 0, width() - 1, height() - 1), Qt::SolidPattern);

    switch(mCanvasType)
    {
        case CanvasType_Image:
            const QImage* pImage = ResourceManager::getImage(mpAnimationModel->getSelectedSourcePath());
            if (pImage)
            {
                painter.drawImage(0, 0, *pImage);
            }
            painter.setPen(Qt::white);
            painter.drawRect(mSelectedRect);

            painter.setPen(palette().dark().color());
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
            break;

        case CanvasType_Animation:
            {
                QPoint centerPoint = QPoint((width()) / 2, (height()) / 2);

                // Render sprites
                QList<const GLSprite*>::Iterator iter = mRenderSpriteList.begin();
                while (iter != mRenderSpriteList.end())
                {
                    const GLSprite* glSprite = (GLSprite*)*iter;


                    painter.translate(centerPoint.x(), centerPoint.y());
                    if (glSprite)
                    {
                        glSprite->render(QPoint(0, 0), painter, mpAnimationModel->getTargetSprite(), true, mEmittedAnimationList);
                    }

                    painter.translate(-centerPoint.x(), -centerPoint.y());
                    iter++;
                }

                // update emitted animationlist
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
            break;
        default:
            break;
    }

    painter.end();
}

void AnimationImagePaletPanel::onAnimationImagePaletChanged(QString path)
{
    switch(ResourceManager::getFileType(path))
    {
        case ResourceManager::FileType_Image:
            {
                const QImage* pImage = ResourceManager::getImage(path, true); // recache
                setFixedSize(pImage->width(), pImage->height());

                mCanvasType = CanvasType_Image;
                this->repaint();
                mpAnimationPlayTimer->stop();
            }
            break;

        case ResourceManager::FileType_Animation:
            {
                setFixedSize(parentWidget()->width(), parentWidget()->height());

                mCanvasType = CanvasType_Animation;
                mpPlayingAnimationModel = ResourceManager::getAnimation(path);
                mpPlayingAnimationModel->loadData(ResourceManager::getResourcePath(path));

                mAnimationFrameNo = 0;
                mpAnimationPlayTimer->start();
            }
            break;

        default:
            mCanvasType = CanvasType_None;
            break;
    }
}

void AnimationImagePaletPanel::hideEvent(QHideEvent *event)
{
    resetAnimation();
    mpAnimationPlayTimer->stop();
}

void AnimationImagePaletPanel::reload()
{
    if (mpPlayingAnimationModel)
    {
        mpPlayingAnimationModel = ResourceManager::getAnimation(mpPlayingAnimationModel->getSelectedSourcePath());
        resetAnimation();
    }
}

void AnimationImagePaletPanel::showEvent(QShowEvent *event)
{
    if (mCanvasType == CanvasType_Animation)
    {
        reload();
        mpAnimationPlayTimer->start();
    }
}

QPoint AnimationImagePaletPanel::getSnappedPosition(int x, int y)
{
    int tx = x;
    int ty = y;
    if (mSnapGridCheck)
    {        
        tx = (x/ mSnapGridX) * mSnapGridX;
        ty = (y/ mSnapGridY) * mSnapGridY;
    }

    return QPoint(tx, ty);
}

void AnimationImagePaletPanel::mousePressEvent(QMouseEvent *event)
{
   if (mCanvasType == CanvasType_Image)
   {
        mPressed = true;
        QPoint pos = getSnappedPosition(event->x(), event->y());

        mSelectedRect.setTopLeft(pos);
        if (mSnapGridCheck)
        {
            pos += QPoint(mSnapGridX, mSnapGridY);
        }
        mSelectedRect.setBottomRight(pos - QPoint(1, 1));
        repaint();
    }
}

void AnimationImagePaletPanel::mouseMoveEvent(QMouseEvent *event)
{
   if (mCanvasType == CanvasType_Image)
   {
        if (mPressed)
        {
            QPoint pos = getSnappedPosition(event->x(), event->y());
            if (mSnapGridCheck)
            {
                if (pos.x() > 0)
                {
                    pos.setX(pos.x() + mSnapGridX);
                }
                if (pos.y() > 0)
                {
                    pos.setY(pos.y() + mSnapGridY);
                }

                QPoint dPoint = pos - mSelectedRect.topLeft();
                if (dPoint.x() == 0)
                {
                    int t = event->x() - mSelectedRect.topLeft().x();
                    pos.setX(pos.x() + (t >= 0 ? mSnapGridX : -mSnapGridX));
                }
                if (dPoint.y() == 0)
                {
                    int t = event->y() - mSelectedRect.topLeft().y();
                    pos.setY(pos.y() + (t >= 0 ? mSnapGridY : -mSnapGridY));
                }
            }
            mSelectedRect.setBottomRight(pos - QPoint(1, 1));
            repaint();
        }
    }
}

void AnimationImagePaletPanel::mouseReleaseEvent(QMouseEvent *event)
{
    if (mCanvasType == CanvasType_Image)
    {
        mpAnimationModel->mSelectedPaletTextureSrcRect = mSelectedRect;
        mPressed = false;
    }
}

void AnimationImagePaletPanel::resetAnimation()
{
    mRenderSpriteList.clear();
    mAnimationFrameNo = 0;
    for (int lineNo = 0; lineNo < AnimationModel::LINE_COUNT; lineNo++)
    {
        while (!mEmittedAnimationList[lineNo].empty()) { delete mEmittedAnimationList[lineNo].takeFirst();}
    }
}

void AnimationImagePaletPanel::onTick()
{
    // Don't play an animation without any frames
    if (mpPlayingAnimationModel)
        {
        if (mpPlayingAnimationModel->getMaxFrameCount() > 0)
        {
            // goto next frame
            mAnimationFrameNo++;

            // Loop animation
            if(!isAnimationExist())
            {
                resetAnimation();
            }

            // Delete previous generated sprites
            while (!mGlSpriteList.empty()) { delete mGlSpriteList.takeFirst(); }
            mRenderSpriteList.clear();

            // Set current glsprite list
            mGlSpriteList = mpPlayingAnimationModel->createGLSpriteListAt(NULL, mAnimationFrameNo);

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
    }
}
