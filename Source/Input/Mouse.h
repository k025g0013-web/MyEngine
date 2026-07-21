#pragma once

#include <Windows.h>

namespace Kizuna {
    /// <summary>
    /// マウス入力を管理するクラス
    /// </summary>
    /// <remarks>
    /// マウスカーソルの座標、左右ボタンの状態、ホイール、
    /// ドラッグ状態を取得・管理する。
    /// 前フレームの状態も保持することで、
    /// Trigger・Releaseなどの入力判定を提供する。
    /// </remarks>
    class Mouse {
    public:
        /// <summary>
        /// マウス入力を初期化する。
        /// </summary>
        /// <param name="hwnd">入力対象となるウィンドウ</param>
        void Initialize(HWND hwnd);

        /// <summary>
        /// マウス入力状態を更新する。
        /// </summary>
        /// <remarks>
        /// カーソル位置、ボタン状態、ドラッグ状態などを取得する。
        /// </remarks>
        void Update();

        /// <summary>
        /// フレーム終了時の処理を行う。
        /// </summary>
        /// <remarks>
        /// ホイール値など1フレームだけ有効な情報をリセットする。
        /// </remarks>
        void EndFrame();

        //=========================================================
        // 左クリック
        //=========================================================

        /// <summary>左ボタンが押されているか取得する。</summary>
        bool PushLeft() const { return left_; }

        /// <summary>左ボタンが押された瞬間か取得する。</summary>
        bool TriggerLeft() const { return left_ && !preLeft_; }

        /// <summary>左ボタンが離された瞬間か取得する。</summary>
        bool ReleaseLeft() const { return !left_ && preLeft_; }

        //=========================================================
        // 右クリック
        //=========================================================

        /// <summary>右ボタンが押されているか取得する。</summary>
        bool PushRight() const { return right_; }

        /// <summary>右ボタンが押された瞬間か取得する。</summary>
        bool TriggerRight() const { return right_ && !preRight_; }

        /// <summary>右ボタンが離された瞬間か取得する。</summary>
        bool ReleaseRight() const { return !right_ && preRight_; }

        //=========================================================
        // マウス座標
        //=========================================================

        /// <summary>現在のマウス座標を取得する。</summary>
        POINT GetPosition() const { return mousePos_; }

        /// <summary>前フレームからのX方向移動量を取得する。</summary>
        LONG GetDeltaX() const { return mousePos_.x - preMousePos_.x; }

        /// <summary>前フレームからのY方向移動量を取得する。</summary>
        LONG GetDeltaY() const { return mousePos_.y - preMousePos_.y; }

        //=========================================================
        // ドラッグ
        //=========================================================

        /// <summary>ドラッグ中か取得する。</summary>
        bool IsDragging() const { return isDragging_; }

        /// <summary>ドラッグ開始位置を取得する。</summary>
        POINT GetDragStart() const { return dragStart_; }

        //=========================================================
        // ホイール
        //=========================================================

        /// <summary>ホイールの回転量を取得する。</summary>
        int GetWheelDelta() const { return wheelDelta_; }

        /// <summary>ホイールが回転した瞬間か取得する。</summary>
        bool TriggerWheel() const { return wheelDelta_ != 0 && preWheelDelta_ == 0; }

        /// <summary>ホイール回転量を設定する。</summary>
        void SetWheelDelta(int delta) { wheelDelta_ = delta; }

    private:
        // 入力対象となるウィンドウハンドル
        HWND hwnd_ = nullptr;

        //=========================================================
        // マウス座標
        //=========================================================

        // 現在のマウス座標
        POINT mousePos_{};

        // 前フレームのマウス座標
        POINT preMousePos_{};

        //=========================================================
        // ボタン状態
        //=========================================================

        // 左ボタンの現在・前フレーム状態
        bool left_ = false;
        bool preLeft_ = false;

        // 右ボタンの現在・前フレーム状態
        bool right_ = false;
        bool preRight_ = false;

        //=========================================================
        // ホイール
        //=========================================================

        // 現在・前フレームのホイール回転量
        int wheelDelta_ = 0;
        int preWheelDelta_ = 0;

        //=========================================================
        // ドラッグ
        //=========================================================

        // ドラッグ開始位置
        POINT dragStart_{};

        // ドラッグ中かどうか
        bool isDragging_ = false;
    };
}