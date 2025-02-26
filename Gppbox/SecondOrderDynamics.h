#pragma once
#include <SFML/System/Vector3.hpp>

using namespace sf;

class SecondOrderDynamics
{
private:
    //Second order dynamics
    Vector3<float> xp;
    Vector3<float> y, yd;
    float _w, _z, _d, k1, k2, k3;
    
    float shakeDuration = 0.0f;
    float shakeTimer = 0.0f;
    float shakeFrequency = 0.0f;
    float shakeAmplitude = 0.0f;
    
public:
    SecondOrderDynamics(float f, float z, float r, Vector3<float> x0);

    void Reset(float f, float z, float r, Vector3<float> x0);

    void StartShake(float duration, float frequency, float amplitude);
    void StopShake();

    Vector3<float> Calculate(float T, Vector3<float> x, Vector3<float> xd);
};
