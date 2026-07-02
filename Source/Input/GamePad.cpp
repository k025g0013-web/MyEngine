#include "GamePad.h"
#include "Core/WinApp.h"
#include <cmath>

void GamePad::Initialize(WinApp *winApp) {
    UNREFERENCED_PARAMETER(winApp); // 警告回避

    playerIndex_ = 0;
    isConnected_ = false;
    SecureZeroMemory(&state_, sizeof(XINPUT_STATE));
    SecureZeroMemory(&preState_, sizeof(XINPUT_STATE));

    Update();
}

void GamePad::Update() {
    preState_ = state_;

    // すでに接続されている場合は、その番号で状態を更新
    if (isConnected_) {
        DWORD result = XInputGetState(playerIndex_, &state_);
        if (result == ERROR_SUCCESS) {
            return; // 正常に更新できたらここで終了
        }
        // 切断された場合はフラグを落として再検索へ進む
        isConnected_ = false;
    }

    // 接続されていない場合、空いているポートを自動検索
    for (DWORD i = 0; i < kMaxPlayers; ++i) {
        DWORD result = XInputGetState(i, &state_);
        if (result == ERROR_SUCCESS) {
            playerIndex_ = i; // 見つかった番号を保存
            isConnected_ = true;
            return;
        }
    }
    
    // どこにも刺さっていなければ完全に切断
    isConnected_ = false;
    SecureZeroMemory(&state_, sizeof(XINPUT_STATE));
}

void GamePad::GetLeftStick(float &x, float &y) const {
    if (!isConnected_) { x = 0.0f; y = 0.0f; return; }

    float sX = static_cast<float>(state_.Gamepad.sThumbLX);
    float sY = static_cast<float>(state_.Gamepad.sThumbLY);

    // デッドゾーン処理
    float magnitude = std::sqrt(sX * sX + sY * sY);

    if (magnitude < static_cast<float>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {
        x = 0.0f;
        y = 0.0f;
    } else {
        x = sX / 32767.0f;
        y = sY / 32767.0f;
    }
}

void GamePad::GetRightStick(float &x, float &y) const {
    if (!isConnected_) { x = 0.0f; y = 0.0f; return; }

    float sX = static_cast<float>(state_.Gamepad.sThumbRX);
    float sY = static_cast<float>(state_.Gamepad.sThumbRY);

    // デッドゾーン処理
    float magnitude = std::sqrt(sX * sX + sY * sY);

    if (magnitude < static_cast<float>(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) {
        x = 0.0f;
        y = 0.0f;
    } else {
        x = sX / 32767.0f;
        y = sY / 32767.0f;
    }
}