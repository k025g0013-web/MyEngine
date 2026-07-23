#pragma once

#include "Camera.h"

namespace Kizuna {
    /// <summary>
    /// 通常視点用のカメラクラス
    /// </summary>
    /// <remarks>
    /// 指定されたTransformをそのままカメラとして使用し、
    /// ビュー行列および射影行列を更新する。
    /// プレイヤー追従カメラなどの通常描画に使用する。
    /// </remarks>
    class NormalCamera : public Camera {
    public:
        /// <summary>
        /// 通常カメラを更新する
        /// </summary>
        /// <param name="transform">
        /// カメラで使用するTransform
        /// </param>
        void Update(const Transform &transform) override;
    };
}