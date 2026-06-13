#include "Input/Mouse.h"

void Mouse::Initialize(HWND hwnd) {
    hwnd_ = hwnd;

    GetCursorPos(&mousePos_);
    ScreenToClient(hwnd_, &mousePos_);

    preMousePos_ = mousePos_;

    left_ = false;
    preLeft_ = false;

    right_ = false;
    preRight_ = false;

    wheelDelta_ = 0;
    preWheelDelta_ = 0;
}

void Mouse::Update() {
    preMousePos_ = mousePos_;
    preLeft_ = left_;
    preRight_ = right_;
    preWheelDelta_ = wheelDelta_;

    // マウス座標取得
    GetCursorPos(&mousePos_);
    ScreenToClient(hwnd_, &mousePos_);

    // ボタン状態チェック
    left_ = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    right_ = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

    // ドラッグ開始チェック
    if (TriggerLeft()) {
        isDragging_ = true;
        dragStart_ = mousePos_;
    }

    if (!PushLeft()) {
        isDragging_ = false;
    }
}

void Mouse::EndFrame() {
    wheelDelta_ = 0;
}