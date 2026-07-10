#pragma once

#pragma comment(lib, "xinput.lib")

#include <windows.h>
#include <Xinput.h>
#include <cstdint>

class WinApp;

/// <summary>
/// ゲームパッド入力を管理するクラス
/// </summary>
/// <remarks>
/// XInputを利用してゲームパッドの入力状態を取得する。
/// ボタンの押下・離した瞬間・押し続けている状態に加え、
/// スティックやトリガーの値も取得できる。
/// </remarks>
class GamePad {
public:
    /// 接続可能な最大プレイヤー数
    static constexpr DWORD kMaxPlayers = 4;

    /// <summary>
    /// ゲームパッド入力システムを初期化する
    /// </summary>
    /// <param name="winApp">ウィンドウ情報</param>
    void Initialize(WinApp *winApp);

    /// <summary>
    /// ゲームパッド入力状態を更新する
    /// </summary>
    /// <remarks>
    /// 前フレームの入力状態を保存した後、
    /// 現在の入力状態を取得する。
    /// </remarks>
    void Update();

    /// <summary>
    /// ゲームパッド入力システムを終了する
    /// </summary>
    void Finalize() {}

    /// <summary>
    /// ゲームパッドが接続されているか取得する
    /// </summary>
    /// <returns>接続されていればtrue</returns>
    bool IsConnected() const { return isConnected_; }

    /// <summary>
    /// ボタンが離されているか取得する
    /// </summary>
    /// <param name="buttonMask">判定するボタン</param>
    /// <returns>離されていればtrue</returns>
    bool FreeButton(WORD buttonMask) const {
        return !(state_.Gamepad.wButtons & buttonMask);
    }

    /// <summary>
    /// ボタンが押されているか取得する
    /// </summary>
    /// <param name="buttonMask">判定するボタン</param>
    /// <returns>押されていればtrue</returns>
    bool PushButton(WORD buttonMask) const {
        return state_.Gamepad.wButtons & buttonMask;
    }

    /// <summary>
    /// ボタンを離した瞬間か取得する
    /// </summary>
    /// <param name="buttonMask">判定するボタン</param>
    /// <returns>離した瞬間ならtrue</returns>
    bool ReleaseButton(WORD buttonMask) const {
        return !(state_.Gamepad.wButtons & buttonMask) &&
            (preState_.Gamepad.wButtons & buttonMask);
    }

    /// <summary>
    /// ボタンを押した瞬間か取得する
    /// </summary>
    /// <param name="buttonMask">判定するボタン</param>
    /// <returns>押した瞬間ならtrue</returns>
    bool TriggerButton(WORD buttonMask) const {
        return (state_.Gamepad.wButtons & buttonMask) &&
            !(preState_.Gamepad.wButtons & buttonMask);
    }

    /// <summary>
    /// 左スティックの入力値を取得する
    /// </summary>
    /// <param name="x">X方向の入力値</param>
    /// <param name="y">Y方向の入力値</param>
    void GetLeftStick(float &x, float &y) const;

    /// <summary>
    /// 右スティックの入力値を取得する
    /// </summary>
    /// <param name="x">X方向の入力値</param>
    /// <param name="y">Y方向の入力値</param>
    void GetRightStick(float &x, float &y) const;

    /// <summary>
    /// 左トリガーの入力値を取得する
    /// </summary>
    /// <returns>0.0～1.0の入力値</returns>
    float GetLeftTrigger() const {
        return static_cast<float>(state_.Gamepad.bLeftTrigger) / 255.0f;
    }

    /// <summary>
    /// 右トリガーの入力値を取得する
    /// </summary>
    /// <returns>0.0～1.0の入力値</returns>
    float GetRightTrigger() const {
        return static_cast<float>(state_.Gamepad.bRightTrigger) / 255.0f;
    }

private:
    /// 使用するプレイヤー番号
    DWORD playerIndex_ = 0;

    /// ゲームパッドの接続状態
    bool isConnected_ = false;

    /// 現在の入力状態
    XINPUT_STATE state_ = {};

    /// 前フレームの入力状態
    XINPUT_STATE preState_ = {};
};