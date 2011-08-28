#ifndef CelModel_H
#define CelModel_H
#include <QObject>
#include "GLSprite.h"

class KeyFrameData;
class CelModel : public QObject
{
Q_OBJECT

public:
    KeyFrameData* getKeyFrameDataReference() const;
    void setKeyFrameDataReference(KeyFrameData* pKeyFrameData);

    CelModel();

public slots:
    void setSourceTexture(QString path, QRect rect);
    void setTextureSrcRect(QRect value);

    void setEmitter(bool isEmitter);

    void setMinEmitSpeed(double value);
    void setMaxEmitSpeed(double value);
    void setMinEmitAngle(int value);
    void setMaxEmitAngle(int value);

    void setBlendType(int index);
    void setPositionType(int value);
    void setPositionTypeOption(int value);

    void setFacingOptionType(int value);
    void setBlur(int value);

    void setCenterX(int value);
    void setCenterY(int value);

    void setPositionX(int value);
    void setPositionY(int value);
    void setPositionTweenType(int index);

    void setRotation(int value);
    void setRotationTweenType(int index);

    void setScaleX(double value);
    void setScaleY(double value);
    void setScaleTweenType(int index);

    void setAlpha(double value);
    void setAlphaTweenType(int index);

    void setColorR(double value);
    void setColorG(double value);
    void setColorB(double value);
    void setColorTweenType(int index);

    void setPriority(double value);

    void setSwapTargetType(int value);
    void setHideActor(bool value);

signals:
    void sourceTextureChanged(QString path, QRect rect);
    void textureSrcRectChanged(QRect value);
    void emitterChanged(bool isEmitter);
    void minEmitSpeedChanged(double value);
    void maxEmitSpeedChanged(double value);
    void minEmitAngleChanged(int value);
    void maxEmitAngleChanged(int value);

    void blendTypeChanged(int value);
    void positionTypeChanged(int value);
    void positionTypeOptionChanged(int value);

    void facingOptionTypeChanged(int value);
    void blurChanged(int value);

    void centerXChanged(int value);
    void centerYChanged(int value);

    void positionXChanged(int value);
    void positionYChanged(int value);

    void positionTweenTypeChanged(int value);

    void rotationChanged(int value);
    void rotationTweenTypeChanged(int index);

    void scaleXChanged(double value);
    void scaleYChanged(double value);
    void scaleTweenTypeChanged(float index);

    void alphaChanged(double value);
    void alphaTweenTypeChanged(float index);

    void colorRChanged(double value);
    void colorGChanged(double value);
    void colorBChanged(double value);
    void colorTweenTypeChanged(int value);

    void priorityChanged(double value);

    void swapTargetTypeChanged(int value);
    void hideActorChanged(bool value);
private:
    KeyFrameData* mpKeyFrameData;
};
#endif // CelModel_H
