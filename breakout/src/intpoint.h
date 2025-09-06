#pragma once

#include "common.h"

struct IntPoint {
    int x;
    int y;
    IntPoint(int x, int y) : x(x), y(y) {}

    IntPoint operator*(IntPoint rhs) { return {x * rhs.x, y * rhs.y}; }
    IntPoint operator+(IntPoint rhs) { return {x + rhs.x, y + rhs.y}; }
    IntPoint operator-(IntPoint rhs) { return {x - rhs.x, y - rhs.y}; }
    IntPoint operator*(float rhs)     { return {int(x*rhs), int(y*rhs)}; }

    operator DirectX::XMFLOAT3() { return {float(x), float(y), 0.0}; }
};