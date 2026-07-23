#pragma once

#include "Camera.h"

#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/GamePad.h"

namespace Kizuna {
    /// <summary>
    /// シーンを描画するためのカメラを管理するクラス
    /// </summary>
    /// <remarks>
    /// 通常カメラとデバッグカメラの2種類を切り替えて使用できる。
    /// ビュー行列・射影行列の生成および、入力デバイスによる
    /// デバッグ操作を担当する。
    /// </remarks>
    class DebugCamera : public Camera {
    public:

        /// <summary>
        /// デバッグカメラを初期化する
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
        /// デバッグカメラの平行移動
        /// </summary>
        void DebugMove();

        /// <summary>
        /// デバッグカメラのズーム
        /// </summary>
        void DebugZoom();

        /// <summary>
        /// デバッグカメラの回転
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