#include "Input/Mouse.h"

namespace Kizuna {
    void Mouse::Initialize(HWND hwnd) {
        // 入力対象となるウィンドウを保持
        hwnd_ = hwnd;

        // 初期マウス座標を取得
        GetCursorPos(&mousePos_);

        // スクリーン座標をクライアント座標へ変換
        ScreenToClient(hwnd_, &mousePos_);

        // 初回の移動量が発生しないよう前フレーム座標を現在値で初期化
        preMousePos_ = mousePos_;

        // ボタン状態を初期化
        left_ = false;
        preLeft_ = false;

        right_ = false;
        preRight_ = false;

        // ホイール状態を初期化
        wheelDelta_ = 0;
        preWheelDelta_ = 0;
    }

    void Mouse::Update() {
        // Triggerや移動量を判定するため前フレームの状態を保存
        preMousePos_ = mousePos_;
        preLeft_ = left_;
        preRight_ = right_;
        preWheelDelta_ = wheelDelta_;

        //=========================================================
        // マウス座標取得
        //=========================================================

        // スクリーン座標を取得
        GetCursorPos(&mousePos_);

        // ウィンドウ内座標へ変換
        ScreenToClient(hwnd_, &mousePos_);

        //=========================================================
        // ボタン状態取得
        //=========================================================

        // 左右ボタンが押されているか取得
        left_ = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        right_ = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

        //=========================================================
        // ドラッグ判定
        //=========================================================

        // 左クリックされた瞬間をドラッグ開始とする
        if (TriggerLeft()) {
            isDragging_ = true;

            // ドラッグ開始位置を記録
            dragStart_ = mousePos_;
        }

        // 左ボタンを離したらドラッグ終了
        if (!PushLeft()) {
            isDragging_ = false;
        }
    }

    void Mouse::EndFrame() {
        // Trigger判定用に前フレーム値を保存
        preWheelDelta_ = wheelDelta_;

        // ホイール入力をリセット
        wheelDelta_ = 0;
    }
}