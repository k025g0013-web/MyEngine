#pragma once

#include "CameraState.h"

namespace Kizuna {
    /// <summary>
    /// 通常カメラ状態
    /// </summary>
    class NormalCameraState : public CameraState {
    public:
        /// <summary>
        /// 通常カメラを更新する
        /// </summary>
        void Update(
            CameraManager &manager,
            const Transform &transform) override;

        /// <summary>
        /// デバッグカメラ状態へ切り替える
        /// </summary>
        void Toggle(CameraManager &manager) override;

        /// <summary>
        /// 使用しているカメラの名前を取得
        /// </summary>
        const char *GetName() const override {
            return "DEBUG";
        }
    };
}