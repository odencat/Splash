#ifndef KEYFRAME_H
#define KEYFRAME_H

class KeyFrameData;
class KeyFrame
{
public:
     enum KeyFrameType
     {
         KeyFrameType_empty,
         KeyFrameType_fix,
         KeyFrameType_tween,

         KeyFrameType_COUNT
     };

    struct KeyFramePosition
    {
        int mLineNo;
        int mFrameNo;

        KeyFramePosition()
        {
            mLineNo = 0;
            mFrameNo = 0;
        }

        KeyFramePosition(int lineNo, int frameNo)
        {
            mLineNo = lineNo;
            mFrameNo = frameNo;
        }

        bool operator==(const KeyFramePosition& item)
        {
            return mFrameNo == item.mFrameNo && mLineNo == item.mLineNo;
        }

        bool operator!=(const KeyFramePosition& item)
        {
            return mFrameNo != item.mFrameNo || mLineNo != item.mLineNo;
        }
    };

    KeyFrame(int lineNo, int frameNo, KeyFrameData* keyFrameData);
    ~KeyFrame();

    int mLineNo;
    int mFrameNo;

    KeyFrameData* mpKeyFrameData;

    KeyFrameType getKeyFrameType() const;
};


#endif // KEYFRAME_H
