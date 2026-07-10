#include "GamePad.h"
#include "Core/WinApp.h"
#include <cmath>

void GamePad::Initialize(WinApp *winApp) {
    UNREFERENCED_PARAMETER(winApp); // 将来的な拡張を考慮し引数を保持

    playerIndex_ = 0;
    isConnected_ = false;

    // 入力状態を初期化
    SecureZeroMemory(&state_, sizeof(XINPUT_STATE));
    SecureZeroMemory(&preState_, sizeof(XINPUT_STATE));

    // 起動時にゲームパッドの接続状態を確認する
    Update();
}

void GamePad::Update() {
    // トリガー・リリース判定に使用するため前フレームの状態を保存
    preState_ = state_;

    // 接続済みの場合は同じプレイヤー番号から入力を取得する
    if (isConnected_) {
        DWORD result = XInputGetState(playerIndex_, &state_);

        if (result == ERROR_SUCCESS) {
            // 正常に取得できたため更新終了
            return;
        }

        // 接続が切れたため再検索を行う
        isConnected_ = false;
    }

    // 全プレイヤー番号を検索し、最初に接続されているゲームパッドを採用する
    for (DWORD i = 0; i < kMaxPlayers; ++i) {
        DWORD result = XInputGetState(i, &state_);

        if (result == ERROR_SUCCESS) {
            playerIndex_ = i;
            isConnected_ = true;
            return;
        }
    }

    // 接続されているゲームパッドが存在しない場合は入力状態を初期化する
    isConnected_ = false;
    SecureZeroMemory(&state_, sizeof(XINPUT_STATE));
}

void GamePad::GetLeftStick(float &x, float &y) const {
    // 未接続の場合は入力なし
    if (!isConnected_) {
        x = 0.0f;
        y = 0.0f;
        return;
    }

    float sX = static_cast<float>(state_.Gamepad.sThumbLX);
    float sY = static_cast<float>(state_.Gamepad.sThumbLY);

    // 微小なスティック入力を無効化するためデッドゾーン判定を行う
    float magnitude = std::sqrt(sX * sX + sY * sY);

    if (magnitude < static_cast<float>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {
        x = 0.0f;
        y = 0.0f;
    } else {
        // -1.0～1.0の範囲へ正規化する
        x = sX / 32767.0f;
        y = sY / 32767.0f;
    }
}

void GamePad::GetRightStick(float &x, float &y) const {
    // 未接続の場合は入力なし
    if (!isConnected_) {
        x = 0.0f;
        y = 0.0f;
        return;
    }

    float sX = static_cast<float>(state_.Gamepad.sThumbRX);
    float sY = static_cast<float>(state_.Gamepad.sThumbRY);

    // 微小なスティック入力を無効化するためデッドゾーン判定を行う
    float magnitude = std::sqrt(sX * sX + sY * sY);

    if (magnitude < static_cast<float>(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) {
        x = 0.0f;
        y = 0.0f;
    } else {
        // -1.0～1.0の範囲へ正規化する
        x = sX / 32767.0f;
        y = sY / 32767.0f;
    }
}