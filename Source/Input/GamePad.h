#pragma once

#pragma comment(lib, "xinput.lib")

#include <windows.h>
#include <Xinput.h>
#include <cstdint>

class WinApp;

class GamePad {
public:
    // プレイヤーの最大数
    static constexpr DWORD kMaxPlayers = 4;

    void Initialize(WinApp *winApp);
    void Update();
    void Finalize() {}

    bool IsConnected() const { return isConnected_; }

    // 離された状態
    bool FreeButton(WORD buttonMask) const { return !(state_.Gamepad.wButtons & buttonMask); }
    
    // 押された状態
    bool PushButton(WORD buttonMask) const { return state_.Gamepad.wButtons & buttonMask; }
    
    // 離した瞬間
    bool ReleaseButton(WORD buttonMask) const {
        return !(state_.Gamepad.wButtons & buttonMask) &&
            (preState_.Gamepad.wButtons & buttonMask); }
    
    // 押した瞬間
    bool TriggerButton(WORD buttonMask) const {
        return (state_.Gamepad.wButtons & buttonMask) && 
            !(preState_.Gamepad.wButtons & buttonMask); }

    // スティック取得
    void GetLeftStick(float &x, float &y) const;
    void GetRightStick(float &x, float &y) const;

    // トリガーの取得
    float GetLeftTrigger() const { return static_cast<float>(state_.Gamepad.bLeftTrigger) / 255.0f; }
    float GetRightTrigger() const { return static_cast<float>(state_.Gamepad.bRightTrigger) / 255.0f; }

private:
    // プレイヤー数
    DWORD playerIndex_ = 0;

    // 接続されているか否か
    bool isConnected_ = false;

    XINPUT_STATE state_ = {};
    XINPUT_STATE preState_ = {};
};