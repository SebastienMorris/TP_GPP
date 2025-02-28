#pragma once

class TweenEngine
{
    
public:
    static float Lerp(float min, float max, float t);
    
    static float EaseInQuart(float min, float max, float t);

    static int EaseInOutBack(int min, int max, float t);
};
