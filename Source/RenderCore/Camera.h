#pragma once

#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/GamePad.h"

#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Transform.h"

/// <summary>
/// シーンを描画するためのカメラを管理するクラス
/// </summary>
/// <remarks>
/// 通常カメラとデバッグカメラの2種類を切り替えて使用できる。
/// ビュー行列・射影行列の生成および、入力デバイスによる
/// デバッグ操作を担当する。
/// </remarks>
class Camera {
public:

    /// <summary>
    /// カメラの動作モード
    /// </summary>
    enum class Mode {
        Normal,    ///< 通常カメラ
        Debug,     ///< デバッグカメラ
    };

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
    /// <param name="normalTargetTransform">
    /// 通常カメラで使用するカメラTransform
    /// </param>
    void Update(const Transform &normalTargetTransform);

    /// <summary>
    /// カメラモードを変更する
    /// </summary>
    void SetMode(Mode mode) { mode_ = mode; }

    /// <summary>
    /// 現在のカメラモードを取得する
    /// </summary>
    Mode GetMode() const { return mode_; }

    /// <summary>
    /// 通常カメラとデバッグカメラを切り替える
    /// </summary>
    void ToggleMode();

    /// ビュー行列を取得する
    const Matrix4x4 &GetViewMatrix() const { return viewMatrix_; }

    /// 射影行列を取得する
    const Matrix4x4 &GetProjectionMatrix() const { return projectionMatrix_; }

    /// ViewProjection行列を取得する
    const Matrix4x4 &GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

    /// デバッグカメラの注視点を取得する
    const Vector3 &GetDebugTarget() const { return debugTarget_; }

    /// デバッグカメラの回転角を取得する
    const Vector3 &GetDebugRotation() const { return debugRotation_; }

    /// デバッグカメラの距離を取得する
    const float &GetDebugDistance() const { return debugDistance_; }

private:

    /// 通常カメラを更新する
    void UpdateCamera(const Transform &targetTransform);

    /// デバッグカメラを更新する
    void UpdateDebug();

    /// デバッグカメラの平行移動
    void DebugMove();

    /// デバッグカメラのズーム
    void DebugZoom();

    /// デバッグカメラの回転
    void DebugRotate();

private:

    /// 現在のカメラモード
    Mode mode_ = Mode::Normal;

    /// ビュー行列
    Matrix4x4 viewMatrix_{};

    /// 射影行列
    Matrix4x4 projectionMatrix_{};

    /// ViewProjection行列
    Matrix4x4 viewProjectionMatrix_{};

    /// 画面サイズ
    float width_ = 1280.0f;
    float height_ = 720.0f;

    /// カメラ設定
    float fovY_ = 0.45f;
    float nearClip_ = 0.1f;
    float farClip_ = 100.0f;

    /// 入力デバイス
    Keyboard *keyboard_ = nullptr;
    Mouse *mouse_ = nullptr;
    GamePad *gamePad_ = nullptr;

    /// デバッグカメラの注視点
    Vector3 debugTarget_ = { 0.0f, 0.0f, 0.0f };

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