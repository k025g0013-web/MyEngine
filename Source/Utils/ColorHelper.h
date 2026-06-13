#pragma once

#include <cstdint>

#include "Math/Vector.h"

inline Vector4 UintToVector4(uint32_t color) {
    return { 
        { ((color >> 24) & 0xFF) / 255.0f },
        { ((color >> 16) & 0xFF) / 255.0f },
        { ((color >>  8) & 0xFF) / 255.0f },
        { ((color >>  0) & 0xFF) / 255.0f },
    };
}