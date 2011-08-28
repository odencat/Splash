#include "AnimationViewer.h"
#include "ui_AnimationViewer.h"
#include "AnimationViewer/AnimationViewerPanel.h"
#include "DataModels/AnimationModel.h"
#include "DataModels/CelModel.h"
#include <QTimer>
#include <QStandardItemModel>

AnimationViewer::AnimationViewer(QWidget* parent, AnimationModel* animationModel)
   :QWidget(parent),
    m_ui(new Ui::AnimationViewer),
    mpSelectedCelModel(new CelModel()),
    mpAnimationModel(animationModel),
    mLoop(false)
{
    mSelectedKeyFramePosition.mFrameNo = -1;
    mSelectedKeyFramePosition.mLineNo = -1;

    m_ui->setupUi(this);

    mpTweenModel = new QStandardItemModel();
    // Set Tween types
    mpTweenModel->appendRow(new QStandardItem(QString("None")));
    mpTweenModel->appendRow(new QStandardItem(QString("Fix")));
    mpTweenModel->appendRow(new QStandardItem(QString("Tween")));

    m_ui->alphaTweenTypeComboBox->setModel(mpTweenModel);
    m_ui->positionTweenTypeComboBox->setModel(mpTweenModel);
    m_ui->rotationTweenTypeComboBox->setModel(mpTweenModel);
    m_ui->scaleTweenTypeComboBox->setModel(mpTweenModel);
    m_ui->colorTweenTypeComboBox->setModel(mpTweenModel);

    // Set Position types
    mpPositionTypeModel = new QStandardItemModel();
    mpPositionTypeModel->appendRow(new QStandardItem(QString("none")));
    mpPositionTypeModel->appendRow(new QStandardItem(QString("RelToTarget")));
    mpPositionTypeModel->appendRow(new QStandardItem(QString("RelToTargetOrigin")));
    m_ui->positionTypeComboBox->setModel(mpPositionTypeModel);

    mpAnimationPlayTimer = new QTimer(this);
    mpAnimationPlayTimer->setInterval(30);

    m_ui->gridCelDataBox->setEnabled(false);

    // Add animation viewer (OpenGL Screen)
    mpAnimationViewerPanel = new AnimationViewerPanel(this, mpAnimationModel, mpSelectedCelModel);
    mpAnimationViewerPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mpAnimationViewerPanel->setMinimumWidth(480);
    mpAnimationViewerPanel->setMinimumHeight(320);
    m_ui->animationViewerContainer->addWidget(mpAnimationViewerPanel);

    // connect Cel model and controls
    connect(m_ui->alphaSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setAlpha(double)));
    connect(m_ui->alphaTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setAlphaTweenType(int)));
    connect(m_ui->alphaTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTweenTypeChanged(int)));

    connect(m_ui->prioritySpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setPriority(double)));

    connect(m_ui->positionXSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setPositionX(int)));
    connect(m_ui->positionYSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setPositionY(int)));
    connect(m_ui->positionTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setPositionTweenType(int)));
    connect(m_ui->positionTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTweenTypeChanged(int)));

    connect(m_ui->scaleXSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setScaleX(double)));
    connect(m_ui->scaleYSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setScaleY(double)));
    connect(m_ui->scaleTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setScaleTweenType(int)));
    connect(m_ui->scaleTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTweenTypeChanged(int)));

    connect(m_ui->rotationXSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setRotation(int)));
    connect(m_ui->rotationTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setRotationTweenType(int)));
    connect(m_ui->rotationTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTweenTypeChanged(int)));

    connect(m_ui->colorSpinBoxR, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setColorR(double)));
    connect(m_ui->colorSpinBoxG, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setColorG(double)));
    connect(m_ui->colorSpinBoxB, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setColorB(double)));

    connect(m_ui->colorTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setColorTweenType(int)));
    connect(m_ui->colorTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTweenTypeChanged(int)));

    connect(m_ui->blendTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setBlendType(int)));
    connect(m_ui->facingOptionCombobox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setFacingOptionType(int)));
    connect(m_ui->positionTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setPositionType(int)));
    connect(m_ui->positionTypeOptionComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setPositionTypeOption(int)));

    connect(m_ui->emitterCheckBox, SIGNAL(toggled(bool)), mpSelectedCelModel, SLOT(setEmitter(bool)));
    connect(m_ui->minEmitSpeedSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setMinEmitSpeed(double)));
    connect(m_ui->maxEmitSpeedSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setMaxEmitSpeed(double)));
    connect(m_ui->minEmitAngleSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setMinEmitAngle(int)));
    connect(m_ui->maxEmitAngleSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setMaxEmitAngle(int)));

    connect(m_ui->blurSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setBlur(int)));

    connect(m_ui->centerXSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setCenterX(int)));
    connect(m_ui->centerYSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setCenterY(int)));

    // Special
    connect(m_ui->swapTargetComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setSwapTargetType(int)));
    connect(m_ui->hideActorCheckbox, SIGNAL(toggled(bool)), mpSelectedCelModel, SLOT(setHideActor(bool)));

    // Selected Model changed
    connect(mpSelectedCelModel, SIGNAL(alphaChanged(double)), m_ui->alphaSpinBox, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(positionXChanged(int)), m_ui->positionXSpinBox, SLOT(setValue(int)));
    connect(mpSelectedCelModel, SIGNAL(positionYChanged(int)), m_ui->positionYSpinBox, SLOT(setValue(int)));
    connect(mpSelectedCelModel, SIGNAL(priorityChanged(double)), m_ui->prioritySpinBox, SLOT(setValue(double)));

    connect(mpSelectedCelModel, SIGNAL(scaleXChanged(double)), m_ui->scaleXSpinBox, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(scaleYChanged(double)), m_ui->scaleYSpinBox, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(rotationChanged(int)), m_ui->rotationXSpinBox, SLOT(setValue(int)));

    connect(mpSelectedCelModel, SIGNAL(colorRChanged(double)), m_ui->colorSpinBoxR, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(colorGChanged(double)), m_ui->colorSpinBoxG, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(colorBChanged(double)), m_ui->colorSpinBoxB, SLOT(setValue(double)));

    connect(mpSelectedCelModel, SIGNAL(centerXChanged(int)), m_ui->centerXSpinBox, SLOT(setValue(int)));
    connect(mpSelectedCelModel, SIGNAL(centerYChanged(int)), m_ui->centerYSpinBox, SLOT(setValue(int)));

    connect(mpSelectedCelModel, SIGNAL(swapTargetTypeChanged(int)), m_ui->swapTargetComboBox, SLOT(setCurrentIndex(int)));

    // Refresh the screen if KeyFrameData changed
    connect(m_ui->prioritySpinBox, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->positionXSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->positionYSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->alphaSpinBox, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->scaleXSpinBox, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->scaleYSpinBox, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->rotationXSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->colorSpinBoxR, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->colorSpinBoxG, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->colorSpinBoxB, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));

    connect(m_ui->blendTypeComboBox, SIGNAL(currentIndexChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->facingOptionCombobox, SIGNAL(currentIndexChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->positionTypeComboBox, SIGNAL(currentIndexChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->positionTypeOptionComboBox, SIGNAL(currentIndexChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->emitterCheckBox, SIGNAL(toggled(bool)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->hideActorCheckbox, SIGNAL(toggled(bool)), mpAnimationViewerPanel, SLOT(refresh()));

    connect(m_ui->blurSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->centerXSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->centerYSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->showAnimationUICheckBox, SIGNAL(toggled(bool)), mpAnimationViewerPanel, SLOT(setShowAnimationUI(bool)));
    connect(m_ui->showCameraCheckbox, SIGNAL(toggled(bool)), mpAnimationViewerPanel, SLOT(setShowCamera(bool)));
    connect(m_ui->showTargetCheckbox, SIGNAL(toggled(bool)), mpAnimationViewerPanel, SLOT(setShowTarget(bool)));

    connect(m_ui->loopPlayCheckbox, SIGNAL(toggled(bool)), this, SLOT(setLoopPlay(bool)));

    // Cel selection
    connect(mpAnimationViewerPanel, SIGNAL(celSelected(KeyFrameData*)), this, SLOT(onCelSelected(KeyFrameData*)));

    // connect save button
    connect(m_ui->saveAnimationButton, SIGNAL(clicked()), this, SLOT(onSaveAnimationButtonClicked()));
    connect(m_ui->saveAnimationButton, SIGNAL(clicked()), this, SLOT(stopAnimation()));

    // connect palet button
    connect(m_ui->showPaletteButton, SIGNAL(clicked()), this, SLOT(stopAnimation()));

    // connect Timer
    connect(m_ui->playAnimationButton, SIGNAL(clicked()), this, SLOT(onPlayButtonClicked()));
    connect(mpAnimationPlayTimer, SIGNAL(timeout()), this, SLOT(onTick()));
}

AnimationViewer::~AnimationViewer()
{
    delete mpPositionTypeModel;
    delete mpTweenModel;
    mpAnimationPlayTimer->stop();
    delete mpAnimationPlayTimer;
    delete m_ui;
    delete mpSelectedCelModel;
}

void AnimationViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AnimationViewer::setLoopPlay(bool loop)
{
    mLoop = loop;
}

void AnimationViewer::blockSignals(bool block)
{

    m_ui->alphaSpinBox->blockSignals(block);
    m_ui->prioritySpinBox->blockSignals(block);
    m_ui->positionXSpinBox->blockSignals(block);
    m_ui->positionYSpinBox->blockSignals(block);
    m_ui->scaleXSpinBox->blockSignals(block);
    m_ui->scaleYSpinBox->blockSignals(block);
    m_ui->rotationXSpinBox->blockSignals(block);
    m_ui->blurSpinBox->blockSignals(block);
    m_ui->positionTypeComboBox->blockSignals(block);
    m_ui->positionTypeOptionComboBox->blockSignals(block);
    m_ui->emitterCheckBox->blockSignals(block);
    m_ui->hideActorCheckbox->blockSignals(block);
    m_ui->facingOptionCombobox->blockSignals(block);
    m_ui->blendTypeComboBox->blockSignals(block);
    m_ui->centerXSpinBox->blockSignals(block);
    m_ui->centerYSpinBox->blockSignals(block);
    m_ui->colorSpinBoxR->blockSignals(block);
    m_ui->colorSpinBoxG->blockSignals(block);
    m_ui->colorSpinBoxB->blockSignals(block);
    m_ui->swapTargetComboBox->blockSignals(block);

    m_ui->minEmitSpeedSpinBox->blockSignals(block);
    m_ui->maxEmitSpeedSpinBox->blockSignals(block);
    m_ui->minEmitAngleSpinBox->blockSignals(block);
    m_ui->maxEmitAngleSpinBox->blockSignals(block);
}

// Cel selected
void AnimationViewer::onCelSelected(KeyFrameData* pKeyFrameData)
{
    if (pKeyFrameData)
    {
        m_ui->gridCelDataBox->setEnabled(true);
        blockSignals(true);
        m_ui->alphaSpinBox->setValue(pKeyFrameData->mSpriteDescriptor.mAlpha);
        m_ui->alphaTweenTypeComboBox->setCurrentIndex(pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_alpha]);

        m_ui->prioritySpinBox->setValue(pKeyFrameData->mSpriteDescriptor.mPriority);
        m_ui->positionXSpinBox->setValue((int)pKeyFrameData->mSpriteDescriptor.mPosition.mX);
        m_ui->positionYSpinBox->setValue((int)pKeyFrameData->mSpriteDescriptor.mPosition.mY);
        m_ui->positionTweenTypeComboBox->setCurrentIndex(pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_position]);
        m_ui->scaleXSpinBox->setValue((double)pKeyFrameData->mSpriteDescriptor.mScale.mX);
        m_ui->scaleYSpinBox->setValue((double)pKeyFrameData->mSpriteDescriptor.mScale.mY);

        m_ui->colorTweenTypeComboBox->setCurrentIndex(pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_color]);
        m_ui->colorSpinBoxR->setValue((double)pKeyFrameData->mSpriteDescriptor.mColor.mR);
        m_ui->colorSpinBoxG->setValue((double)pKeyFrameData->mSpriteDescriptor.mColor.mG);
        m_ui->colorSpinBoxB->setValue((double)pKeyFrameData->mSpriteDescriptor.mColor.mB);

        m_ui->scaleTweenTypeComboBox->setCurrentIndex(pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_scale]);
        m_ui->rotationXSpinBox->setValue((int)pKeyFrameData->mSpriteDescriptor.mRotation);
        m_ui->rotationTweenTypeComboBox->setCurrentIndex(pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_rotation]);
        m_ui->blurSpinBox->setValue(pKeyFrameData->mSpriteDescriptor.mBlur);
        m_ui->positionTypeComboBox->setCurrentIndex(pKeyFrameData->mSpriteDescriptor.mPositionType);
        m_ui->positionTypeOptionComboBox->setCurrentIndex(pKeyFrameData->mSpriteDescriptor.mPositionTypeOption);

        m_ui->facingOptionCombobox->setCurrentIndex(pKeyFrameData->mSpriteDescriptor.mFacingOptionType);
        m_ui->blendTypeComboBox->setCurrentIndex(pKeyFrameData->mSpriteDescriptor.mBlendType);


        m_ui->minEmitSpeedSpinBox->setValue((double)pKeyFrameData->mSpriteDescriptor.mMinEmitSpeed);
        m_ui->maxEmitSpeedSpinBox->setValue((double)pKeyFrameData->mSpriteDescriptor.mMaxEmitSpeed);
        m_ui->minEmitAngleSpinBox->setValue(pKeyFrameData->mSpriteDescriptor.mMinEmitAngle);
        m_ui->maxEmitAngleSpinBox->setValue(pKeyFrameData->mSpriteDescriptor.mMaxEmitAngle);

        m_ui->hideActorCheckbox->setChecked(pKeyFrameData->mHideActor);

        bool positionTypeOptionEnabled = m_ui->positionTypeComboBox->currentIndex() != 0;
        m_ui->positionTypeOptionComboBox->setEnabled(positionTypeOptionEnabled);

        bool specialCelSelected = (mpAnimationModel->getCurrentKeyFramePosition().mLineNo == AnimationModel::LINE_target || mpAnimationModel->getCurrentKeyFramePosition().mLineNo == AnimationModel::LINE_camera);

        m_ui->blurSpinBox->setEnabled(!specialCelSelected);
        m_ui->facingOptionCombobox->setEnabled(!specialCelSelected);
        m_ui->positionTypeOptionComboBox->setEnabled(!specialCelSelected);

        specialCelSelected = (mpAnimationModel->getCurrentKeyFramePosition().mLineNo == AnimationModel::LINE_target);
        m_ui->positionTypeComboBox->setEnabled(!specialCelSelected);

        if (pKeyFrameData->mSpriteDescriptor.isImage())
        {
            // Check box
            m_ui->centerXSpinBox->setEnabled(true);
            m_ui->centerYSpinBox->setEnabled(true);
            m_ui->centerXSpinBox->setValue((int)pKeyFrameData->mSpriteDescriptor.mCenter.mX);
            m_ui->centerYSpinBox->setValue((int)pKeyFrameData->mSpriteDescriptor.mCenter.mY);

            // special
            m_ui->swapTargetComboBox->setEnabled(true);
            m_ui->swapTargetComboBox->setCurrentIndex((int)pKeyFrameData->mSwapTargetType);

            // Emitter
            m_ui->emitterCheckBox->setEnabled(false);
            m_ui->emitterCheckBox->setChecked(false);
            m_ui->emitLayout->setEnabled(false);
        }
        else
        {
            // Check box
            m_ui->centerXSpinBox->setEnabled(false);
            m_ui->centerYSpinBox->setEnabled(false);
            m_ui->centerXSpinBox->setValue(0);
            m_ui->centerYSpinBox->setValue(0);

            // Special
            m_ui->swapTargetComboBox->setEnabled(false);
            m_ui->swapTargetComboBox->setCurrentIndex(0);

            // Emitter
            m_ui->emitterCheckBox->setEnabled(true);
            m_ui->emitterCheckBox->setChecked(pKeyFrameData->mSpriteDescriptor.mEmitter);
            m_ui->emitLayout->setEnabled(true);
        }
        blockSignals(false);
    }
    else
    {
        m_ui->gridCelDataBox->setEnabled(false);
    }
}

void AnimationViewer::onTweenTypeChanged(int tweenType)
{
    bool alphaTweenEnabled = (m_ui->alphaTweenTypeComboBox->currentIndex() != 0);
    m_ui->alphaSpinBox->setEnabled(alphaTweenEnabled);

    bool scaleTweenEnabled = (m_ui->scaleTweenTypeComboBox->currentIndex() != 0);
    m_ui->scaleXSpinBox->setEnabled(scaleTweenEnabled);
    m_ui->scaleYSpinBox->setEnabled(scaleTweenEnabled);

    bool positionTweenEnabled = (m_ui->positionTweenTypeComboBox->currentIndex() != 0);
    m_ui->positionXSpinBox->setEnabled(positionTweenEnabled);
    m_ui->positionYSpinBox->setEnabled(positionTweenEnabled);

    bool rotationTweenEnabled = (m_ui->rotationTweenTypeComboBox->currentIndex() != 0);
    m_ui->rotationXSpinBox->setEnabled(rotationTweenEnabled);

    bool colorTweenEnabled = (m_ui->colorTweenTypeComboBox->currentIndex() != 0);
    m_ui->colorSpinBoxR->setEnabled(colorTweenEnabled);
    m_ui->colorSpinBoxG->setEnabled(colorTweenEnabled);
    m_ui->colorSpinBoxB->setEnabled(colorTweenEnabled);

    mpAnimationModel->tellTimeLineToRefresh();
}

void AnimationViewer::onPlayButtonClicked()
{
    if (mpAnimationPlayTimer->isActive())
    {
        stopAnimation();
    }
    else
    {
        mpAnimationPlayTimer->start();
        mpAnimationModel->getCurrentKeyFramePosition();
        mSelectedKeyFramePosition = mpAnimationModel->getCurrentKeyFramePosition();
        mpAnimationModel->selectCurrentKeyFramePosition(mSelectedKeyFramePosition.mLineNo, 0);
        blockSignals(true);
        emit playAnimation(false);
        mpAnimationViewerPanel->playAnimation();
        mpAnimationViewerPanel->refresh();
    }
}

void AnimationViewer::stopAnimation()
{
    if (mpAnimationViewerPanel->isAnimationPlaying())
        {
        mpAnimationViewerPanel->stopAnimation();
        mpAnimationPlayTimer->stop();
        emit playAnimation(true);
        blockSignals(false);
        mpAnimationModel->selectCurrentKeyFramePosition(mSelectedKeyFramePosition.mLineNo, mSelectedKeyFramePosition.mFrameNo);
    }
}

void AnimationViewer::onTick()
{
    if (mpAnimationViewerPanel->isAnimationExist())
    {
        // move to next frame
        mpAnimationViewerPanel->gotoNextFrame();
    }
    else
    {
        stopAnimation();
    }

    if (!mpAnimationViewerPanel->isAnimationExist() && mLoop)
    {
        mpAnimationModel->selectCurrentKeyFramePosition(mSelectedKeyFramePosition.mLineNo, 0);
    }
}


// Save Button
void AnimationViewer::onSaveAnimationButtonClicked()
{
    mpAnimationModel->saveData();
}
