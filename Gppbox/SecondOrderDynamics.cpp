#include "SecondOrderDynamics.h"

#include <complex>

#include "Dice.hpp"
#include "Lib.hpp"

using namespace Lib;

SecondOrderDynamics::SecondOrderDynamics(float f, float z, float r, Vector3<float> x0)
{
    _w = 2 * pi() * f;
    _z = z;
    _d = _w * sqrt(abs(z * z - 1));

    k1 = z / (pi() * f);
    k2 = 1 / (_w * _w);
    k3 = r * z / _w;

    xp = x0;
    y = x0;
    yd = {0, 0, 0};
}

void SecondOrderDynamics::Reset(float f, float z, float r, Vector3<float> x0)
{
    _w = 2 * pi() * f;
    _z = z;
    _d = _w * std::sqrt(std::abs(z * z - 1));

    k1 = z / (pi() * f);
    k2 = 1 / (_w * _w);
    k3 = r * z / _w;

    xp = x0;
    y = x0;
    yd = {0, 0, 0};
}

void SecondOrderDynamics::StartShake(float duration, float frequency, float amplitude)
{
    shakeFrequency = frequency;
    shakeAmplitude = amplitude;
    shakeDuration = duration;
    shakeTimer = 0.0f;
}


void SecondOrderDynamics::StopShake()
{
    shakeDuration = 0.0f;
    shakeFrequency = 0.0f;
    shakeAmplitude = 0.0f;
    shakeTimer = 0.0f;
}



Vector3<float> SecondOrderDynamics::Calculate(float T, Vector3<float> x, Vector3<float> xd)
{
    float k2_stable = std::max({k2, T*T/2 + T*k1/2, T*k1});
    //float k2_stable = max(k2, T*T/2 + T*k1/2);
    y += T * yd;
    yd += T * (x + k3*xd - y - k1*yd) / k2_stable;
    
    if (shakeDuration > 0.0f) {
        
        shakeTimer += T;
        float progress = shakeTimer / shakeDuration;

        // Génération d'un offset aléatoire
        float angle = static_cast<float>(std::rand()) / RAND_MAX * 2 * Dice::getPi();
        float magnitude = shakeAmplitude * (1.0f - progress); // Amortissement du shake
        Vector3<float> offset = Vector3<float>(std::cos(angle), std::sin(angle), 0) * magnitude;

        if (shakeTimer >= shakeDuration) {
            StopShake(); // stop shake
        }
        return y + offset;  // Position avec shake
    }
    
    return y;
}

