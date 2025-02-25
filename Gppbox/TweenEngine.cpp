#include "TweenEngine.h"

#include <cmath>

/*template <typename T>
T TweenEngine<T>::Lerp(T min, T max, float t)
{
    return min + (max - min) * t;
}


template <typename T>
T TweenEngine<T>::EaseInQuart(T min, T max, float t)
{
    T x = Lerp(min, max, t);
    
    return x * x * x * x; 
}


template <typename T>
T TweenEngine<T>::EaseInOutBack(T min, T max, float t)
{
    const double c1 = 1.70158;
    const double c2 = c1 * 1.525;

    T x = Lerp(min, max, t);

    if(t > 0.5f)
        return pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2) / 2;

    return (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
} */