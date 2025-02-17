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
    
public:
    SecondOrderDynamics(float f, float z, float r, Vector3<float> x0);

    void Reset(float f, float z, float r, Vector3<float> x0);

    Vector3<float> Calculate(float T, Vector3<float> x, Vector3<float> xd);
};
