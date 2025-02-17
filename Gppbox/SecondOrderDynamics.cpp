#include "SecondOrderDynamics.h"

#include <complex>

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


Vector3<float> SecondOrderDynamics::Calculate(float T, Vector3<float> x, Vector3<float> xd)
{
    float k2_stable = std::max({k2, T*T/2 + T*k1/2, T*k1});
    //float k2_stable = max(k2, T*T/2 + T*k1/2);
    y += T * yd;
    yd += T * (x + k3*xd - y - k1*yd) / k2_stable;
    return y;
}

