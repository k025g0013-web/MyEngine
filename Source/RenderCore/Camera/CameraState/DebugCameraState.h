#pragma once

#include "CameraState.h"

namespace Kizuna {
    /// <summary>
    /// デバッグカメラ状態
    /// </summary>
    class DebugCameraState : public CameraState {
    public:
        /// <summary>
        /// デバッグカメラを更新する
        /// </summary>
        void Update(
            CameraManager &manager,
            const Transform &transform) override;

        /// <summary>
        /// 通常カメラ状態へ切り替える
        /// </summary>
        void Toggle(CameraManager &manager) override;

        /// <summary>
        /// 使用しているカメラの名前を取得
        /// </summary>
        const char *GetName() const override {
            return "NORMAL";
        }
    };
}