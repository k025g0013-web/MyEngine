#pragma once

#include "Camera.h"

#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/GamePad.h"

namespace Kizuna {
    /// <summary>
    /// デバッグ操作用のカメラクラス
    /// </summary>
    /// <remarks>
    /// キーボード・マウス・ゲームパッドの入力を用いて
    /// カメラを自由に移動・回転・ズームさせる。
    /// 更新したTransformからビュー行列を生成する。
    /// </remarks>
    class DebugCamera : public Camera {
    public:

        /// <summary>
        /// デバッグ操作に使用する入力デバイスを設定する
        /// </summary>
        /// <param name="keyboard">キーボード入力</param>
        /// <param name="mouse">マウス入力</param>
        /// <param name="gamePad">ゲームパッド入力</param>
        void SetInputDevice(
            Keyboard *keyboard, Mouse *mouse, GamePad *gamePad);

        /// <summary>
        /// デバッグカメラを更新する
        /// </summary>
        /// <param name="transform">
        /// カメラで使用するTransform
        /// </param>
        void Update(const Transform &transform) override;

    private:
        /// <summary>
        /// 注視点を平行移動する
        /// </summary>
        void DebugMove();

        /// <summary>
        /// カメラとの距離を変更する
        /// </summary>
        void DebugZoom();

        /// <summary>
        /// カメラの向きを回転させる
        /// </summary>
        void DebugRotate();

    private:
        /// 入力デバイス
        Keyboard *keyboard_ = nullptr;
        Mouse *mouse_ = nullptr;
        GamePad *gamePad_ = nullptr;

        /// デバッグカメラの注視点
        Vector3 debugTarget_{ 0.0f, 0.0f, 0.0f };
        /// 注視点からの画面移動量
        Vector3 debugScreenOffset_{};
        /// デバッグカメラ位置
        Vector3 debugTranslation_{};
        /// デバッグカメラ回転
        Vector3 debugRotation_{};

        /// カメラと注視点との距離
        float debugDistance_ = -5.0f;

        /// 平行移動速度
        float debugMoveSpeed_ = 0.05f;
        /// 回転速度
        float debugRotateSpeed_ = 0.005f;
    };
}