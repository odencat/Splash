#ifndef ANIMATIONIMAGEPALETPANEL_H
#define ANIMATIONIMAGEPALETPANEL_H

#include <QGLWidget>
#include <QPixmap>
#include <QList>
#include "DataModels/AnimationModel.h"

class GLSprite;
class QPaintEvent;
class QPixmap;
class QTimer;
class EmittedAnimation;
class AnimationImagePaletPanel : public QGLWidget
{
    Q_OBJECT

public:

    enum CanvasType
    {
        CanvasType_None,
        CanvasType_Image,
        CanvasType_Animation
    };

    AnimationImagePaletPanel(AnimationModel* pAnimationModel);
    ~AnimationImagePaletPanel();

    void setSnapGrid(int gridX, int gridY, bool snapGridCheck);
    
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent  *event);

private slots:
    void onAnimationImagePaletChanged(QString id);
    void onTick();
    void reload();

private:
    QPoint getSnappedPosition(int x, int y);
    void resetAnimation();
    bool isAnimationExist() const;

    QRect mSelectedRect;
    AnimationModel* mpAnimationModel;
    QColor mClearColor;
    bool mPressed;
    int mSnapGridX;
    int mSnapGridY;
    bool mSnapGridCheck;
    CanvasType mCanvasType;

    QList<const GLSprite*> mGlSpriteList;
    QList<const GLSprite*> mRenderSpriteList;
    AnimationModel* mpPlayingAnimationModel;
    QTimer* mpAnimationPlayTimer;
    int mAnimationFrameNo;

    QList<EmittedAnimation*> mEmittedAnimationList[AnimationModel::LINE_COUNT];
};

#endif // ANIMATIONIMAGEPALETPANEL_H
