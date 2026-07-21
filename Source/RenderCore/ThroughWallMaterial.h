#pragma once

#include "Math/Vector.h"
#include <cstdint>

namespace Kizuna {
    /// <summary>
    /// 壁越し描画専用マテリアル
    /// </summary>
    struct ThroughWallMaterial {
        /// 壁越し描画時の色
        Vector4 color;

        /// 描画スタイル
        uint32_t style;
    };
}