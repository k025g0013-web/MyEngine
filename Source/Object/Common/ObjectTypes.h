#pragma once

namespace Kizuna {
    enum class RenderPass {
        kDefault,
        kThroughWall,
        kShadowMap,
        kDepth,
        kOutline,
    };

    /// <summary>
    /// 描画方法を切り替えるためのレンダーレイヤ
    /// </summary>
    enum class RenderLayer {
        kDefault,      // 通常描画
        kThroughWall   // 壁越し描画対象
    };
}