#pragma once

#include "Input/Keyboard.h"
#include "Input/GamePad.h"
#include "Input/Mouse.h"

namespace Kizuna {
    class WinApp;

    class InputManager {
    public:
        void Initialize(WinApp *winApp);
        void Update();
        void EndFrame();
        void Finalize();

        /* Getter
        --------------------*/
        /// keyboardGetter
        Keyboard *GetKeyboard() { return &keyboard_; }
        /// gamePadGetter
        GamePad *GetGamePad() { return &gamePad_; }
        /// mouseGetter
        Mouse *GetMouse() { return &mouse_; }

        /* Keyboard
        --------------------*/
        /// キーが離されているか取得する
        bool FreeKey(BYTE key);
        /// キーが押されているか取得する
        bool PushKey(BYTE key);
        /// キーが離された瞬間か取得する
        bool ReleaseKey(BYTE key);
        /// キーが押された瞬間か取得する
        bool TriggerKey(BYTE key);

        /* GamePad
        --------------------*/
        /// ゲームパッドが接続されているか取得する
        bool IsConnected();

        /// ボタンが離されているか取得する
        /// <param name="buttonMask">判定するボタン</param>
        bool FreeButton(WORD button);
        /// ボタンが押されているか取得する
        /// <param name="buttonMask">判定するボタン</param>
        bool PushButton(WORD button);
        /// ボタンを離した瞬間か取得する
        /// <param name="buttonMask">判定するボタン</param>
        bool ReleaseButton(WORD button);
        /// ボタンを押した瞬間か取得する
        /// <param name="buttonMask">判定するボタン</param>
        bool TriggerButton(WORD button);

        /// 左スティックの入力値を取得する
        /// <param name="x">X方向の入力値</param>
        /// <param name="y">Y方向の入力値</param>
        void GetLeftStick(float &x, float &y);
        /// 右スティックの入力値を取得する
        /// <param name="x">X方向の入力値</param>
        /// <param name="y">Y方向の入力値</param>
        void GetRightStick(float &x, float &y);
        /// 左トリガーの入力値を取得する
        float GetLeftTrigger();
        /// 右トリガーの入力値を取得する
        float GetRightTrigger();

        /// ゲームパッドの振動を設定する
        /// <param name="leftMotorSpeed">左モーターの強度（0～65535）</param>
        /// <param name="rightMotorSpeed">右モーターの強度（0～65535）</param>
        bool SetGamePadVibration(WORD leftMotorSpeed, WORD rightMotorSpeed);
        /// ゲームパッドの振動を停止する
        bool StopGamePadVibration();

        /* Mouse
        --------------------*/
        /// 左ボタンが押された瞬間か取得する
        bool TriggerMouseLeft();
        /// 左ボタンが押されているか取得する
        bool PushMouseLeft();
        /// 左ボタンが離された瞬間か取得する
        bool ReleaseMouseLeft();
        
        /// 右ボタンが押された瞬間か取得する
        bool TriggerMouseRight();
        /// 右ボタンが押されているか取得する
        bool PushMouseRight();
        /// 右ボタンが離された瞬間か取得する
        bool ReleaseMouseRight();

        /// 現在のマウス座標を取得する
        POINT GetPosition();
        /// 前フレームからのX方向移動量を取得する
        LONG GetDeltaX();
        /// 前フレームからのY方向移動量を取得する
        LONG GetDeltaY();

        /// ドラッグ中か取得する
        bool IsDragging();
        /// ドラッグ開始位置を取得する
        POINT GetDragStart();

        /// ホイールの回転量を取得する
        int GetWheelDelta();
        /// ホイールが回転した瞬間か取得する
        bool TriggerWheel();
        /// ホイール回転量を設定する
        /// <param name="delta">設定する回転量</param>
        void SetWheelDelta(int delta);

    private:
        Keyboard keyboard_;
        GamePad gamePad_;
        Mouse mouse_;
    };
}