#include "TweenEngine.h"

#include <cmath>


float TweenEngine::Lerp(float min, float max, float t)
{
    return min + (max - min) * t;
}


float TweenEngine::EaseInQuart(float min, float max, float t)
{
    double x = t * t * t;
    
    return Lerp(min, max, x); 
}


int TweenEngine::EaseInOutBack(int min, int max, float t)
{
    const double c1 = 1.70158;
    const double c2 = c1 * 1.525;
    
    int x = Lerp(min, max, t);

    if(t > 0.5f)
        return pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2) / 2;

    return (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}