#ifndef AnimationViewerPanel_H
#define AnimationViewerPanel_H

#include <QGLWidget>
#include "DataModels/AnimationModel.h"
#include "EmittedAnimation.h"

class CelModel;
class GLSprite;
class KeyFrame;
class KeyFrameData;
class AnimationViewerPanel : public QGLWidget
{
Q_OBJECT
public:
    AnimationViewerPanel(QWidget* parent, AnimationModel* pAnimationModel, CelModel* const pSelectedCelModel);
    ~AnimationViewerPanel();

    void selectCel(int celNo);
    void playAnimation();
    bool isAnimationPlaying() const;
    bool isAnimationExist() const;
    void stopAnimation();
    void gotoNextFrame();

protected:
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void refresh();
    void setShowAnimationUI(bool showUI);
    void setShowTarget(bool value);
    void setShowCamera(bool value);

signals:
    void celSelected(KeyFrameData* pKeyFrameData);

private slots:
    void addNewCel(QPoint& relativePressedPosition);
    bool grabTarget(QPoint& relativePressedPosition);
    void grabCel(QPoint& relativePressedPosition);
    void timerEvent();

private:
    QPoint getCenterPoint() const;
    void setCenterPoint(QMouseEvent *event);
    void renderCross(QPainter& painter);
    void renderCelSprites(const QPoint& centerPoint, QPainter& painter);
    void renderCelBox(QPainter& painter, const GLSprite* glSprite, QPoint spriteRenderPoint);
    void renderCenterPointSprite(const GLSprite* pGlSprite, const QPoint& centerPoint, QPainter& painter);
    void renderTargetSprite(const QPoint& centerPoint, QPainter& painter);
    void clearSprites();
    void swapSourceTexture();

    QList<const GLSprite*> mGlSpriteList;
    QList<const GLSprite*> mRenderSpriteList;
    AnimationModel* mpAnimationModel;
    int mCurrentFrameNo;
    QColor mClearColor;

    QPoint mSelectedOffset;
    CelModel* const mpSelectedCelModel;
    QTimer* mpTimer;
    bool mIsAnimationPlaying;
    bool mCelGrabbed;
    bool mTargetGrabbed;
    QList<EmittedAnimation*> mEmittedAnimationList[AnimationModel::LINE_COUNT];
    bool mShowAnimationUI;
    bool mShowTarget;
    bool mShowCamera;
};

#endif // AnimationViewerPanel_H
