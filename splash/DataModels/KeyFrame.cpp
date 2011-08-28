#include "KeyFrame.h"
#include "KeyFrameData.h"

KeyFrame::KeyFrame(int lineNo, int frameNo, KeyFrameData* keyFrameData)
:   mLineNo(lineNo),
    mFrameNo(frameNo),
    mpKeyFrameData(keyFrameData)
{
}

KeyFrame::~KeyFrame()
{
    delete mpKeyFrameData;
}

KeyFrame::KeyFrameType KeyFrame::getKeyFrameType() const
{
    KeyFrameType keyframeType;
    if (mpKeyFrameData)
    {
        if (mpKeyFrameData->hasTween())
        {
            keyframeType = KeyFrameType_tween;
        }
        else
        {
            keyframeType = KeyFrameType_fix;
        }
    }
    else
    {
        keyframeType = KeyFrameType_empty;
    }
    return keyframeType;
}
