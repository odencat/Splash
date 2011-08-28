#ifndef ANIMATIONVIEWER_H
#define ANIMATIONVIEWER_H

#include <QtGui/QWidget>
#include "DataModels/KeyFrameData.h"
#include "DataModels/KeyFrame.h"
namespace Ui
{
    class AnimationViewer;
}

class AnimationViewerPanel;
class AnimationModel;
class QStandardItemModel;
class CelModel;
class AnimationViewer : public QWidget
{
Q_OBJECT
public:
    AnimationViewer(QWidget* parent, AnimationModel* animationModel);
    ~AnimationViewer();

    AnimationViewerPanel* mpAnimationViewerPanel;

    Ui::AnimationViewer *m_ui;

public slots:
    void stopAnimation();

private slots:
    void onCelSelected(KeyFrameData* pKeyFrameData);

    void setLoopPlay(bool loop);
    void onSaveAnimationButtonClicked();

    void onTweenTypeChanged(int tweenType);
    void onPlayButtonClicked();

    void onTick();

signals:
    void playAnimation(bool stop);
    
protected:
    void changeEvent(QEvent *e);

private:

    void blockSignals(bool block);

    CelModel* mpSelectedCelModel;
    QStandardItemModel* mpPositionTypeModel;
    QStandardItemModel* mpTweenModel;

    AnimationModel* mpAnimationModel;
    QTimer* mpAnimationPlayTimer;
    
    KeyFrame::KeyFramePosition mSelectedKeyFramePosition;

    bool mLoop;
};

#endif // ANIMATIONVIEWER_H
