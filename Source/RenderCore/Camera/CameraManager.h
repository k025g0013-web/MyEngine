#pragma once

#include "NormalCamera.h"
#include "DebugCamera.h"
#include "CameraState/CameraState.h"

namespace Kizuna {
    /// <summary>
    /// カメラの切り替えと更新を管理するクラス
    /// </summary>
    /// <remarks>
    /// 通常カメラとデバッグカメラを保持し、
    /// 現在使用するカメラの切り替えと更新を行う。
    /// 外部からは現在有効なカメラの行列を取得できる。
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
        void Initialize(float width, float height,
            Keyboard *keyboard, Mouse *mouse, GamePad *gamePad);

        /// <summary>
        /// 現在使用中のカメラを更新する
        /// </summary>
        /// <param name="transform">
        /// カメラで使用するTransform
        /// </param>
        void Update(const Transform &transform);

        /// <summary>
        /// 通常カメラとデバッグカメラを切り替える
        /// </summary>
        void ToggleCamera();

        /// <summary>
        /// 現在使用中のカメラを取得する
        /// </summary>
        Camera *GetCamera() { return currentCamera_; }

        /// <summary>
        /// 通常カメラを取得する
        /// </summary>
        NormalCamera *GetNormalCamera() { return &normalCamera_; }

        /// <summary>
        /// 通常カメラを取得する
        /// </summary>
        DebugCamera *GetDebugCamera() { return &debugCamera_; }

        /// <summary>
        /// 通常カメラを取得する
        /// </summary>
        void SetCurrentCamera(Camera *camera) { currentCamera_ = camera; }
        
        /// <summary>
        /// カメラの状態を切り替える
        /// </summary>
        void ChangeState(CameraState *state) { state_ = state; }

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

        /// <summary>
        /// 使用しているカメラの名前を取得
        /// </summary>
        const char *GetStateName() const {
            return state_->GetName();
        }

    private:
        // 通常カメラ
        NormalCamera normalCamera_;
        // デバッグカメラ
        DebugCamera debugCamera_;

        // 現在使用中のカメラ
        Camera *currentCamera_ = nullptr;

        // カメラの状態
        CameraState *state_ = nullptr;
    };
}