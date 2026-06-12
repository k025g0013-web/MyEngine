#include "InputMouse.h"

void InputMouse::Initialize(HWND hwnd) {
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

void InputMouse::Update() {
    preMousePos_ = mousePos_;
    preLeft_ = left_;
    preRight_ = right_;
    preWheelDelta_ = wheelDelta_;

    GetCursorPos(&mousePos_);
    ScreenToClient(hwnd_, &mousePos_);

    left_ = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    right_ = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

    if (TriggerLeft()) {
        isDragging_ = true;
        dragStart_ = mousePos_;
    }

    if (!PushLeft()) {
        isDragging_ = false;
    }
}

void InputMouse::EndFrame() {
    wheelDelta_ = 0;
}