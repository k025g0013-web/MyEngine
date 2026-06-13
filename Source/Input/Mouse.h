#pragma once

#include <Windows.h>

class Mouse {
public:
    void Initialize(HWND hwnd);
    void Update();
    void EndFrame();

    // 左クリック
    bool PushLeft() const { return left_; }
    bool TriggerLeft() const { return left_ && !preLeft_; }
    bool ReleaseLeft() const { return !left_ && preLeft_; }

    // 右クリック
    bool PushRight() const { return right_; }
    bool TriggerRight() const { return right_ && !preRight_; }
    bool ReleaseRight() const { return !right_ && preRight_; }

    // 座標・移動量
    POINT GetPosition() const { return mousePos_; }
    LONG GetDeltaX() const { return mousePos_.x - preMousePos_.x; }
    LONG GetDeltaY() const { return mousePos_.y - preMousePos_.y; }

    bool IsDragging() const { return isDragging_; }
    POINT GetDragStart() const { return dragStart_; }

    // ホイール
    int GetWheelDelta() const { return wheelDelta_; }
    bool TriggerWheel() const { return wheelDelta_ != 0 && preWheelDelta_ == 0; }
    void SetWheelDelta(int delta) { wheelDelta_ += delta; }

private:
    HWND hwnd_ = nullptr;

    // MousePos
    POINT mousePos_{};
    POINT preMousePos_{};

    // Button
    bool left_ = false;
    bool preLeft_ = false;

    bool right_ = false;
    bool preRight_ = false;

    // Wheel
    int wheelDelta_ = 0;
    int preWheelDelta_ = 0;

    // Drag
    POINT dragStart_{};
    bool isDragging_ = false;
};