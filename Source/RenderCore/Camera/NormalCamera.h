#pragma once

#include "Camera.h"

namespace Kizuna {
    /// <summary>
    /// シーンを描画するためのカメラを管理するクラス
    /// </summary>
    /// <remarks>
    /// 通常カメラとデバッグカメラの2種類を切り替えて使用できる。
    /// ビュー行列・射影行列の生成および、入力デバイスによる
    /// デバッグ操作を担当する。
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