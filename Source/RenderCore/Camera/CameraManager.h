#pragma once

#include "NormalCamera.h"
#include "DebugCamera.h"

namespace Kizuna {
    /// <summary>
    /// シーンを描画するためのカメラを管理するクラス
    /// </summary>
    /// <remarks>
    /// 通常カメラとデバッグカメラの2種類を切り替えて使用できる。
    /// ビュー行列・射影行列の生成および、入力デバイスによる
    /// デバッグ操作を担当する。
    /// </remarks>
    class CameraManager {
    public:
        /// <summary>
        /// カメラを初期化する
        /// </summary>
        /// <param name="width">画面幅</param>
        /// <param name="height">画面高さ</param>
        /// <param name="keyboard">キーボード入力</param>
        /// <param name="mouse">マウス入力</param>
        /// <param name="gamePad">ゲームパッド入力</param>
        void Initialize(
            float width,
            float height,
            Keyboard *keyboard,
            Mouse *mouse,
            GamePad *gamePad
        );

        /// <summary>
        /// カメラを更新する
        /// </summary>
        /// <param name="transform">
        /// カメラで使用するTransform
        /// </param>
        void Update(const Transform &transform);

        /// <summary>
        /// 通常カメラへ切り替え
        /// </summary>
        void SetNormalCamera();

        /// <summary>
        /// デバッグカメラへ切り替え
        /// </summary>
        void SetDebugCamera();

        /// <summary>
        /// カメラ切り替え
        /// </summary>
        void ToggleCamera();

        /// <summary>
        /// 現在使用中のカメラを取得する
        /// </summary>
        Camera *GetCamera() { return currentCamera_; }

        /// <summary>
        /// ビュー行列を取得する
        /// </summary>
        const Matrix4x4 &GetViewMatrix() const { return currentCamera_->GetViewMatrix(); }

        /// <summary>
        /// 射影行列を取得する
        /// </summary>
        const Matrix4x4 &GetProjectionMatrix() const { return currentCamera_->GetProjectionMatrix(); }

        /// <summary>
        /// ViewProjection行列を取得する
        /// </summary>
        const Matrix4x4 &GetViewProjectionMatrix() const { return currentCamera_->GetViewProjectionMatrix(); }

        /// <summary>
        /// デバッグカメラを使っているか
        /// </summary>
        bool IsDebugCamera() const { return currentCamera_ == &debugCamera_; }

    private:
        // 通常カメラ
        NormalCamera normalCamera_;
        // デバッグカメラ
        DebugCamera debugCamera_;

        // 現在使用中のカメラ
        Camera *currentCamera_ = nullptr;
    };
}