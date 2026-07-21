#pragma once
#include <cstdint>
#include "Math/Vector.h"

namespace Kizuna {
    /// <summary>
    /// 32bitカラー値をVector4(RGBA)へ変換するユーティリティ関数
    /// </summary>
    /// <param name="color">
    /// 0xRRGGBBAA形式の32bitカラー値
    /// </param>
    /// <returns>
    /// 各成分を0.0～1.0へ正規化したRGBAカラー
    /// </returns>
    /// <remarks>
    /// シェーダーへ渡す色は浮動小数点形式が必要になるため、
    /// 整数カラー値を0～1の範囲へ変換して返す。
    /// </remarks>
    inline Vector4 UintToVector4(uint32_t color) {
        return {
            { ((color >> 24) & 0xFF) / 255.0f },
            { ((color >> 16) & 0xFF) / 255.0f },
            { ((color >> 8) & 0xFF) / 255.0f },
            { ((color >> 0) & 0xFF) / 255.0f },
        };
    }
}