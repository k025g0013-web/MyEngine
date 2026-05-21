#pragma once

#include <cstdint>

#include "Vector4.h"

inline Vector4 UintToVector4(uint32_t color) {
    Vector4 result;

    result.x = ((color >> 24) & 0xFF) / 255.0f;
    result.y = ((color >> 16) & 0xFF) / 255.0f;
    result.z = ((color >> 8) & 0xFF) / 255.0f;
    result.w = ((color >> 0) & 0xFF) / 255.0f;

    return result;
}