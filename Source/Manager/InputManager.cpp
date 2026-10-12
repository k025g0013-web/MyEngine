#include "InputManager.h"
#include "Core/WinApp.h"

#include <cassert>

namespace Kizuna {

    void InputManager::Initialize(WinApp *winApp) {
        assert(winApp != nullptr);

        keyboard_.Initialize(winApp);
        gamePad_.Initialize(winApp);
        mouse_.Initialize(winApp->GetHWND());
    }

    void InputManager::Update() {
        keyboard_.Update();
        gamePad_.Update();
        mouse_.Update();
    }

    void InputManager::EndFrame() {
        mouse_.EndFrame();
    }

    void InputManager::Finalize() {
        keyboard_.Finalize();
        gamePad_.Finalize();
    }

    // Keyboard
    bool InputManager::FreeKey(BYTE key) {
        return keyboard_.FreeKey(key);
    }

    bool InputManager::PushKey(BYTE key) {
        return keyboard_.PushKey(key);
    }

    bool InputManager::ReleaseKey(BYTE key) {
        return keyboard_.ReleaseKey(key);
    }

    bool InputManager::TriggerKey(BYTE key) {
        return keyboard_.TriggerKey(key);
    }

    // GamePad
    bool InputManager::IsConnected() {
        return gamePad_.IsConnected();
    }

    bool InputManager::FreeButton(WORD button) {
        return gamePad_.FreeButton(button);
    }

    bool InputManager::PushButton(WORD button) {
        return gamePad_.PushButton(button);
    }

    bool InputManager::ReleaseButton(WORD button) {
        return gamePad_.ReleaseButton(button);
    }

    bool InputManager::TriggerButton(WORD button) {
        return gamePad_.TriggerButton(button);
    }

    void InputManager::GetLeftStick(float &x, float &y) {
        gamePad_.GetLeftStick(x, y);
    }
    
    void  InputManager::GetRightStick(float &x, float &y) {
        gamePad_.GetRightStick(x, y);
    }

    float  InputManager::GetLeftTrigger() {
        return gamePad_.GetLeftTrigger();
    }

    float  InputManager::GetRightTrigger() {
        return gamePad_.GetRightTrigger();
    }

    bool InputManager::SetGamePadVibration(
        WORD leftMotorSpeed, WORD rightMotorSpeed) {
        return gamePad_.SetVibration(leftMotorSpeed, rightMotorSpeed);
    }

    bool InputManager::StopGamePadVibration() {
        return gamePad_.StopVibration();
    }


    // Mouse
    bool InputManager::TriggerMouseLeft() {
        return mouse_.TriggerLeft();
    }

    bool InputManager::ReleaseMouseLeft() {
        return mouse_.ReleaseLeft();
    }

    bool InputManager::PushMouseLeft() {
        return mouse_.PushLeft();
    }

    bool InputManager::TriggerMouseRight() {
        return mouse_.TriggerRight();
    }

    bool InputManager::ReleaseMouseRight() {
        return mouse_.ReleaseRight();
    }

    bool InputManager::PushMouseRight() {
        return mouse_.PushRight();
    }

    POINT InputManager::GetPosition() {
        return mouse_.GetPosition();
    }

    LONG InputManager::GetDeltaX() {
        return mouse_.GetDeltaX();
    }

    LONG InputManager::GetDeltaY() {
        return mouse_.GetDeltaY();
    }

    bool InputManager::IsDragging() {
        return mouse_.IsDragging();
    }

    POINT InputManager::GetDragStart() {
        return mouse_.GetDragStart();
    }

    int InputManager::GetWheelDelta() {
        return mouse_.GetWheelDelta();
    }

    bool InputManager::TriggerWheel() {
        return mouse_.TriggerWheel();
    }

    void InputManager::SetWheelDelta(int delta) {
        mouse_.SetWheelDelta(delta);
    }

}