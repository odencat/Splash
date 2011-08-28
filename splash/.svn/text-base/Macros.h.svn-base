#ifndef MACROS_H
#define MACROS_H

#define PI 3.14159265358979323846
#define LERP(startValue, endValue, frameNo, startFrameNo, endFrameNo)\
startValue + ((endValue - startValue) * (frameNo - startFrameNo)) / (endFrameNo - startFrameNo)
//#define LERP(l, h, a)  l + (h - l) * a

#define CLIP(x, a, b) x > b ? b : (x < a ? a : x)


static int randInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

static float randFloat(float a, float b)
{
    // Random number between low and high
    if (a > b)
    {
        return (qrand() / RAND_MAX) * (a - b) + b;
    }
    else
    {
        return (qrand() / RAND_MAX) * (b - a) + a;
    }
}

#endif // MACROS_H
